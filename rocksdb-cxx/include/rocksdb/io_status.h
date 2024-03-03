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

#include <memory>
#include <string>

#include "rocksdb/slice.h"
#ifdef OS_WIN
#include <string.h>
#endif
#include <cstring>

#include "rocksdb-rs/src/status.rs.h"

#include "rocksdb-rs/src/status.rs.h"

namespace rocksdb {

class IOStatus {
 public:
  enum IOErrorScope : unsigned char {
    kIOErrorScopeFileSystem,
    kIOErrorScopeFile,
    kIOErrorScopeRange,
    kIOErrorScopeMax,
  };

  // Create a success status.
  IOStatus() : IOStatus(rocksdb_rs::status::Code::kOk, rocksdb_rs::status::SubCode::kNone) {}

  // Copy the specified status.
  IOStatus(const IOStatus& s);
  IOStatus& operator=(const IOStatus& s);
  IOStatus(IOStatus&& s) noexcept;
  IOStatus& operator=(IOStatus&& s) noexcept;
  bool operator==(const IOStatus& rhs) const;
  bool operator!=(const IOStatus& rhs) const;

  void SetRetryable(bool retryable) { status_.retryable = retryable; }
  void SetDataLoss(bool data_loss) { status_.data_loss = data_loss; }
  void SetScope(IOErrorScope scope) {
      status_.scope = static_cast<unsigned char>(scope);
  }

  bool GetRetryable() const { return status_.retryable; }
  bool GetDataLoss() const { return status_.data_loss; }
  IOErrorScope GetScope() const { return static_cast<IOErrorScope>(status_.scope); }

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
    operator Status() const { return status_.Clone(); }

    rocksdb_rs::status::SubCode subcode() const { return status_.subcode(); }
    std::string ToString() const { return *status_.ToString(); }
    const char* getState() const { return status_.getState()->c_str(); }

  // Return a success status.
  static IOStatus OK() { return IOStatus(); }

  static IOStatus NotSupported(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kNotSupported, msg, msg2);
  }
  static IOStatus NotSupported(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kNotSupported, msg);
  }

  // Return error status of an appropriate type.
  static IOStatus NotFound(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kNotFound, msg, msg2);
  }
  // Fast path for not found without malloc;
  static IOStatus NotFound(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kNotFound, msg);
  }

  static IOStatus Corruption(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kCorruption, msg, msg2);
  }
  static IOStatus Corruption(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kCorruption, msg);
  }

  static IOStatus InvalidArgument(const Slice& msg,
                                  const Slice& msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kInvalidArgument, msg, msg2);
  }
  static IOStatus InvalidArgument(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kInvalidArgument, msg);
  }

  static IOStatus IOError(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kIOError, msg, msg2);
  }
  static IOStatus IOError(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kIOError, msg);
  }

  static IOStatus Busy(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) { return IOStatus(rocksdb_rs::status::Code::kBusy, msg); }
  static IOStatus Busy(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kBusy, msg, msg2);
  }

  static IOStatus TimedOut(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kTimedOut, msg);
  }
  static IOStatus TimedOut(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kTimedOut, msg, msg2);
  }

  static IOStatus NoSpace() { return IOStatus(rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace); }
  static IOStatus NoSpace(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace, msg, msg2);
  }

  static IOStatus PathNotFound() { return IOStatus(rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kPathNotFound); }
  static IOStatus PathNotFound(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kPathNotFound, msg, msg2);
  }

  static IOStatus IOFenced() { return IOStatus(rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced); }
  static IOStatus IOFenced(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced, msg, msg2);
  }

  static IOStatus Aborted(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kAborted, msg);
  }
  static IOStatus Aborted(const Slice& msg, const Slice& msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kAborted, msg, msg2);
  }

  // Return a string representation of this status suitable for printing.
  // Returns the string "OK" for success.
  // std::string ToString() const;

 private:
    Status status_;

  friend IOStatus status_to_io_status(Status&&);

  explicit IOStatus(rocksdb_rs::status::Code _code, rocksdb_rs::status::SubCode _subcode = rocksdb_rs::status::SubCode::kNone)
    : status_(Status_new(_code, _subcode, false, false, kIOErrorScopeFileSystem)) {}

  IOStatus(rocksdb_rs::status::Code _code, rocksdb_rs::status::SubCode _subcode, const Slice& msg, const Slice& msg2);
  IOStatus(rocksdb_rs::status::Code _code, const Slice& msg, const Slice& msg2)
      : IOStatus(_code, rocksdb_rs::status::SubCode::kNone, msg, msg2) {}
};

inline IOStatus::IOStatus(rocksdb_rs::status::Code _code, rocksdb_rs::status::SubCode _subcode, const Slice& msg, const Slice& msg2)
    : status_(Status_new(_code, _subcode, false, false, kIOErrorScopeFileSystem)) {
    assert(status_.code_ != rocksdb_rs::status::Code::kOk);
    assert(status_.subcode_ != rocksdb_rs::status::SubCode::kMaxSubCode);
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
    status_.state = std::make_unique<std::string>(result);
}

inline IOStatus::IOStatus(const IOStatus& s)
    : status_(Status_new(
        s.status_.code_,
        s.status_.subcode_,
        s.status_.retryable,
        s.status_.data_loss,
        s.status_.scope)) {
    status_.state = s.status_.state == nullptr ? nullptr : Status_CopyState(*s.status_.state);
}

inline IOStatus& IOStatus::operator=(const IOStatus& s) {
  // The following condition catches both aliasing (when this == &s),
  // and the common case where both s and *this are ok.
  if (this != &s) {
    status_.code_ = s.status_.code_;
    status_.subcode_ = s.status_.subcode_;
    status_.retryable = s.status_.retryable;
    status_.data_loss = s.status_.data_loss;
    status_.scope = s.status_.scope;
      status_.state = s.status_.state == nullptr ? nullptr : Status_CopyState(*s.status_.state);
  }
  return *this;
}

inline IOStatus::IOStatus(IOStatus&& s) noexcept : IOStatus() {
  *this = std::move(s);
}

inline IOStatus& IOStatus::operator=(IOStatus&& s) noexcept {
  if (this != &s) {
    status_.code_ = s.status_.code_;
    s.status_.code_ = rocksdb_rs::status::Code::kOk;
    status_.subcode_ = s.status_.subcode_;
    s.status_.subcode_ = rocksdb_rs::status::SubCode::kNone;
    status_.retryable = s.status_.retryable;
    status_.data_loss = s.status_.data_loss;
    status_.scope = s.status_.scope;
    s.status_.scope = kIOErrorScopeFileSystem;
    status_.state = std::move(s.status_.state);
  }
  return *this;
}

inline bool IOStatus::operator==(const IOStatus& rhs) const {
  return (status_.code_ == rhs.status_.code_);
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

}  // namespace rocksdb
