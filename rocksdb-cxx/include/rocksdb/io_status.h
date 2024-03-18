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

  explicit IOStatus(rocksdb_rs::status::Code _code,
                    rocksdb_rs::status::SubCode _subcode)
      : oxidize_(rocksdb_rs::io_status::IOStatus_new(_code, _subcode)) {}

  explicit IOStatus(rocksdb_rs::status::Code _code)
      : oxidize_(rocksdb_rs::io_status::IOStatus_new(_code)) {}

  IOStatus(rocksdb_rs::status::Code _code, rocksdb_rs::status::SubCode _subcode,
           const Slice &msg, const Slice &msg2);

  IOStatus(rocksdb_rs::status::Code _code, const Slice &msg, const Slice &msg2)
      : oxidize_(rocksdb_rs::io_status::IOStatus_new(_code, msg, msg2)) {}

  IOStatus(rocksdb_rs::io_status::IOStatus &&s) : oxidize_(std::move(s)) {}

  // TODO: move to rust
  // Copy the specified status.
  IOStatus(const IOStatus &s);

  // TODO: move to rust
  IOStatus &operator=(const IOStatus &s);

  // TODO: move to rust
  IOStatus(IOStatus &&s) noexcept;

  // TODO: move to rust
  IOStatus &operator=(IOStatus &&s) noexcept;

  bool operator==(const IOStatus &rhs) const;

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

  // Return a string representation of this status suitable for printing.
  // Returns the string "OK" for success.
  // std::string ToString() const;

 private:
  rocksdb_rs::io_status::IOStatus oxidize_;

  // TODO: move to rust
  friend IOStatus status_to_io_status(rocksdb_rs::status::Status &&);
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

inline bool IOStatus::operator==(const IOStatus &rhs) const {
  return (oxidize_ == rhs.oxidize_);
}

inline bool IOStatus::operator!=(const IOStatus &rhs) const {
  return oxidize_ != rhs.oxidize_;
}

// TODO: move to rust
inline IOStatus status_to_io_status(rocksdb_rs::status::Status &&status) {
  IOStatus io_s;
  rocksdb_rs::status::Status &s = io_s.oxidize_.status_;
  s = std::move(status);
  return io_s;
}

// Return a success status.
static IOStatus IOStatus_OK() {
  return IOStatus(rocksdb_rs::io_status::IOStatus_OK());
}

static IOStatus IOStatus_NotSupported(const Slice &msg, const Slice &msg2) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_NotSupported(msg, msg2));
}

static IOStatus IOStatus_NotSupported(const Slice &msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_NotSupported(msg));
}

static IOStatus IOStatus_NotSupported(rocksdb_rs::status::SubCode msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_NotSupported(msg));
}

static IOStatus IOStatus_NotSupported() {
  return IOStatus(rocksdb_rs::io_status::IOStatus_NotSupported());
}

// Return error status of an appropriate type.
static IOStatus IOStatus_NotFound(const Slice &msg, const Slice &msg2) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_NotFound(msg, msg2));
}

// Return error status of an appropriate type.
static IOStatus IOStatus_NotFound(const Slice &msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_NotFound(msg));
}

// Fast path for not found without malloc;
static IOStatus IOStatus_NotFound(rocksdb_rs::status::SubCode msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_NotFound(msg));
}

// Fast path for not found without malloc;
static IOStatus IOStatus_NotFound() {
  return IOStatus(rocksdb_rs::io_status::IOStatus_NotFound());
}

static IOStatus IOStatus_Corruption(const Slice &msg, const Slice &msg2) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_Corruption(msg, msg2));
}

static IOStatus IOStatus_Corruption(const Slice &msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_Corruption(msg));
}

static IOStatus IOStatus_Corruption(rocksdb_rs::status::SubCode msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_Corruption(msg));
}

static IOStatus IOStatus_Corruption() {
  return IOStatus(rocksdb_rs::io_status::IOStatus_Corruption());
}

static IOStatus IOStatus_InvalidArgument(const Slice &msg, const Slice &msg2) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_InvalidArgument(msg, msg2));
}

static IOStatus IOStatus_InvalidArgument(const Slice &msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_InvalidArgument(msg));
}

static IOStatus IOStatus_InvalidArgument(rocksdb_rs::status::SubCode msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_InvalidArgument(msg));
}

static IOStatus IOStatus_InvalidArgument() {
  return IOStatus(rocksdb_rs::io_status::IOStatus_InvalidArgument());
}

static IOStatus IOStatus_IOError(const Slice &msg, const Slice &msg2) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_IOError(msg, msg2));
}

static IOStatus IOStatus_IOError(const Slice &msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_IOError(msg));
}

static IOStatus IOStatus_IOError(rocksdb_rs::status::SubCode msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_IOError(msg));
}

static IOStatus IOStatus_IOError() {
  return IOStatus(rocksdb_rs::io_status::IOStatus_IOError());
}

static IOStatus IOStatus_Busy(rocksdb_rs::status::SubCode msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_Busy(msg));
}

static IOStatus IOStatus_Busy() {
  return IOStatus(rocksdb_rs::io_status::IOStatus_Busy());
}

static IOStatus IOStatus_Busy(const Slice &msg, const Slice &msg2) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_Busy(msg, msg2));
}

static IOStatus IOStatus_Busy(const Slice &msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_Busy(msg));
}

static IOStatus IOStatus_TimedOut(rocksdb_rs::status::SubCode msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_TimedOut(msg));
}

static IOStatus IOStatus_TimedOut() {
  return IOStatus(rocksdb_rs::io_status::IOStatus_TimedOut());
}

static IOStatus IOStatus_TimedOut(const Slice &msg, const Slice &msg2) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_TimedOut(msg, msg2));
}

static IOStatus IOStatus_TimedOut(const Slice &msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_TimedOut(msg));
}

static IOStatus IOStatus_NoSpace() {
  return IOStatus(rocksdb_rs::io_status::IOStatus_NoSpace());
}

static IOStatus IOStatus_NoSpace(const Slice &msg, const Slice &msg2) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_NoSpace(msg, msg2));
}

static IOStatus IOStatus_NoSpace(const Slice &msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_NoSpace(msg));
}

static IOStatus IOStatus_PathNotFound() {
  return IOStatus(rocksdb_rs::io_status::IOStatus_PathNotFound());
}

static IOStatus IOStatus_PathNotFound(const Slice &msg, const Slice &msg2) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_PathNotFound(msg, msg2));
}

static IOStatus IOStatus_PathNotFound(const Slice &msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_PathNotFound(msg));
}

static IOStatus IOStatus_IOFenced() {
  return IOStatus(rocksdb_rs::io_status::IOStatus_IOFenced());
}

static IOStatus IOStatus_IOFenced(const Slice &msg, const Slice &msg2) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_IOFenced(msg, msg2));
}

static IOStatus IOStatus_IOFenced(const Slice &msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_IOFenced(msg));
}

static IOStatus IOStatus_Aborted(rocksdb_rs::status::SubCode msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_Aborted(msg));
}

static IOStatus IOStatus_Aborted() {
  return IOStatus(rocksdb_rs::io_status::IOStatus_Aborted());
}

static IOStatus IOStatus_Aborted(const Slice &msg, const Slice &msg2) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_Aborted(msg, msg2));
}

static IOStatus IOStatus_Aborted(const Slice &msg) {
  return IOStatus(rocksdb_rs::io_status::IOStatus_Aborted(msg));
}
}  // namespace rocksdb
