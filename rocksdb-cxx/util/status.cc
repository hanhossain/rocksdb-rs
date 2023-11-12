//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "rocksdb/status.h"

#include <stdio.h>
#ifdef OS_WIN
#include <string.h>
#endif
#include <cstring>

#include "port/port.h"

namespace ROCKSDB_NAMESPACE {
// Create a success status.
Status::Status()
    : code_(Code::kOk),
      subcode_(SubCode::kNone),
      sev_(Severity::kNoError),
      retryable_(false),
      data_loss_(false),
      scope_(0),
      state_(nullptr) {}

Status::~Status() {
#ifdef ROCKSDB_ASSERT_STATUS_CHECKED
    if (!checked_) {
        fprintf(stderr, "Failed to check Status %p\n", this);
        port::PrintStack();
        std::abort();
    }
#endif  // ROCKSDB_ASSERT_STATUS_CHECKED
}

Status::Status(Code _code, SubCode _subcode, Severity _sev, const Slice& msg)
    : Status(_code, _subcode, msg, "", _sev) {}

Status::Code Status::code() const {
    MarkChecked();
    return code_;
}

Status::SubCode Status::subcode() const {
    MarkChecked();
    return subcode_;
}

Status::Severity Status::severity() const {
    MarkChecked();
    return sev_;
}

const char* Status::getState() const {
    MarkChecked();
    return state_.get();
}

Status Status::OK() { return Status(); }

Status Status::OkOverwritten() { return Status(Code::kOk, SubCode::kOverwritten); }

Status Status::NotFound(const Slice& msg, const Slice& msg2) {
    return Status(Code::kNotFound, msg, msg2);
}

Status Status::NotFound(SubCode msg) { return Status(Code::kNotFound, msg); }

Status Status::NotFound(SubCode sc, const Slice& msg,
                       const Slice& msg2) {
    return Status(Code::kNotFound, sc, msg, msg2);
}

Status Status::Corruption(const Slice& msg, const Slice& msg2) {
    return Status(Code::kCorruption, msg, msg2);
}
Status Status::Corruption(SubCode msg) {
    return Status(Code::kCorruption, msg);
}

Status Status::NotSupported(const Slice& msg, const Slice& msg2) {
    return Status(Code::kNotSupported, msg, msg2);
}
Status Status::NotSupported(SubCode msg) {
    return Status(Code::kNotSupported, msg);
}

Status Status::InvalidArgument(const Slice& msg, const Slice& msg2) {
    return Status(Code::kInvalidArgument, msg, msg2);
}
Status Status::InvalidArgument(SubCode msg) {
    return Status(Code::kInvalidArgument, msg);
}

Status Status::IOError(const Slice& msg, const Slice& msg2) {
    return Status(Code::kIOError, msg, msg2);
}
Status Status::IOError(SubCode msg) { return Status(Code::kIOError, msg); }

Status Status::MergeInProgress(const Slice& msg, const Slice& msg2) {
    return Status(Code::kMergeInProgress, msg, msg2);
}
Status Status::MergeInProgress(SubCode msg) {
    return Status(Code::kMergeInProgress, msg);
}

Status Status::Incomplete(const Slice& msg, const Slice& msg2) {
    return Status(Code::kIncomplete, msg, msg2);
}
Status Status::Incomplete(SubCode msg) {
    return Status(Code::kIncomplete, msg);
}

Status Status::ShutdownInProgress(SubCode msg) {
    return Status(Code::kShutdownInProgress, msg);
}
Status Status::ShutdownInProgress(const Slice& msg,
                                 const Slice& msg2) {
    return Status(Code::kShutdownInProgress, msg, msg2);
}
Status Status::Aborted(SubCode msg) { return Status(Code::kAborted, msg); }
Status Status::Aborted(const Slice& msg, const Slice& msg2) {
    return Status(Code::kAborted, msg, msg2);
}

Status Status::Busy(SubCode msg) { return Status(Code::kBusy, msg); }
Status Status::Busy(const Slice& msg, const Slice& msg2) {
    return Status(Code::kBusy, msg, msg2);
}

Status Status::TimedOut(SubCode msg) { return Status(Code::kTimedOut, msg); }
Status Status::TimedOut(const Slice& msg, const Slice& msg2) {
    return Status(Code::kTimedOut, msg, msg2);
}

Status Status::Expired(SubCode msg) { return Status(Code::kExpired, msg); }
Status Status::Expired(const Slice& msg, const Slice& msg2) {
    return Status(Code::kExpired, msg, msg2);
}

Status Status::TryAgain(SubCode msg) { return Status(Code::kTryAgain, msg); }
Status Status::TryAgain(const Slice& msg, const Slice& msg2) {
    return Status(Code::kTryAgain, msg, msg2);
}

Status Status::CompactionTooLarge(SubCode msg) {
    return Status(Code::kCompactionTooLarge, msg);
}
Status Status::CompactionTooLarge(const Slice& msg, const Slice& msg2) {
    return Status(Code::kCompactionTooLarge, msg, msg2);
}

Status Status::ColumnFamilyDropped(SubCode msg) {
    return Status(Code::kColumnFamilyDropped, msg);
}

Status Status::ColumnFamilyDropped(const Slice& msg, const Slice& msg2) {
    return Status(Code::kColumnFamilyDropped, msg, msg2);
}

Status Status::NoSpace() { return Status(Code::kIOError, SubCode::kNoSpace); }

Status Status::NoSpace(const Slice& msg, const Slice& msg2) {
    return Status(Code::kIOError, SubCode::kNoSpace, msg, msg2);
}
Status Status::MemoryLimit() { return Status(Code::kAborted, SubCode::kMemoryLimit); }

Status Status::MemoryLimit(const Slice& msg, const Slice& msg2) {
    return Status(Code::kAborted, SubCode::kMemoryLimit, msg, msg2);
}

Status Status::SpaceLimit() { return Status(Code::kIOError, SubCode::kSpaceLimit); }

Status Status::SpaceLimit(const Slice& msg, const Slice& msg2) {
    return Status(Code::kIOError, SubCode::kSpaceLimit, msg, msg2);
}

Status Status::PathNotFound() { return Status(Code::kIOError, SubCode::kPathNotFound); }

Status Status::PathNotFound(const Slice& msg, const Slice& msg2) {
    return Status(Code::kIOError, SubCode::kPathNotFound, msg, msg2);
}

Status Status::TxnNotPrepared() {
    return Status(Code::kInvalidArgument, SubCode::kTxnNotPrepared);
}

Status Status::TxnNotPrepared(const Slice& msg, const Slice& msg2) {
    return Status(Code::kInvalidArgument, SubCode::kTxnNotPrepared, msg, msg2);
}

bool Status::ok() const {
    MarkChecked();
    return code() == Code::kOk;
}

bool Status::IsOkOverwritten() const {
    MarkChecked();
    return code() == Code::kOk && subcode() == SubCode::kOverwritten;
}

bool Status::IsNotFound() const {
    MarkChecked();
    return code() == Code::kNotFound;
}

bool Status::IsCorruption() const {
    MarkChecked();
    return code() == Code::kCorruption;
}

bool Status::IsNotSupported() const {
    MarkChecked();
    return code() == Code::kNotSupported;
}

bool Status::IsInvalidArgument() const {
    MarkChecked();
    return code() == Code::kInvalidArgument;
}

bool Status::IsIOError() const {
    MarkChecked();
    return code() == Code::kIOError;
}

bool Status::IsMergeInProgress() const {
    MarkChecked();
    return code() == Code::kMergeInProgress;
}

bool Status::IsIncomplete() const {
    MarkChecked();
    return code() == Code::kIncomplete;
}

bool Status::IsShutdownInProgress() const {
    MarkChecked();
    return code() == Code::kShutdownInProgress;
}

bool Status::IsTimedOut() const {
    MarkChecked();
    return code() == Code::kTimedOut;
}

bool Status::IsAborted() const {
    MarkChecked();
    return code() == Code::kAborted;
}

bool Status::IsLockLimit() const {
    MarkChecked();
    return code() == Code::kAborted && subcode() == SubCode::kLockLimit;
}

bool Status::IsBusy() const {
    MarkChecked();
    return code() == Code::kBusy;
}

bool Status::IsDeadlock() const {
    MarkChecked();
    return code() == Code::kBusy && subcode() == SubCode::kDeadlock;
}

bool Status::IsExpired() const {
    MarkChecked();
    return code() == Code::kExpired;
}

bool Status::IsTryAgain() const {
    MarkChecked();
    return code() == Code::kTryAgain;
}

bool Status::IsCompactionTooLarge() const {
    MarkChecked();
    return code() == Code::kCompactionTooLarge;
}

bool Status::IsColumnFamilyDropped() const {
    MarkChecked();
    return code() == Code::kColumnFamilyDropped;
}

bool Status::IsNoSpace() const {
    MarkChecked();
    return (code() == Code::kIOError) && (subcode() == SubCode::kNoSpace);
}

bool Status::IsMemoryLimit() const {
    MarkChecked();
    return (code() == Code::kAborted) && (subcode() == SubCode::kMemoryLimit);
}

bool Status::IsPathNotFound() const {
    MarkChecked();
    return (code() == Code::kIOError || code() == Code::kNotFound) &&
           (subcode() == SubCode::kPathNotFound);
}

bool Status::IsManualCompactionPaused() const {
    MarkChecked();
    return (code() == Code::kIncomplete) && (subcode() == SubCode::kManualCompactionPaused);
}

bool Status::IsTxnNotPrepared() const {
    MarkChecked();
    return (code() == Code::kInvalidArgument) && (subcode() == SubCode::kTxnNotPrepared);
}

bool Status::IsIOFenced() const {
    MarkChecked();
    return (code() == Code::kIOError) && (subcode() == SubCode::kIOFenced);
}

Status::Status(Code _code, SubCode _subcode)
    : code_(_code),
    subcode_(_subcode),
    sev_(Severity::kNoError),
    retryable_(false),
    data_loss_(false),
    scope_(0) {}
Status::Status(Code _code, SubCode _subcode, bool retryable, bool data_loss,
    unsigned char scope)
    : code_(_code),
    subcode_(_subcode),
    sev_(Severity::kNoError),
    retryable_(retryable),
    data_loss_(data_loss),
    scope_(scope) {}

Status::Status(Code _code, const Slice& msg, const Slice& msg2)
    : Status(_code, SubCode::kNone, msg, msg2) {}

inline void Status::MarkChecked() const {
#ifdef ROCKSDB_ASSERT_STATUS_CHECKED
    checked_ = true;
#endif  // ROCKSDB_ASSERT_STATUS_CHECKED
}

Status::Status(const Status& s)
        : code_(s.code_),
          subcode_(s.subcode_),
          sev_(s.sev_),
          retryable_(s.retryable_),
          data_loss_(s.data_loss_),
          scope_(s.scope_) {
    s.MarkChecked();
    state_ = (s.state_ == nullptr) ? nullptr : CopyState(s.state_.get());
}

Status::Status(const Status& s, Severity sev)
        : code_(s.code_),
          subcode_(s.subcode_),
          sev_(sev),
          retryable_(s.retryable_),
          data_loss_(s.data_loss_),
          scope_(s.scope_) {
    s.MarkChecked();
    state_ = (s.state_ == nullptr) ? nullptr : CopyState(s.state_.get());
}

Status& Status::operator=(const Status& s) {
    if (this != &s) {
        s.MarkChecked();
        MustCheck();
        code_ = s.code_;
        subcode_ = s.subcode_;
        sev_ = s.sev_;
        retryable_ = s.retryable_;
        data_loss_ = s.data_loss_;
        scope_ = s.scope_;
        state_ = (s.state_ == nullptr) ? nullptr : CopyState(s.state_.get());
    }
    return *this;
}

Status::Status(Status&& s) noexcept : Status() {
    s.MarkChecked();
    *this = std::move(s);
}

Status& Status::operator=(Status&& s) noexcept {
    if (this != &s) {
        s.MarkChecked();
        MustCheck();
        code_ = std::move(s.code_);
        s.code_ = Code::kOk;
        subcode_ = std::move(s.subcode_);
        s.subcode_ = SubCode::kNone;
        sev_ = std::move(s.sev_);
        s.sev_ = Severity::kNoError;
        retryable_ = std::move(s.retryable_);
        s.retryable_ = false;
        data_loss_ = std::move(s.data_loss_);
        s.data_loss_ = false;
        scope_ = std::move(s.scope_);
        s.scope_ = 0;
        state_ = std::move(s.state_);
    }
    return *this;
}

bool Status::operator==(const Status& rhs) const {
    MarkChecked();
    rhs.MarkChecked();
    return (code_ == rhs.code_);
}

bool Status::operator!=(const Status& rhs) const {
    MarkChecked();
    rhs.MarkChecked();
    return !(*this == rhs);
}

std::unique_ptr<const char[]> Status::CopyState(const char* s) {
  const size_t cch = std::strlen(s) + 1;  // +1 for the null terminator
  char* rv = new char[cch];
  std::strncpy(rv, s, cch);
  return std::unique_ptr<const char[]>(rv);
}

static const char* msgs[static_cast<int>(Status::SubCode::kMaxSubCode)] = {
    "",                                                   // kNone
    "Timeout Acquiring Mutex",                            // kMutexTimeout
    "Timeout waiting to lock key",                        // kLockTimeout
    "Failed to acquire lock due to max_num_locks limit",  // kLockLimit
    "No space left on device",                            // kNoSpace
    "Deadlock",                                           // kDeadlock
    "Stale file handle",                                  // kStaleFile
    "Memory limit reached",                               // kMemoryLimit
    "Space limit reached",                                // kSpaceLimit
    "No such file or directory",                          // kPathNotFound
    // KMergeOperandsInsufficientCapacity
    "Insufficient capacity for merge operands",
    // kManualCompactionPaused
    "Manual compaction paused",
    " (overwritten)",         // kOverwritten, subcode of OK
    "Txn not prepared",       // kTxnNotPrepared
    "IO fenced off",          // kIOFenced
    "Merge operator failed",  // kMergeOperatorFailed
};

Status::Status(Code _code, SubCode _subcode, const Slice& msg,
               const Slice& msg2, Severity sev)
    : code_(_code),
      subcode_(_subcode),
      sev_(sev),
      retryable_(false),
      data_loss_(false),
      scope_(0) {
  assert(subcode_ != SubCode::kMaxSubCode);
  const size_t len1 = msg.size();
  const size_t len2 = msg2.size();
  const size_t size = len1 + (len2 ? (2 + len2) : 0);
  char* const result = new char[size + 1];  // +1 for null terminator
  memcpy(result, msg.data(), len1);
  if (len2) {
    result[len1] = ':';
    result[len1 + 1] = ' ';
    memcpy(result + len1 + 2, msg2.data(), len2);
  }
  result[size] = '\0';  // null terminator for C style string
  state_.reset(result);
}

Status Status::CopyAppendMessage(const Status& s, const Slice& delim,
                                 const Slice& msg) {
  // (No attempt at efficiency)
  return Status(s.code(), s.subcode(), s.severity(),
                std::string(s.getState()) + delim.ToString() + msg.ToString());
}

std::string Status::ToString() const {
#ifdef ROCKSDB_ASSERT_STATUS_CHECKED
  checked_ = true;
#endif  // ROCKSDB_ASSERT_STATUS_CHECKED
  const char* type = nullptr;
  switch (code_) {
    case Code::kOk:
      return "OK";
    case Code::kNotFound:
      type = "NotFound: ";
      break;
    case Code::kCorruption:
      type = "Corruption: ";
      break;
    case Code::kNotSupported:
      type = "Not implemented: ";
      break;
    case Code::kInvalidArgument:
      type = "Invalid argument: ";
      break;
    case Code::kIOError:
      type = "IO error: ";
      break;
    case Code::kMergeInProgress:
      type = "Merge in progress: ";
      break;
    case Code::kIncomplete:
      type = "Result incomplete: ";
      break;
    case Code::kShutdownInProgress:
      type = "Shutdown in progress: ";
      break;
    case Code::kTimedOut:
      type = "Operation timed out: ";
      break;
    case Code::kAborted:
      type = "Operation aborted: ";
      break;
    case Code::kBusy:
      type = "Resource busy: ";
      break;
    case Code::kExpired:
      type = "Operation expired: ";
      break;
    case Code::kTryAgain:
      type = "Operation failed. Try again.: ";
      break;
    case Code::kCompactionTooLarge:
      type = "Compaction too large: ";
      break;
    case Code::kColumnFamilyDropped:
      type = "Column family dropped: ";
      break;
    case Code::kMaxCode:
      assert(false);
      break;
  }
  char tmp[30];
  if (type == nullptr) {
    // This should not happen since `code_` should be a valid non-`kMaxCode`
    // member of the `Code` enum. The above switch-statement should have had a
    // case assigning `type` to a corresponding string.
    assert(false);
    snprintf(tmp, sizeof(tmp), "Unknown code(%d): ", static_cast<int>(code()));
    type = tmp;
  }
  std::string result(type);
  if (subcode_ != SubCode::kNone) {
    uint32_t index = static_cast<int32_t>(subcode_);
    assert(sizeof(msgs) / sizeof(msgs[0]) > index);
    result.append(msgs[index]);
  }

  if (state_ != nullptr) {
    if (subcode_ != SubCode::kNone) {
      result.append(": ");
    }
    result.append(state_.get());
  }
  return result;
}

}  // namespace ROCKSDB_NAMESPACE
