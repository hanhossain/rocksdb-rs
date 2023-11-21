// Copyright (c) 2019-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// An IOStatus encapsulates the result of an operation.  It may indicate
// success, or it may indicate an error with an associated error message.
//
// Multiple threads can invoke const methods on an IOStatus without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same IOStatus must use
// external synchronization.

#pragma once

#include <string>

#include "rocksdb/slice.h"
#ifdef OS_WIN
#include <string.h>
#endif
#include <cstring>

#include "status.h"

#ifndef ROCKSDB_RS
#include "rocksdb-rs-cxx/status.h"
#else
#include "rocksdb-rs/src/status.rs.h"
#endif

namespace ROCKSDB_NAMESPACE {

class IOStatus {
 public:
  enum IOErrorScope : unsigned char {
    kIOErrorScopeFileSystem,
    kIOErrorScopeFile,
    kIOErrorScopeRange,
    kIOErrorScopeMax,
  };

  // Create a success status.
  IOStatus() : IOStatus(Code::kOk, SubCode::kNone) {}

  // Copy the specified status.
  IOStatus(const IOStatus& s);
  IOStatus& operator=(const IOStatus& s);
  IOStatus(IOStatus&& s) noexcept;
  IOStatus& operator=(IOStatus&& s) noexcept;
  bool operator==(const IOStatus& rhs) const;
  bool operator!=(const IOStatus& rhs) const;

  void SetRetryable(bool retryable) { status_.rs_status_.retryable = retryable; }
  void SetDataLoss(bool data_loss) { status_.rs_status_.data_loss = data_loss; }
  void SetScope(IOErrorScope scope) {
      status_.rs_status_.scope = static_cast<unsigned char>(scope);
  }

  bool GetRetryable() const { return status_.rs_status_.retryable; }
  bool GetDataLoss() const { return status_.rs_status_.data_loss; }
  IOErrorScope GetScope() const { return static_cast<IOErrorScope>(status_.rs_status_.scope); }

    bool ok() const { return status_.ok(); }
    bool IsNotFound() const { return status_.IsNotFound(); }
    bool IsNotSupported() const { return status_.IsNotSupported(); }
    bool IsIncomplete() const { return status_.IsIncomplete(); }
    bool IsCorruption() const { return status_.IsCorruption(); }
    bool IsBusy() const { return status_.IsBusy(); }
    bool IsIOFenced() const { return status_.IsIOFenced(); }
    bool IsIOError() const { return status_.IsIOError(); }
    bool IsShutdownInProgress() const { return status_.IsShutdownInProgress(); }
    bool IsColumnFamilyDropped() const { return status_.IsColumnFamilyDropped(); }
    bool IsTryAgain() const { return status_.IsTryAgain(); }
    bool IsAborted() const { return status_.IsAborted(); }
    bool IsPathNotFound() const { return status_.IsPathNotFound(); }
    bool IsInvalidArgument() const { return status_.IsInvalidArgument(); }
    operator Status() const { return status_; }

    SubCode subcode() const { return status_.subcode(); }
    std::string ToString() const { return status_.ToString(); }
    const char* getState() const { return status_.getState(); }

  // Return a success status.
  static IOStatus OK() { return IOStatus(); }

  static IOStatus NotSupported(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(Code::kNotSupported, msg, msg2);
  }
  static IOStatus NotSupported(SubCode msg = SubCode::kNone) {
    return IOStatus(Code::kNotSupported, msg);
  }

  // Return error status of an appropriate type.
  static IOStatus NotFound(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(Code::kNotFound, msg, msg2);
  }
  // Fast path for not found without malloc;
  static IOStatus NotFound(SubCode msg = SubCode::kNone) {
    return IOStatus(Code::kNotFound, msg);
  }

  static IOStatus Corruption(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(Code::kCorruption, msg, msg2);
  }
  static IOStatus Corruption(SubCode msg = SubCode::kNone) {
    return IOStatus(Code::kCorruption, msg);
  }

  static IOStatus InvalidArgument(const Slice& msg,
                                  const Slice& msg2 = Slice()) {
    return IOStatus(Code::kInvalidArgument, msg, msg2);
  }
  static IOStatus InvalidArgument(SubCode msg = SubCode::kNone) {
    return IOStatus(Code::kInvalidArgument, msg);
  }

  static IOStatus IOError(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(Code::kIOError, msg, msg2);
  }
  static IOStatus IOError(SubCode msg = SubCode::kNone) {
    return IOStatus(Code::kIOError, msg);
  }

  static IOStatus Busy(SubCode msg = SubCode::kNone) { return IOStatus(Code::kBusy, msg); }
  static IOStatus Busy(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(Code::kBusy, msg, msg2);
  }

