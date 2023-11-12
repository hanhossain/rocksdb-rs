// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// A Status encapsulates the result of an operation.  It may indicate success,
// or it may indicate an error with an associated error message.
//
// Multiple threads can invoke const methods on a Status without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Status must use
// external synchronization.

#pragma once

#ifdef ROCKSDB_ASSERT_STATUS_CHECKED
#include <stdio.h>
#include <stdlib.h>
#endif

#include <memory>
#include <string>

#ifdef ROCKSDB_ASSERT_STATUS_CHECKED
#include "port/stack_trace.h"
#endif

#include "rocksdb/slice.h"

namespace ROCKSDB_NAMESPACE {

class Status {
 public:
  // Create a success status.
  Status()
      : code_(Code::kOk),
        subcode_(SubCode::kNone),
        sev_(Severity::kNoError),
        retryable_(false),
        data_loss_(false),
        scope_(0),
        state_(nullptr) {}
  ~Status() {
#ifdef ROCKSDB_ASSERT_STATUS_CHECKED
    if (!checked_) {
      fprintf(stderr, "Failed to check Status %p\n", this);
      port::PrintStack();
      std::abort();
    }
#endif  // ROCKSDB_ASSERT_STATUS_CHECKED
  }

  // Copy the specified status.
  Status(const Status& s);
  Status& operator=(const Status& s);
  Status(Status&& s) noexcept;
  Status& operator=(Status&& s) noexcept;
  bool operator==(const Status& rhs) const;
  bool operator!=(const Status& rhs) const;

  // In case of intentionally swallowing an error, user must explicitly call
  // this function. That way we are easily able to search the code to find where
  // error swallowing occurs.
  inline void PermitUncheckedError() const { MarkChecked(); }

  inline void MustCheck() const {
#ifdef ROCKSDB_ASSERT_STATUS_CHECKED
    checked_ = false;
#endif  // ROCKSDB_ASSERT_STATUS_CHECKED
  }

  enum class Code : unsigned char {
    kOk = 0,
    kNotFound = 1,
    kCorruption = 2,
    kNotSupported = 3,
    kInvalidArgument = 4,
    kIOError = 5,
    kMergeInProgress = 6,
    kIncomplete = 7,
    kShutdownInProgress = 8,
    kTimedOut = 9,
    kAborted = 10,
    kBusy = 11,
    kExpired = 12,
    kTryAgain = 13,
    kCompactionTooLarge = 14,
    kColumnFamilyDropped = 15,
    kMaxCode
  };

  Code code() const;

  enum class SubCode : unsigned char {
    kNone = 0,
    kMutexTimeout = 1,
    kLockTimeout = 2,
    kLockLimit = 3,
    kNoSpace = 4,
    kDeadlock = 5,
    kStaleFile = 6,
    kMemoryLimit = 7,
    kSpaceLimit = 8,
    kPathNotFound = 9,
    KMergeOperandsInsufficientCapacity = 10,
    kManualCompactionPaused = 11,
    kOverwritten = 12,
    kTxnNotPrepared = 13,
    kIOFenced = 14,
    kMergeOperatorFailed = 15,
    kMaxSubCode
  };

  SubCode subcode() const;

  enum class Severity : unsigned char {
    kNoError = 0,
    kSoftError = 1,
    kHardError = 2,
    kFatalError = 3,
    kUnrecoverableError = 4,
    kMaxSeverity
  };

  Status(const Status& s, Severity sev);

  Status(Code _code, SubCode _subcode, Severity _sev, const Slice& msg)
      : Status(_code, _subcode, msg, "", _sev) {}

  static Status CopyAppendMessage(const Status& s, const Slice& delim,
                                  const Slice& msg);

  Severity severity() const;

  // Returns a C style string indicating the message of the Status
  const char* getState() const;

  // Return a success status.
  static Status OK();

  // Successful, though an existing something was overwritten
  // Note: using variants of OK status for program logic is discouraged,
  // but it can be useful for communicating statistical information without
  // changing public APIs.
  static Status OkOverwritten();

  // Return error status of an appropriate type.
  static Status NotFound(const Slice& msg, const Slice& msg2 = Slice());

  // Fast path for not found without malloc;
  static Status NotFound(SubCode msg = SubCode::kNone);

  static Status NotFound(SubCode sc, const Slice& msg, const Slice& msg2 = Slice());

  static Status Corruption(const Slice& msg, const Slice& msg2 = Slice());
  static Status Corruption(SubCode msg = SubCode::kNone);

  static Status NotSupported(const Slice& msg, const Slice& msg2 = Slice());
  static Status NotSupported(SubCode msg = SubCode::kNone);

  static Status InvalidArgument(const Slice& msg, const Slice& msg2 = Slice());
  static Status InvalidArgument(SubCode msg = SubCode::kNone);

  static Status IOError(const Slice& msg, const Slice& msg2 = Slice());
  static Status IOError(SubCode msg = SubCode::kNone);

  static Status MergeInProgress(const Slice& msg, const Slice& msg2 = Slice());
  static Status MergeInProgress(SubCode msg = SubCode::kNone);

  static Status Incomplete(const Slice& msg, const Slice& msg2 = Slice());
  static Status Incomplete(SubCode msg = SubCode::kNone);

  static Status ShutdownInProgress(SubCode msg = SubCode::kNone);
  static Status ShutdownInProgress(const Slice& msg, const Slice& msg2 = Slice());
  static Status Aborted(SubCode msg = SubCode::kNone);
  static Status Aborted(const Slice& msg, const Slice& msg2 = Slice());

