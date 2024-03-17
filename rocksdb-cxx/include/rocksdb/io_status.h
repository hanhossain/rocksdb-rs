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

#include "rocksdb-rs/src/io_status.rs.h"
#include "rocksdb-rs/src/status.rs.h"

namespace rocksdb {
class IOStatus {
 public:
  // Create a success status.
  IOStatus() : oxidize_(rocksdb_rs::io_status::IOStatus_new()) {}

  // TODO: move to rust
  // Copy the specified status.
  IOStatus(const IOStatus &s);

  // TODO: move to rust
  IOStatus &operator=(const IOStatus &s);

  // TODO: move to rust
  IOStatus(IOStatus &&s) noexcept;

  // TODO: move to rust
  IOStatus &operator=(IOStatus &&s) noexcept;

  // TODO: move to rust
  bool operator==(const IOStatus &rhs) const;

  // TODO: move to rust
  bool operator!=(const IOStatus &rhs) const;

  void SetRetryable(bool retryable) { oxidize_.SetRetryable(retryable); }

  void SetDataLoss(bool data_loss) { oxidize_.SetDataLoss(data_loss); }

  void SetScope(rocksdb_rs::io_status::IOErrorScope scope) {
    oxidize_.SetScope(scope);
  }

  bool GetRetryable() const { return oxidize_.GetRetryable(); }

  bool GetDataLoss() const { return oxidize_.GetDataLoss(); }

  rocksdb_rs::io_status::IOErrorScope GetScope() const {
    return oxidize_.GetScope();
  }

  bool ok() const { return oxidize_.ok(); }

  bool IsNotFound() const { return oxidize_.IsNotFound(); }

  bool IsNotSupported() const { return oxidize_.IsNotSupported(); }

  bool IsIncomplete() const { return oxidize_.IsIncomplete(); }

  bool IsCorruption() const { return oxidize_.IsCorruption(); }

  bool IsBusy() const { return oxidize_.IsBusy(); }

  bool IsIOFenced() const { return oxidize_.IsIOFenced(); }

  bool IsIOError() const { return oxidize_.IsIOError(); }

  bool IsShutdownInProgress() const { return oxidize_.IsShutdownInProgress(); }

  bool IsColumnFamilyDropped() const {
    return oxidize_.IsColumnFamilyDropped();
  }

  bool IsTryAgain() const { return oxidize_.IsTryAgain(); }

  bool IsAborted() const { return oxidize_.IsAborted(); }

  bool IsPathNotFound() const { return oxidize_.IsPathNotFound(); }

  bool IsInvalidArgument() const { return oxidize_.IsInvalidArgument(); }

  operator rocksdb_rs::status::Status() const { return oxidize_.status(); }

  rocksdb_rs::status::SubCode subcode() const { return oxidize_.subcode(); }

  std::string ToString() const { return *oxidize_.ToString(); }

  const std::unique_ptr<std::string> &getState() const {
    return oxidize_.getState();
  }

  // TODO: move to rust
  // Return a success status.
  static IOStatus OK() { return IOStatus(); }

  // TODO: move to rust
  static IOStatus NotSupported(const Slice &msg, const Slice &msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kNotSupported, msg, msg2);
  }

  // TODO: move to rust
  static IOStatus NotSupported(
      rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kNotSupported, msg);
  }

  // TODO: move to rust
  // Return error status of an appropriate type.
  static IOStatus NotFound(const Slice &msg, const Slice &msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kNotFound, msg, msg2);
  }

  // TODO: move to rust
  // Fast path for not found without malloc;
  static IOStatus NotFound(
      rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kNotFound, msg);
  }

  // TODO: move to rust
  static IOStatus Corruption(const Slice &msg, const Slice &msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kCorruption, msg, msg2);
  }

  // TODO: move to rust
  static IOStatus Corruption(
      rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kCorruption, msg);
  }

  // TODO: move to rust
  static IOStatus InvalidArgument(const Slice &msg,
                                  const Slice &msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kInvalidArgument, msg, msg2);
  }

  // TODO: move to rust
  static IOStatus InvalidArgument(
      rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kInvalidArgument, msg);
  }

  // TODO: move to rust
  static IOStatus IOError(const Slice &msg, const Slice &msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kIOError, msg, msg2);
  }

  // TODO: move to rust
  static IOStatus IOError(
      rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kIOError, msg);
  }

  // TODO: move to rust
  static IOStatus Busy(
      rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kBusy, msg);
  }

  // TODO: move to rust
  static IOStatus Busy(const Slice &msg, const Slice &msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kBusy, msg, msg2);
  }

  // TODO: move to rust
  static IOStatus TimedOut(
      rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kTimedOut, msg);
  }

  // TODO: move to rust
  static IOStatus TimedOut(const Slice &msg, const Slice &msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kTimedOut, msg, msg2);
  }

  // TODO: move to rust
  static IOStatus NoSpace() {
    return IOStatus(rocksdb_rs::status::Code::kIOError,
                    rocksdb_rs::status::SubCode::kNoSpace);
  }

  // TODO: move to rust
  static IOStatus NoSpace(const Slice &msg, const Slice &msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kIOError,
                    rocksdb_rs::status::SubCode::kNoSpace, msg, msg2);
  }

  // TODO: move to rust
  static IOStatus PathNotFound() {
    return IOStatus(rocksdb_rs::status::Code::kIOError,
                    rocksdb_rs::status::SubCode::kPathNotFound);
  }

  // TODO: move to rust
  static IOStatus PathNotFound(const Slice &msg, const Slice &msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kIOError,
                    rocksdb_rs::status::SubCode::kPathNotFound, msg, msg2);
  }

  // TODO: move to rust
  static IOStatus IOFenced() {
    return IOStatus(rocksdb_rs::status::Code::kIOError,
                    rocksdb_rs::status::SubCode::kIOFenced);
  }

  // TODO: move to rust
  static IOStatus IOFenced(const Slice &msg, const Slice &msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kIOError,
                    rocksdb_rs::status::SubCode::kIOFenced, msg, msg2);
  }

  // TODO: move to rust
  static IOStatus Aborted(
      rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
    return IOStatus(rocksdb_rs::status::Code::kAborted, msg);
  }

  // TODO: move to rust
  static IOStatus Aborted(const Slice &msg, const Slice &msg2 = Slice()) {
    return IOStatus(rocksdb_rs::status::Code::kAborted, msg, msg2);
  }

  // Return a string representation of this status suitable for printing.
  // Returns the string "OK" for success.
  // std::string ToString() const;

 private:
  rocksdb_rs::io_status::IOStatus oxidize_;

  // TODO: move to rust
  friend IOStatus status_to_io_status(rocksdb_rs::status::Status &&);

  explicit IOStatus(rocksdb_rs::status::Code _code,
                    rocksdb_rs::status::SubCode _subcode)
      : oxidize_(rocksdb_rs::io_status::IOStatus_new(_code, _subcode)) {}

  explicit IOStatus(rocksdb_rs::status::Code _code)
      : oxidize_(rocksdb_rs::io_status::IOStatus_new(_code)) {}

  IOStatus(rocksdb_rs::status::Code _code, rocksdb_rs::status::SubCode _subcode,
           const Slice &msg, const Slice &msg2);

  IOStatus(rocksdb_rs::status::Code _code, const Slice &msg, const Slice &msg2)
      : oxidize_(rocksdb_rs::io_status::IOStatus_new(_code, msg, msg2)) {}
};

