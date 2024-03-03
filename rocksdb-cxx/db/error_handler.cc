//  Copyright (c) 2018-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
#include "db/error_handler.h"

#include "db/db_impl/db_impl.h"
#include "db/event_helpers.h"
#include "file/sst_file_manager_impl.h"
#include "logging/logging.h"
#include "port/lang.h"

namespace rocksdb {

// Maps to help decide the severity of an error based on the
// BackgroundErrorReason, Code, SubCode and whether db_options.paranoid_checks
// is set or not. There are 3 maps, going from most specific to least specific
// (i.e from all 4 fields in a tuple to only the BackgroundErrorReason and
// paranoid_checks). The less specific map serves as a catch all in case we miss
// a specific error code or subcode.
std::map<std::tuple<BackgroundErrorReason, rocksdb_rs::status::Code, rocksdb_rs::status::SubCode, bool>,
         rocksdb_rs::status::Severity>
    ErrorSeverityMap = {
        // Errors during BG compaction
        {std::make_tuple(BackgroundErrorReason::kCompaction,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace,
                         true),
         rocksdb_rs::status::Severity::kSoftError},
        {std::make_tuple(BackgroundErrorReason::kCompaction,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace,
                         false),
         rocksdb_rs::status::Severity::kNoError},
        {std::make_tuple(BackgroundErrorReason::kCompaction,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kSpaceLimit,
                         true),
         rocksdb_rs::status::Severity::kHardError},
        {std::make_tuple(BackgroundErrorReason::kCompaction,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced,
                         true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kCompaction,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced,
                         false),
         rocksdb_rs::status::Severity::kFatalError},
        // Errors during BG flush
        {std::make_tuple(BackgroundErrorReason::kFlush, rocksdb_rs::status::Code::kIOError,
                         rocksdb_rs::status::SubCode::kNoSpace, true),
         rocksdb_rs::status::Severity::kHardError},
        {std::make_tuple(BackgroundErrorReason::kFlush, rocksdb_rs::status::Code::kIOError,
                         rocksdb_rs::status::SubCode::kNoSpace, false),
         rocksdb_rs::status::Severity::kNoError},
        {std::make_tuple(BackgroundErrorReason::kFlush, rocksdb_rs::status::Code::kIOError,
                         rocksdb_rs::status::SubCode::kSpaceLimit, true),
         rocksdb_rs::status::Severity::kHardError},
        {std::make_tuple(BackgroundErrorReason::kFlush, rocksdb_rs::status::Code::kIOError,
                         rocksdb_rs::status::SubCode::kIOFenced, true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kFlush, rocksdb_rs::status::Code::kIOError,
                         rocksdb_rs::status::SubCode::kIOFenced, false),
         rocksdb_rs::status::Severity::kFatalError},
        // Errors during Write
        {std::make_tuple(BackgroundErrorReason::kWriteCallback,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace,
                         true),
         rocksdb_rs::status::Severity::kHardError},
        {std::make_tuple(BackgroundErrorReason::kWriteCallback,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace,
                         false),
         rocksdb_rs::status::Severity::kHardError},
        {std::make_tuple(BackgroundErrorReason::kWriteCallback,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced,
                         true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kWriteCallback,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced,
                         false),
         rocksdb_rs::status::Severity::kFatalError},
        // Errors during MANIFEST write
        {std::make_tuple(BackgroundErrorReason::kManifestWrite,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace,
                         true),
         rocksdb_rs::status::Severity::kHardError},
        {std::make_tuple(BackgroundErrorReason::kManifestWrite,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace,
                         false),
         rocksdb_rs::status::Severity::kHardError},
        {std::make_tuple(BackgroundErrorReason::kManifestWrite,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced,
                         true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kManifestWrite,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced,
                         false),
         rocksdb_rs::status::Severity::kFatalError},
        // Errors during BG flush with WAL disabled
        {std::make_tuple(BackgroundErrorReason::kFlushNoWAL,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace,
                         true),
         rocksdb_rs::status::Severity::kHardError},
        {std::make_tuple(BackgroundErrorReason::kFlushNoWAL,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace,
                         false),
         rocksdb_rs::status::Severity::kNoError},
        {std::make_tuple(BackgroundErrorReason::kFlushNoWAL,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kSpaceLimit,
                         true),
         rocksdb_rs::status::Severity::kHardError},
        {std::make_tuple(BackgroundErrorReason::kFlushNoWAL,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced,
                         true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kFlushNoWAL,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced,
                         false),
         rocksdb_rs::status::Severity::kFatalError},
        // Errors during MANIFEST write when WAL is disabled
        {std::make_tuple(BackgroundErrorReason::kManifestWriteNoWAL,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace,
                         true),
         rocksdb_rs::status::Severity::kHardError},
        {std::make_tuple(BackgroundErrorReason::kManifestWriteNoWAL,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace,
                         false),
         rocksdb_rs::status::Severity::kHardError},
        {std::make_tuple(BackgroundErrorReason::kManifestWriteNoWAL,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced,
                         true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kManifestWriteNoWAL,
                         rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced,
                         false),
         rocksdb_rs::status::Severity::kFatalError},

};

std::map<std::tuple<BackgroundErrorReason, rocksdb_rs::status::Code, bool>,
         rocksdb_rs::status::Severity>
    DefaultErrorSeverityMap = {
        // Errors during BG compaction
        {std::make_tuple(BackgroundErrorReason::kCompaction,
                         rocksdb_rs::status::Code::kCorruption, true),
         rocksdb_rs::status::Severity::kUnrecoverableError},
        {std::make_tuple(BackgroundErrorReason::kCompaction,
                         rocksdb_rs::status::Code::kCorruption, false),
         rocksdb_rs::status::Severity::kNoError},
        {std::make_tuple(BackgroundErrorReason::kCompaction,
                         rocksdb_rs::status::Code::kIOError, true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kCompaction,
                         rocksdb_rs::status::Code::kIOError, false),
         rocksdb_rs::status::Severity::kNoError},
        // Errors during BG flush
        {std::make_tuple(BackgroundErrorReason::kFlush,
                         rocksdb_rs::status::Code::kCorruption, true),
         rocksdb_rs::status::Severity::kUnrecoverableError},
        {std::make_tuple(BackgroundErrorReason::kFlush,
                         rocksdb_rs::status::Code::kCorruption, false),
         rocksdb_rs::status::Severity::kNoError},
        {std::make_tuple(BackgroundErrorReason::kFlush, rocksdb_rs::status::Code::kIOError,
                         true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kFlush, rocksdb_rs::status::Code::kIOError,
                         false),
         rocksdb_rs::status::Severity::kNoError},
        // Errors during Write
        {std::make_tuple(BackgroundErrorReason::kWriteCallback,
                         rocksdb_rs::status::Code::kCorruption, true),
         rocksdb_rs::status::Severity::kUnrecoverableError},
        {std::make_tuple(BackgroundErrorReason::kWriteCallback,
                         rocksdb_rs::status::Code::kCorruption, false),
         rocksdb_rs::status::Severity::kNoError},
        {std::make_tuple(BackgroundErrorReason::kWriteCallback,
                         rocksdb_rs::status::Code::kIOError, true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kWriteCallback,
                         rocksdb_rs::status::Code::kIOError, false),
         rocksdb_rs::status::Severity::kNoError},
        {std::make_tuple(BackgroundErrorReason::kManifestWrite,
                         rocksdb_rs::status::Code::kIOError, true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kManifestWrite,
                         rocksdb_rs::status::Code::kIOError, false),
         rocksdb_rs::status::Severity::kFatalError},
        // Errors during BG flush with WAL disabled
        {std::make_tuple(BackgroundErrorReason::kFlushNoWAL,
                         rocksdb_rs::status::Code::kCorruption, true),
         rocksdb_rs::status::Severity::kUnrecoverableError},
        {std::make_tuple(BackgroundErrorReason::kFlushNoWAL,
                         rocksdb_rs::status::Code::kCorruption, false),
         rocksdb_rs::status::Severity::kNoError},
        {std::make_tuple(BackgroundErrorReason::kFlushNoWAL,
                         rocksdb_rs::status::Code::kIOError, true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kFlushNoWAL,
                         rocksdb_rs::status::Code::kIOError, false),
         rocksdb_rs::status::Severity::kNoError},
        {std::make_tuple(BackgroundErrorReason::kManifestWriteNoWAL,
                         rocksdb_rs::status::Code::kIOError, true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kManifestWriteNoWAL,
                         rocksdb_rs::status::Code::kIOError, false),
         rocksdb_rs::status::Severity::kFatalError},
};

std::map<std::tuple<BackgroundErrorReason, bool>, rocksdb_rs::status::Severity>
    DefaultReasonMap = {
        // Errors during BG compaction
        {std::make_tuple(BackgroundErrorReason::kCompaction, true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kCompaction, false),
         rocksdb_rs::status::Severity::kNoError},
        // Errors during BG flush
        {std::make_tuple(BackgroundErrorReason::kFlush, true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kFlush, false),
         rocksdb_rs::status::Severity::kNoError},
        // Errors during Write
        {std::make_tuple(BackgroundErrorReason::kWriteCallback, true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kWriteCallback, false),
         rocksdb_rs::status::Severity::kFatalError},
        // Errors during Memtable update
        {std::make_tuple(BackgroundErrorReason::kMemTable, true),
         rocksdb_rs::status::Severity::kFatalError},
        {std::make_tuple(BackgroundErrorReason::kMemTable, false),
         rocksdb_rs::status::Severity::kFatalError},
};

void ErrorHandler::CancelErrorRecovery() {
  db_mutex_->AssertHeld();

  // We'll release the lock before calling sfm, so make sure no new
  // recovery gets scheduled at that point
  auto_recovery_ = false;
  SstFileManagerImpl* sfm =
      reinterpret_cast<SstFileManagerImpl*>(db_options_.sst_file_manager.get());
  if (sfm) {
    // This may or may not cancel a pending recovery
    db_mutex_->Unlock();
    bool cancelled = sfm->CancelErrorRecovery(this);
    db_mutex_->Lock();
    if (cancelled) {
      recovery_in_prog_ = false;
    }
  }

  // If auto recovery is also runing to resume from the retryable error,
  // we should wait and end the auto recovery.
  EndAutoRecovery();
}

STATIC_AVOID_DESTRUCTION(const Status, kOkStatus){Status_OK()};

// This is the main function for looking at an error during a background
// operation and deciding the severity, and error recovery strategy. The high
// level algorithm is as follows -
// 1. Classify the severity of the error based on the ErrorSeverityMap,
//    DefaultErrorSeverityMap and DefaultReasonMap defined earlier
// 2. Call a Status code specific override function to adjust the severity
//    if needed. The reason for this is our ability to recover may depend on
//    the exact options enabled in DBOptions
// 3. Determine if auto recovery is possible. A listener notification callback
//    is called, which can disable the auto recovery even if we decide its
//    feasible
// 4. For Status_NoSpace() errors, rely on SstFileManagerImpl to control
//    the actual recovery. If no sst file manager is specified in DBOptions,
//    a default one is allocated during DB::Open(), so there will always be
//    one.
// This can also get called as part of a recovery operation. In that case, we
// also track the error separately in recovery_error_ so we can tell in the
// end whether recovery succeeded or not
const Status& ErrorHandler::HandleKnownErrors(const Status& bg_err,
                                              BackgroundErrorReason reason) {
  db_mutex_->AssertHeld();
  if (bg_err.ok()) {
    return kOkStatus;
  }

  if (bg_error_stats_ != nullptr) {
    RecordTick(bg_error_stats_.get(), ERROR_HANDLER_BG_ERROR_COUNT);
    RecordTick(bg_error_stats_.get(), ERROR_HANDLER_BG_ERROR_COUNT_MISSPELLED);
  }
  ROCKS_LOG_INFO(db_options_.info_log,
                 "ErrorHandler: Set regular background error\n");

  bool paranoid = db_options_.paranoid_checks;
  rocksdb_rs::status::Severity sev = rocksdb_rs::status::Severity::kFatalError;
  Status new_bg_err = Status_new();
  DBRecoverContext context;
  bool found = false;

  {
    auto entry = ErrorSeverityMap.find(
        std::make_tuple(reason, bg_err.code(), bg_err.subcode(), paranoid));
    if (entry != ErrorSeverityMap.end()) {
      sev = entry->second;
      found = true;
    }
  }

  if (!found) {
    auto entry = DefaultErrorSeverityMap.find(
        std::make_tuple(reason, bg_err.code(), paranoid));
    if (entry != DefaultErrorSeverityMap.end()) {
      sev = entry->second;
      found = true;
    }
  }

  if (!found) {
    auto entry = DefaultReasonMap.find(std::make_tuple(reason, paranoid));
    if (entry != DefaultReasonMap.end()) {
      sev = entry->second;
    }
  }

  new_bg_err = Status_new(bg_err, sev);

  // Check if recovery is currently in progress. If it is, we will save this
  // error so we can check it at the end to see if recovery succeeded or not
  if (recovery_in_prog_ && recovery_error_.ok()) {
    recovery_error_.copy_from(new_bg_err);
  }

  bool auto_recovery = auto_recovery_;
  if (new_bg_err.severity() >= rocksdb_rs::status::Severity::kFatalError && auto_recovery) {
    auto_recovery = false;
  }

  // Allow some error specific overrides
  if (new_bg_err.subcode() == rocksdb_rs::status::SubCode::kNoSpace ||
      new_bg_err.subcode() == rocksdb_rs::status::SubCode::kSpaceLimit) {
    new_bg_err = OverrideNoSpaceError(new_bg_err, &auto_recovery);
  }

  if (!new_bg_err.ok()) {
    Status s = new_bg_err.Clone();
    EventHelpers::NotifyOnBackgroundError(db_options_.listeners, reason, &s,
                                          db_mutex_, &auto_recovery);
    if (!s.ok() && (s.severity() > bg_error_.severity())) {
      bg_error_.copy_from(s);
    } else {
      // This error is less severe than previously encountered error. Don't
      // take any further action
      return bg_error_;
    }
  }

  recover_context_ = context;
  if (auto_recovery) {
    recovery_in_prog_ = true;

    // Kick-off error specific recovery
    if (new_bg_err.subcode() == rocksdb_rs::status::SubCode::kNoSpace ||
        new_bg_err.subcode() == rocksdb_rs::status::SubCode::kSpaceLimit) {
      RecoverFromNoSpace();
    }
  }
  if (bg_error_.severity() >= rocksdb_rs::status::Severity::kHardError) {
    is_db_stopped_.store(true, std::memory_order_release);
  }
  return bg_error_;
}

// This is the main function for looking at IO related error during the
// background operations. The main logic is:
// 1) File scope IO error is treated as retryable IO error in the write
//    path. In RocksDB, If a file has write IO error and it is at file scope,
//    RocksDB never write to the same file again. RocksDB will create a new
//    file and rewrite the whole content. Thus, it is retryable.
// 1) if the error is caused by data loss, the error is mapped to
//    unrecoverable error. Application/user must take action to handle
//    this situation (File scope case is excluded).
// 2) if the error is a Retryable IO error (i.e., it is a file scope IO error,
//     or its retryable flag is set and not a data loss error), auto resume
//     will be called and the auto resume can be controlled by resume count
//     and resume interval options. There are three sub-cases:
//    a) if the error happens during compaction, it is mapped to a soft error.
//       the compaction thread will reschedule a new compaction.
//    b) if the error happens during flush and also WAL is empty, it is mapped
//       to a soft error. Note that, it includes the case that IO error happens
//       in SST or manifest write during flush.
//    c) all other errors are mapped to hard error.
// 3) for other cases, SetBGError(const Status& bg_err, BackgroundErrorReason
//    reason) will be called to handle other error cases.
const Status& ErrorHandler::SetBGError(const Status& bg_status,
                                       BackgroundErrorReason reason) {
  db_mutex_->AssertHeld();
  Status tmp_status = bg_status.Clone();
  IOStatus bg_io_err = status_to_io_status(std::move(tmp_status));

  if (bg_io_err.ok()) {
    return kOkStatus;
  }
  ROCKS_LOG_WARN(db_options_.info_log, "Background IO error %s",
                 bg_io_err.ToString().c_str());

  if (recovery_in_prog_ && recovery_io_error_.ok()) {
    recovery_io_error_ = bg_io_err;
  }
  if (BackgroundErrorReason::kManifestWrite == reason ||
      BackgroundErrorReason::kManifestWriteNoWAL == reason) {
    // Always returns ok
    ROCKS_LOG_INFO(db_options_.info_log, "Disabling File Deletions");
    db_->DisableFileDeletionsWithLock();
  }

  Status new_bg_io_err = bg_io_err;
  DBRecoverContext context;
  if (bg_io_err.GetScope() != IOStatus::IOErrorScope::kIOErrorScopeFile &&
      bg_io_err.GetDataLoss()) {
    // First, data loss (non file scope) is treated as unrecoverable error. So
    // it can directly overwrite any existing bg_error_.
    bool auto_recovery = false;
    Status bg_err = Status_new(new_bg_io_err, rocksdb_rs::status::Severity::kUnrecoverableError);
    CheckAndSetRecoveryAndBGError(bg_err);
    if (bg_error_stats_ != nullptr) {
      RecordTick(bg_error_stats_.get(), ERROR_HANDLER_BG_ERROR_COUNT);
      RecordTick(bg_error_stats_.get(),
                 ERROR_HANDLER_BG_ERROR_COUNT_MISSPELLED);
      RecordTick(bg_error_stats_.get(), ERROR_HANDLER_BG_IO_ERROR_COUNT);
      RecordTick(bg_error_stats_.get(),
                 ERROR_HANDLER_BG_IO_ERROR_COUNT_MISSPELLED);
    }
    ROCKS_LOG_INFO(
        db_options_.info_log,
        "ErrorHandler: Set background IO error as unrecoverable error\n");
    EventHelpers::NotifyOnBackgroundError(db_options_.listeners, reason,
                                          &bg_err, db_mutex_, &auto_recovery);
    recover_context_ = context;
    return bg_error_;
  } else if (bg_io_err.subcode() != rocksdb_rs::status::SubCode::kNoSpace &&
             (bg_io_err.GetScope() ==
                  IOStatus::IOErrorScope::kIOErrorScopeFile ||
              bg_io_err.GetRetryable())) {
    // Second, check if the error is a retryable IO error (file scope IO error
    // is also treated as retryable IO error in RocksDB write path). if it is
    // retryable error and its severity is higher than bg_error_, overwrite the
    // bg_error_ with new error. In current stage, for retryable IO error of
    // compaction, treat it as soft error. In other cases, treat the retryable
    // IO error as hard error. Note that, all the NoSpace error should be
    // handled by the SstFileManager::StartErrorRecovery(). Therefore, no matter
    // it is retryable or file scope, this logic will be bypassed.
    bool auto_recovery = false;
    EventHelpers::NotifyOnBackgroundError(db_options_.listeners, reason,
                                          &new_bg_io_err, db_mutex_,
                                          &auto_recovery);
    if (bg_error_stats_ != nullptr) {
      RecordTick(bg_error_stats_.get(), ERROR_HANDLER_BG_ERROR_COUNT);
      RecordTick(bg_error_stats_.get(),
                 ERROR_HANDLER_BG_ERROR_COUNT_MISSPELLED);
      RecordTick(bg_error_stats_.get(), ERROR_HANDLER_BG_IO_ERROR_COUNT);
      RecordTick(bg_error_stats_.get(),
                 ERROR_HANDLER_BG_IO_ERROR_COUNT_MISSPELLED);
      RecordTick(bg_error_stats_.get(),
                 ERROR_HANDLER_BG_RETRYABLE_IO_ERROR_COUNT);
      RecordTick(bg_error_stats_.get(),
                 ERROR_HANDLER_BG_RETRYABLE_IO_ERROR_COUNT_MISSPELLED);
    }
    ROCKS_LOG_INFO(db_options_.info_log,
                   "ErrorHandler: Set background retryable IO error\n");
    if (BackgroundErrorReason::kCompaction == reason) {
      // We map the retryable IO error during compaction to soft error. Since
      // compaction can reschedule by itself. We will not set the BG error in
      // this case
      // TODO:  a better way to set or clean the retryable IO error which
      // happens during compaction SST file write.
      if (bg_error_stats_ != nullptr) {
        RecordTick(bg_error_stats_.get(), ERROR_HANDLER_AUTORESUME_COUNT);
      }
      ROCKS_LOG_INFO(
          db_options_.info_log,
          "ErrorHandler: Compaction will schedule by itself to resume\n");
      return bg_error_;
    } else if (BackgroundErrorReason::kFlushNoWAL == reason ||
               BackgroundErrorReason::kManifestWriteNoWAL == reason) {
      // When the BG Retryable IO error reason is flush without WAL,
      // We map it to a soft error. At the same time, all the background work
      // should be stopped except the BG work from recovery. Therefore, we
      // set the soft_error_no_bg_work_ to true. At the same time, since DB
      // continues to receive writes when BG error is soft error, to avoid
      // to many small memtable being generated during auto resume, the flush
      // reason is set to kErrorRecoveryRetryFlush.
      Status bg_err = Status_new(new_bg_io_err, rocksdb_rs::status::Severity::kSoftError);
      CheckAndSetRecoveryAndBGError(bg_err);
      soft_error_no_bg_work_ = true;
      context.flush_reason = FlushReason::kErrorRecoveryRetryFlush;
      recover_context_ = context;
      return StartRecoverFromRetryableBGIOError(bg_io_err);
    } else {
      Status bg_err = Status_new(new_bg_io_err, rocksdb_rs::status::Severity::kHardError);
      CheckAndSetRecoveryAndBGError(bg_err);
      recover_context_ = context;
      return StartRecoverFromRetryableBGIOError(bg_io_err);
    }
  } else {
    if (bg_error_stats_ != nullptr) {
      RecordTick(bg_error_stats_.get(), ERROR_HANDLER_BG_IO_ERROR_COUNT);
      RecordTick(bg_error_stats_.get(),
                 ERROR_HANDLER_BG_IO_ERROR_COUNT_MISSPELLED);
    }
    return HandleKnownErrors(new_bg_io_err, reason);
  }
}

Status ErrorHandler::OverrideNoSpaceError(const Status& bg_error,
                                          bool* auto_recovery) {
  if (bg_error.severity() >= rocksdb_rs::status::Severity::kFatalError) {
    return bg_error.Clone();
  }

  if (db_options_.sst_file_manager.get() == nullptr) {
    // We rely on SFM to poll for enough disk space and recover
    *auto_recovery = false;
    return bg_error.Clone();
  }

  if (db_options_.allow_2pc &&
      (bg_error.severity() <= rocksdb_rs::status::Severity::kSoftError)) {
    // Don't know how to recover, as the contents of the current WAL file may
    // be inconsistent, and it may be needed for 2PC. If 2PC is not enabled,
    // we can just flush the memtable and discard the log
    *auto_recovery = false;
    return Status_new(bg_error, rocksdb_rs::status::Severity::kFatalError);
  }

  {
    uint64_t free_space;
    if (db_options_.env->GetFreeSpace(db_options_.db_paths[0].path,
                                      &free_space).eq(Status_NotSupported())) {
      *auto_recovery = false;
    }
  }

  return bg_error.Clone();
}

void ErrorHandler::RecoverFromNoSpace() {
  SstFileManagerImpl* sfm =
      reinterpret_cast<SstFileManagerImpl*>(db_options_.sst_file_manager.get());

  // Inform SFM of the error, so it can kick-off the recovery
  if (sfm) {
    sfm->StartErrorRecovery(this, bg_error_.Clone());
  }
}

Status ErrorHandler::ClearBGError() {
  db_mutex_->AssertHeld();

  // Signal that recovery succeeded
  if (recovery_error_.ok()) {
    Status old_bg_error = bg_error_.Clone();
    // Clear and check the recovery IO and BG error
    bg_error_ = Status_OK();
    recovery_io_error_ = IOStatus::OK();
    recovery_in_prog_ = false;
    soft_error_no_bg_work_ = false;
    EventHelpers::NotifyOnErrorRecoveryEnd(db_options_.listeners, old_bg_error,
                                           bg_error_, db_mutex_);
  }
  return recovery_error_.Clone();
}

Status ErrorHandler::RecoverFromBGError(bool is_manual) {
  InstrumentedMutexLock l(db_mutex_);
  bool no_bg_work_original_flag = soft_error_no_bg_work_;
  if (is_manual) {
    // If its a manual recovery and there's a background recovery in progress
    // return busy status
    if (recovery_in_prog_) {
      return Status_Busy();
    }
    recovery_in_prog_ = true;

    // In manual resume, we allow the bg work to run. If it is a auto resume,
    // the bg work should follow this tag.
    soft_error_no_bg_work_ = false;

    // In manual resume, if the bg error is a soft error and also requires
    // no bg work, the error must be recovered by call the flush with
    // flush reason: kErrorRecoveryRetryFlush. In other case, the flush
    // reason is set to kErrorRecovery.
    if (no_bg_work_original_flag) {
      recover_context_.flush_reason = FlushReason::kErrorRecoveryRetryFlush;
    } else {
      recover_context_.flush_reason = FlushReason::kErrorRecovery;
    }
  }

  if (bg_error_.severity() == rocksdb_rs::status::Severity::kSoftError &&
      recover_context_.flush_reason == FlushReason::kErrorRecovery) {
    // Simply clear the background error and return
    recovery_error_ = Status_OK();
    return ClearBGError();
  }

  // Reset recovery_error_. We will use this to record any errors that happen
  // during the recovery process. While recovering, the only operations that
  // can generate background errors should be the flush operations
  recovery_error_ = Status_OK();
  Status s = db_->ResumeImpl(recover_context_);
  if (s.ok()) {
    soft_error_no_bg_work_ = false;
  } else {
    soft_error_no_bg_work_ = no_bg_work_original_flag;
  }

  // For manual recover, shutdown, and fatal error  cases, set
  // recovery_in_prog_ to false. For automatic background recovery, leave it
  // as is regardless of success or failure as it will be retried
  if (is_manual || s.IsShutdownInProgress() ||
      bg_error_.severity() >= rocksdb_rs::status::Severity::kFatalError) {
    recovery_in_prog_ = false;
  }
  return s;
}

const Status& ErrorHandler::StartRecoverFromRetryableBGIOError(
    const IOStatus& io_error) {
  db_mutex_->AssertHeld();
  if (bg_error_.ok()) {
    return bg_error_;
  } else if (io_error.ok()) {
    return kOkStatus;
  } else if (db_options_.max_bgerror_resume_count <= 0 || recovery_in_prog_) {
    // Auto resume BG error is not enabled, directly return bg_error_.
    return bg_error_;
  }
  if (bg_error_stats_ != nullptr) {
    RecordTick(bg_error_stats_.get(), ERROR_HANDLER_AUTORESUME_COUNT);
  }
  ROCKS_LOG_INFO(
      db_options_.info_log,
      "ErrorHandler: Call StartRecoverFromRetryableBGIOError to resume\n");
  if (recovery_thread_) {
    // In this case, if recovery_in_prog_ is false, current thread should
    // wait the previous recover thread to finish and create a new thread
    // to recover from the bg error.
    db_mutex_->Unlock();
    recovery_thread_->join();
    db_mutex_->Lock();
  }

  recovery_in_prog_ = true;
  recovery_thread_.reset(
      new port::Thread(&ErrorHandler::RecoverFromRetryableBGIOError, this));

  if (recovery_io_error_.ok() && recovery_error_.ok()) {
    return recovery_error_;
  } else {
    return bg_error_;
  }
}

// Automatic recover from Retryable BG IO error. Must be called after db
// mutex is released.
void ErrorHandler::RecoverFromRetryableBGIOError() {
  TEST_SYNC_POINT("RecoverFromRetryableBGIOError:BeforeStart");
  InstrumentedMutexLock l(db_mutex_);
  if (end_recovery_) {
    EventHelpers::NotifyOnErrorRecoveryEnd(db_options_.listeners, bg_error_,
                                           Status_ShutdownInProgress(),
                                           db_mutex_);
    return;
  }
  DBRecoverContext context = recover_context_;
  int resume_count = db_options_.max_bgerror_resume_count;
  uint64_t wait_interval = db_options_.bgerror_resume_retry_interval;
  uint64_t retry_count = 0;
  // Recover from the retryable error. Create a separate thread to do it.
  while (resume_count > 0) {
    if (end_recovery_) {
      EventHelpers::NotifyOnErrorRecoveryEnd(db_options_.listeners, bg_error_,
                                             Status_ShutdownInProgress(),
                                             db_mutex_);
      return;
    }
    TEST_SYNC_POINT("RecoverFromRetryableBGIOError:BeforeResume0");
    TEST_SYNC_POINT("RecoverFromRetryableBGIOError:BeforeResume1");
    recovery_io_error_ = IOStatus::OK();
    recovery_error_ = Status_OK();
    retry_count++;
    Status s = db_->ResumeImpl(context);
    if (bg_error_stats_ != nullptr) {
      RecordTick(bg_error_stats_.get(),
                 ERROR_HANDLER_AUTORESUME_RETRY_TOTAL_COUNT);
    }
    if (s.IsShutdownInProgress() ||
        bg_error_.severity() >= rocksdb_rs::status::Severity::kFatalError) {
      // If DB shutdown in progress or the error severity is higher than
      // Hard Error, stop auto resume and returns.
      recovery_in_prog_ = false;
      if (bg_error_stats_ != nullptr) {
        RecordInHistogram(bg_error_stats_.get(),
                          ERROR_HANDLER_AUTORESUME_RETRY_COUNT, retry_count);
      }
      EventHelpers::NotifyOnErrorRecoveryEnd(db_options_.listeners, bg_error_,
                                             bg_error_, db_mutex_);
      return;
    }
    if (!recovery_io_error_.ok() &&
        recovery_error_.severity() <= rocksdb_rs::status::Severity::kHardError &&
        recovery_io_error_.GetRetryable()) {
      // If new BG IO error happens during auto recovery and it is retryable
      // and its severity is Hard Error or lower, the auto resmue sleep for
      // a period of time and redo auto resume if it is allowed.
      TEST_SYNC_POINT("RecoverFromRetryableBGIOError:BeforeWait0");
      TEST_SYNC_POINT("RecoverFromRetryableBGIOError:BeforeWait1");
      int64_t wait_until = db_options_.clock->NowMicros() + wait_interval;
      cv_.TimedWait(wait_until);
    } else {
      // There are three possibility: 1) recover_io_error is set during resume
      // and the error is not retryable, 2) recover is successful, 3) other
      // error happens during resume and cannot be resumed here.
      if (recovery_io_error_.ok() && recovery_error_.ok() && s.ok()) {
        // recover from the retryable IO error and no other BG errors. Clean
        // the bg_error and notify user.
        TEST_SYNC_POINT("RecoverFromRetryableBGIOError:RecoverSuccess");
        Status old_bg_error = bg_error_.Clone();
        is_db_stopped_.store(false, std::memory_order_release);
        bg_error_ = Status_OK();
        EventHelpers::NotifyOnErrorRecoveryEnd(
            db_options_.listeners, old_bg_error, bg_error_, db_mutex_);
        if (bg_error_stats_ != nullptr) {
          RecordTick(bg_error_stats_.get(),
                     ERROR_HANDLER_AUTORESUME_SUCCESS_COUNT);
          RecordInHistogram(bg_error_stats_.get(),
                            ERROR_HANDLER_AUTORESUME_RETRY_COUNT, retry_count);
        }
        recovery_in_prog_ = false;
        if (soft_error_no_bg_work_) {
          soft_error_no_bg_work_ = false;
        }
        return;
      } else {
        // In this case: 1) recovery_io_error is more serious or not retryable
        // 2) other Non IO recovery_error happens. The auto recovery stops.
        recovery_in_prog_ = false;
        if (bg_error_stats_ != nullptr) {
          RecordInHistogram(bg_error_stats_.get(),
                            ERROR_HANDLER_AUTORESUME_RETRY_COUNT, retry_count);
        }
        EventHelpers::NotifyOnErrorRecoveryEnd(
            db_options_.listeners, bg_error_,
            !recovery_io_error_.ok()
                ? recovery_io_error_
                : (!recovery_error_.ok() ? recovery_error_ : s).Clone(),
            db_mutex_);
        return;
      }
    }
    resume_count--;
  }
  recovery_in_prog_ = false;
  EventHelpers::NotifyOnErrorRecoveryEnd(
      db_options_.listeners, bg_error_,
      Status_Aborted("Exceeded resume retry count"), db_mutex_);
  TEST_SYNC_POINT("RecoverFromRetryableBGIOError:LoopOut");
  if (bg_error_stats_ != nullptr) {
    RecordInHistogram(bg_error_stats_.get(),
                      ERROR_HANDLER_AUTORESUME_RETRY_COUNT, retry_count);
  }
  return;
}

void ErrorHandler::CheckAndSetRecoveryAndBGError(const Status& bg_err) {
  if (recovery_in_prog_ && recovery_error_.ok()) {
    recovery_error_.copy_from(bg_err);
  }
  if (bg_err.severity() > bg_error_.severity()) {
    bg_error_.copy_from(bg_err);
  }
  if (bg_error_.severity() >= rocksdb_rs::status::Severity::kHardError) {
    is_db_stopped_.store(true, std::memory_order_release);
  }
  return;
}

void ErrorHandler::EndAutoRecovery() {
  db_mutex_->AssertHeld();
  if (!end_recovery_) {
    end_recovery_ = true;
  }
  cv_.SignalAll();
  db_mutex_->Unlock();
  if (recovery_thread_) {
    recovery_thread_->join();
  }
  db_mutex_->Lock();
  return;
}

}  // namespace rocksdb
