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
#else
#include "rocksdb-rs/src/status.rs.h"
#endif

namespace ROCKSDB_NAMESPACE {
Status::Status(RsStatus _rs_status) : rs_status_(std::move(_rs_status)) {}


Status::Status(Code _code, SubCode _subcode, Severity _sev, const Slice& msg)
    : rs_status_(RsStatus_new(_code, _subcode, _sev, msg)) {}

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

Status Status_new() { return Status(RsStatus_new()); }

Status Status_OK() { return Status(RsStatus_OK()); }

Status Status_OkOverwritten() { return Status(RsStatus_OkOverwritten()); }

Status Status_NotFound(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_NotFound(msg, msg2));
}

Status Status_NotFound(const Slice& msg) {
    return Status(RsStatus_NotFound(msg));
}

Status Status_NotFound(SubCode msg) { return Status(RsStatus_NotFound(msg)); }

Status Status_NotFound() { return Status(RsStatus_NotFound()); }

Status Status_NotFound(SubCode sc, const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_NotFound(sc, msg, msg2));
}

Status Status_NotFound(SubCode sc, const Slice& msg) {
    return Status(RsStatus_NotFound(sc, msg));
}

Status Status_Corruption(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_Corruption(msg, msg2));
}
Status Status_Corruption(SubCode msg) {
    return Status(RsStatus_Corruption(msg));
}
Status Status_Corruption() {
    return Status(RsStatus_Corruption());
}

Status Status_NotSupported(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_NotSupported(msg, msg2));
}
Status Status_NotSupported(const Slice& msg) {
    return Status(RsStatus_NotSupported(msg));
}
Status Status_NotSupported(SubCode msg) {
    return Status(RsStatus_NotSupported(msg));
}
Status Status_NotSupported() {
    return Status(RsStatus_NotSupported());
}

Status Status_InvalidArgument(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_InvalidArgument(msg, msg2));
}
Status Status_InvalidArgument(const Slice& msg) {
    return Status(RsStatus_InvalidArgument(msg));
}
Status Status_InvalidArgument(SubCode msg) {
    return Status(RsStatus_InvalidArgument(msg));
}
Status Status_InvalidArgument() {
    return Status(RsStatus_InvalidArgument());
}

Status Status_IOError(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_IOError(msg, msg2));
}
Status Status_IOError(const Slice& msg) {
    return Status(RsStatus_IOError(msg));
}
Status Status_IOError(SubCode msg) { return Status(RsStatus_IOError(msg)); }
Status Status_IOError() { return Status(RsStatus_IOError()); }

Status Status_MergeInProgress(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_MergeInProgress(msg, msg2));
}
Status Status_MergeInProgress(const Slice& msg) {
    return Status(RsStatus_MergeInProgress(msg));
}
Status Status_MergeInProgress(SubCode msg) {
    return Status(RsStatus_MergeInProgress(msg));
}
Status Status_MergeInProgress() {
    return Status(RsStatus_MergeInProgress());
}

Status Status_Incomplete(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_Incomplete(msg, msg2));
}
Status Status_Incomplete(const Slice& msg) {
    return Status(RsStatus_Incomplete(msg));
}
Status Status_Incomplete(SubCode msg) {
    return Status(RsStatus_Incomplete(msg));
}
Status Status_Incomplete() {
    return Status(RsStatus_Incomplete());
}

Status Status_ShutdownInProgress(SubCode msg) {
    return Status(RsStatus_ShutdownInProgress(msg));
}
Status Status_ShutdownInProgress() {
    return Status(RsStatus_ShutdownInProgress());
}
Status Status_ShutdownInProgress(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_ShutdownInProgress(msg, msg2));
}
Status Status_ShutdownInProgress(const Slice& msg) {
    return Status(RsStatus_ShutdownInProgress(msg));
}

Status Status_Aborted(SubCode msg) { return Status(RsStatus_Aborted(msg)); }
Status Status_Aborted() { return Status(RsStatus_Aborted()); }
Status Status_Aborted(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_Aborted(msg, msg2));
}
Status Status_Aborted(const Slice& msg) {
    return Status(RsStatus_Aborted(msg));
}

Status Status_Busy(SubCode msg) { return Status(RsStatus_Busy(msg)); }
Status Status_Busy() { return Status(RsStatus_Busy()); }
Status Status_Busy(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_Busy(msg, msg2));
}
Status Status_Busy(const Slice& msg) {
    return Status(RsStatus_Busy(msg));
}

Status Status_TimedOut(SubCode msg) { return Status(RsStatus_TimedOut(msg)); }
Status Status_TimedOut() { return Status(RsStatus_TimedOut()); }
Status Status_TimedOut(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_TimedOut(msg, msg2));
}
Status Status_TimedOut(const Slice& msg) {
    return Status(RsStatus_TimedOut(msg));
}