  static Status Busy(SubCode msg = SubCode::kNone);
  static Status Busy(const Slice& msg, const Slice& msg2 = Slice());

  static Status TimedOut(SubCode msg = SubCode::kNone);
  static Status TimedOut(const Slice& msg, const Slice& msg2 = Slice());

  static Status Expired(SubCode msg = SubCode::kNone);
  static Status Expired(const Slice& msg, const Slice& msg2 = Slice());

  static Status TryAgain(SubCode msg = SubCode::kNone);
  static Status TryAgain(const Slice& msg, const Slice& msg2 = Slice());

  static Status CompactionTooLarge(SubCode msg = SubCode::kNone);
  static Status CompactionTooLarge(const Slice& msg,
                                   const Slice& msg2 = Slice());

  static Status ColumnFamilyDropped(SubCode msg = SubCode::kNone);

  static Status ColumnFamilyDropped(const Slice& msg,
                                    const Slice& msg2 = Slice());

  static Status NoSpace();
  static Status NoSpace(const Slice& msg, const Slice& msg2 = Slice());

  static Status MemoryLimit();
  static Status MemoryLimit(const Slice& msg, const Slice& msg2 = Slice());

  static Status SpaceLimit();
  static Status SpaceLimit(const Slice& msg, const Slice& msg2 = Slice());

  static Status PathNotFound();
  static Status PathNotFound(const Slice& msg, const Slice& msg2 = Slice());

  static Status TxnNotPrepared();
  static Status TxnNotPrepared(const Slice& msg, const Slice& msg2 = Slice());

  // Returns true iff the status indicates success.
  bool ok() const;

  // Returns true iff the status indicates success *with* something
  // overwritten
  bool IsOkOverwritten() const;

  // Returns true iff the status indicates a NotFound error.
  bool IsNotFound() const;

  // Returns true iff the status indicates a Corruption error.
  bool IsCorruption() const;

  // Returns true iff the status indicates a NotSupported error.
  bool IsNotSupported() const;

  // Returns true iff the status indicates an InvalidArgument error.
  bool IsInvalidArgument() const;

  // Returns true iff the status indicates an IOError.
  bool IsIOError() const;

  // Returns true iff the status indicates an MergeInProgress.
  bool IsMergeInProgress() const;

  // Returns true iff the status indicates Incomplete
  bool IsIncomplete() const;

  // Returns true iff the status indicates Shutdown In progress
  bool IsShutdownInProgress() const;

  bool IsTimedOut() const;

  bool IsAborted() const;

  bool IsLockLimit() const;

  // Returns true iff the status indicates that a resource is Busy and
  // temporarily could not be acquired.
  bool IsBusy() const;

  bool IsDeadlock() const;

  // Returns true iff the status indicated that the operation has Expired.
  bool IsExpired() const;

  // Returns true iff the status indicates a TryAgain error.
  // This usually means that the operation failed, but may succeed if
  // re-attempted.
  bool IsTryAgain() const;

  // Returns true iff the status indicates the proposed compaction is too large
  bool IsCompactionTooLarge() const;

  // Returns true iff the status indicates Column Family Dropped
  bool IsColumnFamilyDropped() const;

  // Returns true iff the status indicates a NoSpace error
  // This is caused by an I/O error returning the specific "out of space"
  // error condition. Stricto sensu, an NoSpace error is an I/O error
  // with a specific subcode, enabling users to take the appropriate action
  // if needed
  bool IsNoSpace() const;

  // Returns true iff the status indicates a memory limit error.  There may be
  // cases where we limit the memory used in certain operations (eg. the size
  // of a write batch) in order to avoid out of memory exceptions.
  bool IsMemoryLimit() const;

  // Returns true iff the status indicates a PathNotFound error
  // This is caused by an I/O error returning the specific "no such file or
  // directory" error condition. A PathNotFound error is an I/O error with
  // a specific subcode, enabling users to take appropriate action if necessary
  bool IsPathNotFound() const;

  // Returns true iff the status indicates manual compaction paused. This
  // is caused by a call to PauseManualCompaction
  bool IsManualCompactionPaused() const;

  // Returns true iff the status indicates a TxnNotPrepared error.
  bool IsTxnNotPrepared() const;

  // Returns true iff the status indicates a IOFenced error.
  bool IsIOFenced() const;

  // Return a string representation of this status suitable for printing.
  // Returns the string "OK" for success.
  std::string ToString() const;

 protected:
  Code code_;
  SubCode subcode_;
  Severity sev_;
  bool retryable_;
  bool data_loss_;
  unsigned char scope_;
  // A nullptr state_ (which is at least the case for OK) means the extra
  // message is empty.
  std::unique_ptr<const char[]> state_;
#ifdef ROCKSDB_ASSERT_STATUS_CHECKED
  mutable bool checked_ = false;
#endif  // ROCKSDB_ASSERT_STATUS_CHECKED

  explicit Status(Code _code, SubCode _subcode = SubCode::kNone);
  explicit Status(Code _code, SubCode _subcode, bool retryable, bool data_loss,
                  unsigned char scope);

  Status(Code _code, SubCode _subcode, const Slice& msg, const Slice& msg2,
         Severity sev = Severity::kNoError);
  Status(Code _code, const Slice& msg, const Slice& msg2);

  static std::unique_ptr<const char[]> CopyState(const char* s);
  void MarkChecked() const;
};

}  // namespace ROCKSDB_NAMESPACE
