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
        // TODO: move to rust
        // Create a success status.
        IOStatus() : IOStatus(rocksdb_rs::status::Code::kOk, rocksdb_rs::status::SubCode::kNone) {
        }

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

        // TODO: move to rust
        void SetRetryable(bool retryable) { status_.retryable = retryable; }

        // TODO: move to rust
        void SetDataLoss(bool data_loss) { status_.data_loss = data_loss; }

        // TODO: move to rust
        void SetScope(rocksdb_rs::io_status::IOErrorScope scope) {
            status_.scope = static_cast<unsigned char>(scope);
        }

        // TODO: move to rust
        bool GetRetryable() const { return status_.retryable; }

        // TODO: move to rust
        bool GetDataLoss() const { return status_.data_loss; }

        // TODO: move to rust
        rocksdb_rs::io_status::IOErrorScope GetScope() const {
            return static_cast<rocksdb_rs::io_status::IOErrorScope>(status_.scope);
        }

        // TODO: move to rust
        bool ok() const { return status_.ok(); }

        // TODO: move to rust
        bool IsNotFound() const { return status_.IsNotFound(); }

        // TODO: move to rust
        bool IsNotSupported() const { return status_.IsNotSupported(); }

        // TODO: move to rust
        bool IsIncomplete() const { return status_.IsIncomplete(); }

        // TODO: move to rust
        bool IsCorruption() const { return status_.IsCorruption(); }

        // TODO: move to rust
        bool IsBusy() const { return status_.IsBusy(); }

        // TODO: move to rust
        bool IsIOFenced() const { return status_.IsIOFenced(); }

        // TODO: move to rust
        bool IsIOError() const { return status_.IsIOError(); }

        // TODO: move to rust
        bool IsShutdownInProgress() const { return status_.IsShutdownInProgress(); }

        // TODO: move to rust
        bool IsColumnFamilyDropped() const { return status_.IsColumnFamilyDropped(); }

        // TODO: move to rust
        bool IsTryAgain() const { return status_.IsTryAgain(); }

        // TODO: move to rust
        bool IsAborted() const { return status_.IsAborted(); }

        // TODO: move to rust
        bool IsPathNotFound() const { return status_.IsPathNotFound(); }

        // TODO: move to rust
        bool IsInvalidArgument() const { return status_.IsInvalidArgument(); }

        // TODO: move to rust
        operator rocksdb_rs::status::Status() const { return status_.Clone(); }

        // TODO: move to rust
        rocksdb_rs::status::SubCode subcode() const { return status_.subcode(); }

        // TODO: move to rust
        std::string ToString() const { return *status_.ToString(); }

        // TODO: move to rust
        const char *getState() const { return status_.getState()->c_str(); }

        // TODO: move to rust
        // Return a success status.
        static IOStatus OK() { return IOStatus(); }

        // TODO: move to rust
        static IOStatus NotSupported(const Slice &msg, const Slice &msg2 = Slice()) {
            return IOStatus(rocksdb_rs::status::Code::kNotSupported, msg, msg2);
        }

        // TODO: move to rust
        static IOStatus NotSupported(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
            return IOStatus(rocksdb_rs::status::Code::kNotSupported, msg);
        }

        // TODO: move to rust
        // Return error status of an appropriate type.
        static IOStatus NotFound(const Slice &msg, const Slice &msg2 = Slice()) {
            return IOStatus(rocksdb_rs::status::Code::kNotFound, msg, msg2);
        }

        // TODO: move to rust
        // Fast path for not found without malloc;
        static IOStatus NotFound(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
            return IOStatus(rocksdb_rs::status::Code::kNotFound, msg);
        }

        // TODO: move to rust
        static IOStatus Corruption(const Slice &msg, const Slice &msg2 = Slice()) {
            return IOStatus(rocksdb_rs::status::Code::kCorruption, msg, msg2);
        }

        // TODO: move to rust
        static IOStatus Corruption(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
            return IOStatus(rocksdb_rs::status::Code::kCorruption, msg);
        }

        // TODO: move to rust
        static IOStatus InvalidArgument(const Slice &msg,
                                        const Slice &msg2 = Slice()) {
            return IOStatus(rocksdb_rs::status::Code::kInvalidArgument, msg, msg2);
        }

        // TODO: move to rust
        static IOStatus InvalidArgument(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
            return IOStatus(rocksdb_rs::status::Code::kInvalidArgument, msg);
        }

        // TODO: move to rust
        static IOStatus IOError(const Slice &msg, const Slice &msg2 = Slice()) {
            return IOStatus(rocksdb_rs::status::Code::kIOError, msg, msg2);
        }

        // TODO: move to rust
        static IOStatus IOError(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
            return IOStatus(rocksdb_rs::status::Code::kIOError, msg);
        }

        // TODO: move to rust
        static IOStatus Busy(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
            return IOStatus(rocksdb_rs::status::Code::kBusy, msg);
        }

        // TODO: move to rust
        static IOStatus Busy(const Slice &msg, const Slice &msg2 = Slice()) {
            return IOStatus(rocksdb_rs::status::Code::kBusy, msg, msg2);
        }

        // TODO: move to rust
        static IOStatus TimedOut(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
            return IOStatus(rocksdb_rs::status::Code::kTimedOut, msg);
        }

        // TODO: move to rust
        static IOStatus TimedOut(const Slice &msg, const Slice &msg2 = Slice()) {
            return IOStatus(rocksdb_rs::status::Code::kTimedOut, msg, msg2);
        }

        // TODO: move to rust
        static IOStatus NoSpace() {
            return IOStatus(rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace);
        }

        // TODO: move to rust
        static IOStatus NoSpace(const Slice &msg, const Slice &msg2 = Slice()) {
            return IOStatus(rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kNoSpace, msg, msg2);
        }

        // TODO: move to rust
        static IOStatus PathNotFound() {
            return IOStatus(rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kPathNotFound);
        }

        // TODO: move to rust
        static IOStatus PathNotFound(const Slice &msg, const Slice &msg2 = Slice()) {
            return IOStatus(rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kPathNotFound, msg, msg2);
        }

        // TODO: move to rust
        static IOStatus IOFenced() {
            return IOStatus(rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced);
        }

        // TODO: move to rust
        static IOStatus IOFenced(const Slice &msg, const Slice &msg2 = Slice()) {
            return IOStatus(rocksdb_rs::status::Code::kIOError, rocksdb_rs::status::SubCode::kIOFenced, msg, msg2);
        }

        // TODO: move to rust
        static IOStatus Aborted(rocksdb_rs::status::SubCode msg = rocksdb_rs::status::SubCode::kNone) {
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
        rocksdb_rs::status::Status status_;

        // TODO: move to rust
        friend IOStatus status_to_io_status(rocksdb_rs::status::Status &&);

        // TODO: move to rust
        explicit IOStatus(rocksdb_rs::status::Code _code,
                          rocksdb_rs::status::SubCode _subcode = rocksdb_rs::status::SubCode::kNone)
            : status_(rocksdb_rs::status::Status_new(_code, _subcode, false, false,
                                                     static_cast<uint8_t>(
                                                         rocksdb_rs::io_status::IOErrorScope::kIOErrorScopeFileSystem))) {
        }

        // TODO: move to rust
        IOStatus(rocksdb_rs::status::Code _code, rocksdb_rs::status::SubCode _subcode, const Slice &msg,
                 const Slice &msg2);

        // TODO: move to rust
        IOStatus(rocksdb_rs::status::Code _code, const Slice &msg, const Slice &msg2)
            : IOStatus(_code, rocksdb_rs::status::SubCode::kNone, msg, msg2) {
        }
    };

    // TODO: move to rust
    inline IOStatus::IOStatus(rocksdb_rs::status::Code _code, rocksdb_rs::status::SubCode _subcode, const Slice &msg,
                              const Slice &msg2)
        : status_(rocksdb_rs::status::Status_new(_code, _subcode, false, false,
                                                 static_cast<uint8_t>(
                                                     rocksdb_rs::io_status::IOErrorScope::kIOErrorScopeFileSystem))) {
        assert(status_.code_ != rocksdb_rs::status::Code::kOk);
        assert(status_.subcode_ != rocksdb_rs::status::SubCode::kMaxSubCode);
        const size_t len1 = msg.size();
        const size_t len2 = msg2.size();
        const size_t size = len1 + (len2 ? (2 + len2) : 0);
        char *const result = new char[size + 1]; // +1 for null terminator
        memcpy(result, msg.data(), len1);
        if (len2) {
            result[len1] = ':';
            result[len1 + 1] = ' ';
            memcpy(result + len1 + 2, msg2.data(), len2);
        }
        result[size] = '\0'; // null terminator for C style string
        status_.state = std::make_unique<std::string>(result);
    }

    // TODO: move to rust
    inline IOStatus::IOStatus(const IOStatus &s)
        : status_(rocksdb_rs::status::Status_new(
            s.status_.code_,
            s.status_.subcode_,
            s.status_.retryable,
            s.status_.data_loss,
            s.status_.scope)) {
        status_.state = s.status_.state == nullptr ? nullptr : rocksdb_rs::status::Status_CopyState(*s.status_.state);
    }

    // TODO: move to rust
    inline IOStatus &IOStatus::operator=(const IOStatus &s) {
        // The following condition catches both aliasing (when this == &s),
        // and the common case where both s and *this are ok.
        if (this != &s) {
            status_.code_ = s.status_.code_;
            status_.subcode_ = s.status_.subcode_;
            status_.retryable = s.status_.retryable;
            status_.data_loss = s.status_.data_loss;
            status_.scope = s.status_.scope;
            status_.state =
                    s.status_.state == nullptr ? nullptr : rocksdb_rs::status::Status_CopyState(*s.status_.state);
        }
        return *this;
    }

    // TODO: move to rust
    inline IOStatus::IOStatus(IOStatus &&s) noexcept: IOStatus() {
        *this = std::move(s);
    }

    // TODO: move to rust
    inline IOStatus &IOStatus::operator=(IOStatus &&s) noexcept {
        if (this != &s) {
            status_.code_ = s.status_.code_;
            s.status_.code_ = rocksdb_rs::status::Code::kOk;
            status_.subcode_ = s.status_.subcode_;
            s.status_.subcode_ = rocksdb_rs::status::SubCode::kNone;
            status_.retryable = s.status_.retryable;
            status_.data_loss = s.status_.data_loss;
            status_.scope = s.status_.scope;
            s.status_.scope = static_cast<uint8_t>(rocksdb_rs::io_status::IOErrorScope::kIOErrorScopeFileSystem);
            status_.state = std::move(s.status_.state);
        }
        return *this;
    }

    // TODO: move to rust
    inline bool IOStatus::operator==(const IOStatus &rhs) const {
        return (status_.code_ == rhs.status_.code_);
    }

    // TODO: move to rust
    inline bool IOStatus::operator!=(const IOStatus &rhs) const {
        return !(*this == rhs);
    }

    // TODO: move to rust
    inline IOStatus status_to_io_status(rocksdb_rs::status::Status &&status) {
        IOStatus io_s;
        rocksdb_rs::status::Status &s = io_s.status_;
        s = std::move(status);
        return io_s;
    }
} // namespace rocksdb