Status Status_Expired(SubCode msg) { return Status(RsStatus_Expired(msg)); }
Status Status_Expired() { return Status(RsStatus_Expired()); }
Status Status_Expired(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_Expired(msg, msg2));
}
Status Status_Expired(const Slice& msg) {
    return Status(RsStatus_Expired(msg));
}

Status Status_TryAgain(SubCode msg) { return Status(RsStatus_TryAgain(msg)); }
Status Status_TryAgain() { return Status(RsStatus_TryAgain()); }
Status Status_TryAgain(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_TryAgain(msg, msg2));
}
Status Status_TryAgain(const Slice& msg) {
    return Status(RsStatus_TryAgain(msg));
}

Status Status_CompactionTooLarge(SubCode msg) {
    return Status(RsStatus_CompactionTooLarge(msg));
}
Status Status_CompactionTooLarge() {
    return Status(RsStatus_CompactionTooLarge());
}
Status Status_CompactionTooLarge(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_CompactionTooLarge(msg, msg2));
}
Status Status_CompactionTooLarge(const Slice& msg) {
    return Status(RsStatus_CompactionTooLarge(msg));
}

Status Status_ColumnFamilyDropped(SubCode msg) {
    return Status(RsStatus_ColumnFamilyDropped(msg));
}
Status Status_ColumnFamilyDropped() {
    return Status(RsStatus_ColumnFamilyDropped());
}
Status Status_ColumnFamilyDropped(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_ColumnFamilyDropped(msg, msg2));
}
Status Status_ColumnFamilyDropped(const Slice& msg) {
    return Status(RsStatus_ColumnFamilyDropped(msg));
}

Status Status_NoSpace() { return Status(RsStatus_NoSpace()); }
Status Status_NoSpace(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_NoSpace(msg, msg2));
}
Status Status_NoSpace(const Slice& msg) {
    return Status(RsStatus_NoSpace(msg));
}

Status Status_MemoryLimit() { return Status(RsStatus_MemoryLimit()); }
Status Status_MemoryLimit(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_MemoryLimit(msg, msg2));
}
Status Status_MemoryLimit(const Slice& msg) {
    return Status(RsStatus_MemoryLimit(msg));
}

Status Status_SpaceLimit() { return Status(RsStatus_SpaceLimit()); }
Status Status_SpaceLimit(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_SpaceLimit(msg, msg2));
}
Status Status_SpaceLimit(const Slice& msg) {
    return Status(RsStatus_SpaceLimit(msg));
}

Status Status_PathNotFound() { return Status(RsStatus_PathNotFound()); }
Status Status_PathNotFound(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_PathNotFound(msg, msg2));
}
Status Status_PathNotFound(const Slice& msg) {
    return Status(RsStatus_PathNotFound(msg));
}

Status Status_TxnNotPrepared() {
    return Status(RsStatus_TxnNotPrepared());
}
Status Status_TxnNotPrepared(const Slice& msg, const Slice& msg2) {
    return Status(RsStatus_TxnNotPrepared(msg, msg2));
}
Status Status_TxnNotPrepared(const Slice& msg) {
    return Status(RsStatus_TxnNotPrepared(msg));
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
    : rs_status_(RsStatus_new(_code, msg, msg2)) {}

Status::Status(const Status& s)
        : rs_status_(s.rs_status_.Clone()) {}

Status::Status(const Status& s, Severity sev)
        : rs_status_(RsStatus_new(s.rs_status_, sev)) {}

Status& Status::operator=(const Status& s) {
    rs_status_.copy_from(s.rs_status_);
    return *this;
}

Status::Status(Status&& s) noexcept
    : Status(RsStatus_Move(std::move(s.rs_status_))) {}

Status& Status::operator=(Status&& s) noexcept {
    rs_status_.move_from(std::move(s.rs_status_));
    return *this;
}

bool Status::operator==(const Status& rhs) const {
    return rs_status_.eq(rhs.rs_status_);
}

bool Status::eq(const Status& s) const {
    return rs_status_.eq(s.rs_status_);
}

std::unique_ptr<std::string> Status_CopyState(const std::string& s) {
    auto value = RsStatus_CopyState(s);
    return value;
}

Status::Status(Code _code, SubCode _subcode, const Slice& msg,
               const Slice& msg2, Severity sev)
    : rs_status_(RsStatus_new(_code, _subcode, msg, msg2, sev)) {}

Status::Status(Code _code, SubCode _subcode, const Slice& msg, const Slice& msg2)
    : rs_status_(RsStatus_new(_code, _subcode, msg, msg2)) {}

Status Status_CopyAppendMessage(const Status& s, const Slice& delim,
                                 const Slice& msg) {
  // (No attempt at efficiency)
  return Status(RsStatus_CopyAppendMessage(s.rs_status_, delim, msg));
}

std::string Status::ToString() const {
    auto rs_string = rs_status_.ToString();
    return rs_string == nullptr ? "" : rs_string->c_str();
}

}  // namespace ROCKSDB_NAMESPACE
