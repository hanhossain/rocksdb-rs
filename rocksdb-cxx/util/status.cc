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
#include <memory>

#include "port/port.h"

#ifndef ROCKSDB_RS
#include "rocksdb-rs-cxx/status.h"
#include "rust/cxx.h"
#else
#include "rocksdb-rs/src/status.rs.h"
#include "rust/cxx.h"
#endif

namespace ROCKSDB_NAMESPACE {
// Create a success status.
Status::Status()
    : rs_status_(RsStatus_new(
              Code::kOk,
              SubCode::kNone,
              Severity::kNoError,
              false,
              false,
              0,
              nullptr)) {}

Status::Status(Code _code, SubCode _subcode, Severity _sev, const Slice& msg)
    : Status(_code, _subcode, msg, "", _sev) {}

Code Status::code() const {
    return rs_status_.code();
}

SubCode Status::subcode() const {
    return rs_status_.subcode();
}

Severity Status::severity() const {
    return rs_status_.severity();
}

const char* Status::getState() const {
    return rs_status_.getState() == nullptr ? nullptr : rs_status_.getState()->c_str();
}

Status Status_OK() { return Status(); }

Status Status_OkOverwritten() { return Status(Code::kOk, SubCode::kOverwritten); }

Status Status_NotFound(const Slice& msg, const Slice& msg2) {
    return Status(Code::kNotFound, msg, msg2);
}

Status Status_NotFound(SubCode msg) { return Status(Code::kNotFound, msg); }

Status Status_NotFound() { return Status_NotFound(SubCode::kNone); }

Status Status_NotFound(SubCode sc, const Slice& msg,
                       const Slice& msg2) {
    return Status(Code::kNotFound, sc, msg, msg2);
}

Status Status_Corruption(const Slice& msg, const Slice& msg2) {
    return Status(Code::kCorruption, msg, msg2);
}
Status Status_Corruption(SubCode msg) {
    return Status(Code::kCorruption, msg);
}
Status Status_Corruption() {
    return Status_Corruption(SubCode::kNone);
}

Status Status_NotSupported(const Slice& msg, const Slice& msg2) {
    return Status(Code::kNotSupported, msg, msg2);
}
Status Status_NotSupported(SubCode msg) {
    return Status(Code::kNotSupported, msg);
}
Status Status_NotSupported() {
    return Status_NotSupported(SubCode::kNone);
}

Status Status_InvalidArgument(const Slice& msg, const Slice& msg2) {
    return Status(Code::kInvalidArgument, msg, msg2);
}
Status Status_InvalidArgument(SubCode msg) {
    return Status(Code::kInvalidArgument, msg);
}
Status Status_InvalidArgument() {
    return Status_InvalidArgument(SubCode::kNone);
}

Status Status_IOError(const Slice& msg, const Slice& msg2) {
    return Status(Code::kIOError, msg, msg2);
}
Status Status_IOError(SubCode msg) { return Status(Code::kIOError, msg); }
Status Status_IOError() { return Status_IOError(SubCode::kNone); }

Status Status_MergeInProgress(const Slice& msg, const Slice& msg2) {
    return Status(Code::kMergeInProgress, msg, msg2);
}
Status Status_MergeInProgress(SubCode msg) {
    return Status(Code::kMergeInProgress, msg);
}
Status Status_MergeInProgress() {
    return Status_MergeInProgress(SubCode::kNone);
}

Status Status_Incomplete(const Slice& msg, const Slice& msg2) {
    return Status(Code::kIncomplete, msg, msg2);
}
Status Status_Incomplete(SubCode msg) {
    return Status(Code::kIncomplete, msg);
}
Status Status_Incomplete() {
    return Status_Incomplete(SubCode::kNone);
}

Status Status_ShutdownInProgress(SubCode msg) {
    return Status(Code::kShutdownInProgress, msg);
}
Status Status_ShutdownInProgress() {
    return Status_ShutdownInProgress(SubCode::kNone);
}
Status Status_ShutdownInProgress(const Slice& msg,
                                 const Slice& msg2) {
    return Status(Code::kShutdownInProgress, msg, msg2);
}
Status Status_Aborted(SubCode msg) { return Status(Code::kAborted, msg); }
Status Status_Aborted() { return Status_Aborted(SubCode::kNone); }
Status Status_Aborted(const Slice& msg, const Slice& msg2) {
    return Status(Code::kAborted, msg, msg2);
}

Status Status_Busy(SubCode msg) { return Status(Code::kBusy, msg); }
Status Status_Busy() { return Status_Busy(SubCode::kNone); }
Status Status_Busy(const Slice& msg, const Slice& msg2) {
    return Status(Code::kBusy, msg, msg2);
}

Status Status_TimedOut(SubCode msg) { return Status(Code::kTimedOut, msg); }
Status Status_TimedOut() { return Status_TimedOut(SubCode::kNone); }
Status Status_TimedOut(const Slice& msg, const Slice& msg2) {
    return Status(Code::kTimedOut, msg, msg2);
}

Status Status_Expired(SubCode msg) { return Status(Code::kExpired, msg); }
Status Status_Expired() { return Status_Expired(SubCode::kNone); }
Status Status_Expired(const Slice& msg, const Slice& msg2) {
    return Status(Code::kExpired, msg, msg2);
}

Status Status_TryAgain(SubCode msg) { return Status(Code::kTryAgain, msg); }
Status Status_TryAgain() { return Status_TryAgain(SubCode::kNone); }
Status Status_TryAgain(const Slice& msg, const Slice& msg2) {
    return Status(Code::kTryAgain, msg, msg2);
}

Status Status_CompactionTooLarge(SubCode msg) {
    return Status(Code::kCompactionTooLarge, msg);
}
Status Status_CompactionTooLarge() {
    return Status_CompactionTooLarge(SubCode::kNone);
}
Status Status_CompactionTooLarge(const Slice& msg, const Slice& msg2) {
    return Status(Code::kCompactionTooLarge, msg, msg2);
}

Status Status_ColumnFamilyDropped(SubCode msg) {
    return Status(Code::kColumnFamilyDropped, msg);
}
Status Status_ColumnFamilyDropped() {
    return Status_ColumnFamilyDropped(SubCode::kNone);
}
Status Status_ColumnFamilyDropped(const Slice& msg, const Slice& msg2) {
    return Status(Code::kColumnFamilyDropped, msg, msg2);
}

Status Status_NoSpace() { return Status(Code::kIOError, SubCode::kNoSpace); }

Status Status_NoSpace(const Slice& msg, const Slice& msg2) {
    return Status(Code::kIOError, SubCode::kNoSpace, msg, msg2);
}
Status Status_MemoryLimit() { return Status(Code::kAborted, SubCode::kMemoryLimit); }

Status Status_MemoryLimit(const Slice& msg, const Slice& msg2) {
    return Status(Code::kAborted, SubCode::kMemoryLimit, msg, msg2);
}

Status Status_SpaceLimit() { return Status(Code::kIOError, SubCode::kSpaceLimit); }

Status Status_SpaceLimit(const Slice& msg, const Slice& msg2) {
    return Status(Code::kIOError, SubCode::kSpaceLimit, msg, msg2);
}

Status Status_PathNotFound() { return Status(Code::kIOError, SubCode::kPathNotFound); }

Status Status_PathNotFound(const Slice& msg, const Slice& msg2) {
    return Status(Code::kIOError, SubCode::kPathNotFound, msg, msg2);
}

Status Status_TxnNotPrepared() {
    return Status(Code::kInvalidArgument, SubCode::kTxnNotPrepared);
}

Status Status_TxnNotPrepared(const Slice& msg, const Slice& msg2) {
    return Status(Code::kInvalidArgument, SubCode::kTxnNotPrepared, msg, msg2);
}

bool Status::ok() const {
    return rs_status_.ok();
}

bool Status::IsOkOverwritten() const {
    return rs_status_.IsOkOverwritten();
}

bool Status::IsNotFound() const {
    return rs_status_.IsNotFound();
}

bool Status::IsCorruption() const {
    return rs_status_.IsCorruption();
}

bool Status::IsNotSupported() const {
    return rs_status_.IsNotSupported();
}

bool Status::IsInvalidArgument() const {
    return rs_status_.IsInvalidArgument();
}

bool Status::IsIOError() const {
    return rs_status_.IsIOError();
}

bool Status::IsMergeInProgress() const {
    return rs_status_.IsMergeInProgress();
}

bool Status::IsIncomplete() const {
    return rs_status_.IsIncomplete();
}

bool Status::IsShutdownInProgress() const {
    return rs_status_.IsShutdownInProgress();
}

bool Status::IsTimedOut() const {
    return rs_status_.IsTimedOut();
}

bool Status::IsAborted() const {
    return rs_status_.IsAborted();
}

bool Status::IsLockLimit() const {
    return rs_status_.IsLockLimit();
}

bool Status::IsBusy() const {
    return rs_status_.IsBusy();
}

bool Status::IsDeadlock() const {
    return rs_status_.IsDeadlock();
}

bool Status::IsExpired() const {
    return rs_status_.IsExpired();
}

bool Status::IsTryAgain() const {
    return rs_status_.IsTryAgain();
}

bool Status::IsCompactionTooLarge() const {
    return rs_status_.IsCompactionTooLarge();
}

bool Status::IsColumnFamilyDropped() const {
    return rs_status_.IsColumnFamilyDropped();
}

bool Status::IsNoSpace() const {
    return rs_status_.IsNoSpace();
}

bool Status::IsMemoryLimit() const {
    return rs_status_.IsMemoryLimit();
}

bool Status::IsPathNotFound() const {
    return rs_status_.IsPathNotFound();
}

bool Status::IsManualCompactionPaused() const {
    return rs_status_.IsManualCompactionPaused();
}

bool Status::IsTxnNotPrepared() const {
    return rs_status_.IsTxnNotPrepared();
}

bool Status::IsIOFenced() const {
    return rs_status_.IsIOFenced();
}

Status::Status(Code _code, SubCode _subcode)
    : rs_status_(RsStatus_new(_code, _subcode)) {}

Status::Status(Code _code)
    : rs_status_(RsStatus_new(_code)) {}

Status::Status(Code _code, SubCode _subcode, bool retryable, bool data_loss,
    unsigned char scope)
    : rs_status_(RsStatus_new(_code, _subcode, retryable, data_loss, scope)) {}

Status::Status(Code _code, const Slice& msg, const Slice& msg2)
    : Status(_code, SubCode::kNone, msg, msg2) {}

Status::Status(const Status& s)
        : rs_status_(RsStatus_new(
                  s.rs_status_.code_,
                  s.rs_status_.subcode_,
                  s.rs_status_.severity_,
                  s.rs_status_.retryable,
                  s.rs_status_.data_loss,
                  s.rs_status_.scope,
                  nullptr)) {
    rs_status_.state = s.rs_status_.state == nullptr ? nullptr : Status_CopyState(s.rs_status_.state->c_str());
}

Status::Status(const Status& s, Severity sev)
        : rs_status_(RsStatus_new(
                  s.rs_status_.code_,
                  s.rs_status_.subcode_,
                  sev,
                  s.rs_status_.retryable,
                  s.rs_status_.data_loss,
                  s.rs_status_.scope,
                  nullptr)) {
    rs_status_.state = s.rs_status_.state == nullptr ? nullptr : Status_CopyState(s.rs_status_.state->c_str());
}

Status& Status::operator=(const Status& s) {
    if (this != &s) {
        rs_status_.code_ = s.rs_status_.code_;
        rs_status_.subcode_ = s.rs_status_.subcode_;
        rs_status_.severity_ = s.rs_status_.severity_;
        rs_status_.retryable = s.rs_status_.retryable;
        rs_status_.data_loss = s.rs_status_.data_loss;
        rs_status_.scope = s.rs_status_.scope;
        rs_status_.state = s.rs_status_.state == nullptr ? nullptr : Status_CopyState(s.rs_status_.state->c_str());
    }
    return *this;
}

Status::Status(Status&& s) noexcept : Status() {
    *this = std::move(s);
}

Status& Status::operator=(Status&& s) noexcept {
    if (this != &s) {
        rs_status_.code_ = s.rs_status_.code_;
        s.rs_status_.code_ = Code::kOk;
        rs_status_.subcode_ = s.rs_status_.subcode_;
        s.rs_status_.subcode_ = SubCode::kNone;
        rs_status_.severity_ = s.rs_status_.severity_;
        s.rs_status_.severity_ = Severity::kNoError;
        rs_status_.retryable = s.rs_status_.retryable;
        s.rs_status_.retryable = false;
        rs_status_.data_loss = s.rs_status_.data_loss;
        s.rs_status_.data_loss = false;
        rs_status_.scope = s.rs_status_.scope;
        s.rs_status_.scope = 0;
        rs_status_.state = std::move(s.rs_status_.state);
    }
    return *this;
}

bool Status::operator==(const Status& rhs) const {
    return rs_status_.code_ == rhs.rs_status_.code_;
}

bool Status::operator!=(const Status& rhs) const {
    return !(*this == rhs);
}

std::unique_ptr<std::string> Status_CopyState(const std::string& s) {
    auto value = std::make_unique<std::string>(s);
    return value;
}

static const char* msgs[static_cast<int>(SubCode::kMaxSubCode)] = {
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
    : rs_status_(RsStatus_new(_code, _subcode, sev, false, false, 0, nullptr)) {
  assert(rs_status_.subcode_ != SubCode::kMaxSubCode);
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
  rs_status_.state = std::make_unique<std::string>(result);
}

Status::Status(Code _code, SubCode _subcode, const Slice& msg, const Slice& msg2)
    : Status(_code, _subcode, msg, msg2, Severity::kNoError) {}

Status Status_CopyAppendMessage(const Status& s, const Slice& delim,
                                 const Slice& msg) {
  // (No attempt at efficiency)
  return Status(s.code(), s.subcode(), s.severity(),
                std::string(s.getState()) + delim.ToString() + msg.ToString());
}

std::string Status::ToString() const {
  const char* type = nullptr;
  switch (rs_status_.code_) {
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
  if (rs_status_.subcode_ != SubCode::kNone) {
    uint32_t index = static_cast<int32_t>(rs_status_.subcode_);
    assert(sizeof(msgs) / sizeof(msgs[0]) > index);
    result.append(msgs[index]);
  }

  if (rs_status_.state != nullptr) {
    if (rs_status_.subcode_ != SubCode::kNone) {
      result.append(": ");
    }
    result.append(*rs_status_.state);
  }
  return result;
}

}  // namespace ROCKSDB_NAMESPACE