inline IOStatus::IOStatus(rocksdb_rs::status::Code _code,
                          rocksdb_rs::status::SubCode _subcode,
                          const Slice &msg, const Slice &msg2)
    : oxidize_(
          rocksdb_rs::io_status::IOStatus_new(_code, _subcode, msg, msg2)) {}

// TODO: move to rust
inline IOStatus::IOStatus(const IOStatus &s)
    : oxidize_(
          rocksdb_rs::io_status::IOStatus_new(rocksdb_rs::status::Status_new(
              s.oxidize_.status_.code_, s.oxidize_.status_.subcode_,
              s.oxidize_.status_.retryable, s.oxidize_.status_.data_loss,
              s.oxidize_.status_.scope))) {
  oxidize_.status_.state =
      s.oxidize_.status_.state == nullptr
          ? nullptr
          : rocksdb_rs::status::Status_CopyState(*s.oxidize_.status_.state);
}

// TODO: move to rust
inline IOStatus &IOStatus::operator=(const IOStatus &s) {
  // The following condition catches both aliasing (when this == &s),
  // and the common case where both s and *this are ok.
  if (this != &s) {
    oxidize_.status_.code_ = s.oxidize_.status_.code_;
    oxidize_.status_.subcode_ = s.oxidize_.status_.subcode_;
    oxidize_.status_.retryable = s.oxidize_.status_.retryable;
    oxidize_.status_.data_loss = s.oxidize_.status_.data_loss;
    oxidize_.status_.scope = s.oxidize_.status_.scope;
    oxidize_.status_.state =
        s.oxidize_.status_.state == nullptr
            ? nullptr
            : rocksdb_rs::status::Status_CopyState(*s.oxidize_.status_.state);
  }
  return *this;
}

// TODO: move to rust
inline IOStatus::IOStatus(IOStatus &&s) noexcept : IOStatus() {
  *this = std::move(s);
}

// TODO: move to rust
inline IOStatus &IOStatus::operator=(IOStatus &&s) noexcept {
  if (this != &s) {
    oxidize_.status_.code_ = s.oxidize_.status_.code_;
    s.oxidize_.status_.code_ = rocksdb_rs::status::Code::kOk;
    oxidize_.status_.subcode_ = s.oxidize_.status_.subcode_;
    s.oxidize_.status_.subcode_ = rocksdb_rs::status::SubCode::kNone;
    oxidize_.status_.retryable = s.oxidize_.status_.retryable;
    oxidize_.status_.data_loss = s.oxidize_.status_.data_loss;
    oxidize_.status_.scope = s.oxidize_.status_.scope;
    s.oxidize_.status_.scope = static_cast<uint8_t>(
        rocksdb_rs::io_status::IOErrorScope::kIOErrorScopeFileSystem);
    oxidize_.status_.state = std::move(s.oxidize_.status_.state);
  }
  return *this;
}

// TODO: move to rust
inline bool IOStatus::operator==(const IOStatus &rhs) const {
  return (oxidize_.status_.code_ == rhs.oxidize_.status_.code_);
}

// TODO: move to rust
inline bool IOStatus::operator!=(const IOStatus &rhs) const {
  return !(*this == rhs);
}

// TODO: move to rust
inline IOStatus status_to_io_status(rocksdb_rs::status::Status &&status) {
  IOStatus io_s;
  rocksdb_rs::status::Status &s = io_s.oxidize_.status_;
  s = std::move(status);
  return io_s;
}
}  // namespace rocksdb