  static IOStatus TimedOut(SubCode msg = SubCode::kNone) {
    return IOStatus(Code::kTimedOut, msg);
  }
  static IOStatus TimedOut(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(Code::kTimedOut, msg, msg2);
  }

  static IOStatus NoSpace() { return IOStatus(Code::kIOError, SubCode::kNoSpace); }
  static IOStatus NoSpace(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(Code::kIOError, SubCode::kNoSpace, msg, msg2);
  }

  static IOStatus PathNotFound() { return IOStatus(Code::kIOError, SubCode::kPathNotFound); }
  static IOStatus PathNotFound(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(Code::kIOError, SubCode::kPathNotFound, msg, msg2);
  }

  static IOStatus IOFenced() { return IOStatus(Code::kIOError, SubCode::kIOFenced); }
  static IOStatus IOFenced(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(Code::kIOError, SubCode::kIOFenced, msg, msg2);
  }

  static IOStatus Aborted(SubCode msg = SubCode::kNone) {
    return IOStatus(Code::kAborted, msg);
  }
  static IOStatus Aborted(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(Code::kAborted, msg, msg2);
  }

  // Return a string representation of this status suitable for printing.
  // Returns the string "OK" for success.
  // std::string ToString() const;

 private:
    Status status_;

  friend IOStatus status_to_io_status(Status&&);

  explicit IOStatus(Code _code, SubCode _subcode = SubCode::kNone) {
      status_ = Status(_code, _subcode, false, false, kIOErrorScopeFileSystem);
  }

  IOStatus(Code _code, SubCode _subcode, const Slice& msg, const Slice& msg2);
  IOStatus(Code _code, const Slice& msg, const Slice& msg2)
      : IOStatus(_code, SubCode::kNone, msg, msg2) {}
};

inline IOStatus::IOStatus(Code _code, SubCode _subcode, const Slice& msg,
                          const Slice& msg2) {
    status_ = Status(_code, _subcode, false, false, kIOErrorScopeFileSystem);
    assert(status_.rs_status_.code != Code::kOk);
    assert(status_.rs_status_.subcode != SubCode::kMaxSubCode);
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
    status_.state_.reset(result);
}

inline IOStatus::IOStatus(const IOStatus& s) {
    status_ = Status(
        s.status_.rs_status_.code,
        s.status_.rs_status_.subcode,
        s.status_.rs_status_.retryable,
        s.status_.rs_status_.data_loss,
        s.status_.rs_status_.scope);
    status_.state_ = (s.status_.state_ == nullptr) ? nullptr : Status_CopyState(s.status_.state_.get());
}

inline IOStatus& IOStatus::operator=(const IOStatus& s) {
  // The following condition catches both aliasing (when this == &s),
  // and the common case where both s and *this are ok.
  if (this != &s) {
    status_.rs_status_.code = s.status_.rs_status_.code;
    status_.rs_status_.subcode = s.status_.rs_status_.subcode;
    status_.rs_status_.retryable = s.status_.rs_status_.retryable;
    status_.rs_status_.data_loss = s.status_.rs_status_.data_loss;
    status_.rs_status_.scope = s.status_.rs_status_.scope;
    status_.state_ = (s.status_.state_ == nullptr) ? nullptr : Status_CopyState(s.status_.state_.get());
  }
  return *this;
}

inline IOStatus::IOStatus(IOStatus&& s) noexcept : IOStatus() {
  *this = std::move(s);
}

inline IOStatus& IOStatus::operator=(IOStatus&& s) noexcept {
  if (this != &s) {
    status_.rs_status_.code = s.status_.rs_status_.code;
    s.status_.rs_status_.code = Code::kOk;
    status_.rs_status_.subcode = s.status_.rs_status_.subcode;
    s.status_.rs_status_.subcode = SubCode::kNone;
    status_.rs_status_.retryable = s.status_.rs_status_.retryable;
    status_.rs_status_.data_loss = s.status_.rs_status_.data_loss;
    status_.rs_status_.scope = s.status_.rs_status_.scope;
    s.status_.rs_status_.scope = kIOErrorScopeFileSystem;
    status_.state_ = std::move(s.status_.state_);
  }
  return *this;
}

inline bool IOStatus::operator==(const IOStatus& rhs) const {
  return (status_.rs_status_.code == rhs.status_.rs_status_.code);
}

inline bool IOStatus::operator!=(const IOStatus& rhs) const {
  return !(*this == rhs);
}

inline IOStatus status_to_io_status(Status&& status) {
  IOStatus io_s;
  Status& s = io_s.status_;
  s = std::move(status);
  return io_s;
}

}  // namespace ROCKSDB_NAMESPACE
