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

#include <memory>
#include <string>

#include "rocksdb/slice.h"

#ifndef ROCKSDB_RS
#include "rocksdb-rs-cxx/status.h"
#else
#include "rocksdb-rs/src/status.rs.h"
#endif

namespace ROCKSDB_NAMESPACE {
class Status {
 public:
  // Create a success status.
  Status();

  // Copy the specified status.
  // TODO: Rust
  Status(const Status& s);
  // TODO: Rust
  Status& operator=(const Status& s);
  // TODO: Rust
  Status(Status&& s) noexcept;
  // TODO: Rust
  Status& operator=(Status&& s) noexcept;
  // TODO: Rust
  bool operator==(const Status& rhs) const;
  // TODO: Rust
  bool operator!=(const Status& rhs) const;

  Code code() const;
  SubCode subcode() const;

  // TODO: Rust
  Status(const Status& s, Severity sev);

  Status(Code _code, SubCode _subcode, Severity _sev, const Slice& msg);

  Severity severity() const;

  // Returns a C style string indicating the message of the Status
  const char* getState() const;

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

    explicit Status(Code _code, SubCode _subcode);

    Status(Code _code, SubCode _subcode, const Slice& msg, const Slice& msg2);
    Status(Code _code, const Slice& msg, const Slice& msg2);

  RsStatus rs_status_;

  explicit Status(Code _code);
  explicit Status(Code _code, SubCode _subcode, bool retryable, bool data_loss,
                  unsigned char scope);

  Status(Code _code, SubCode _subcode, const Slice& msg, const Slice& msg2, Severity sev);

    // Only used to migrate Status to rust
    Status(RsStatus _rs_status);
};

    // TODO: Rust
    Status Status_CopyAppendMessage(const Status& s, const Slice& delim, const Slice& msg);

    // Return a success status.
    Status Status_OK();

    // Successful, though an existing something was overwritten
    // Note: using variants of OK status for program logic is discouraged,
    // but it can be useful for communicating statistical information without
    // changing public APIs.
    Status Status_OkOverwritten();

    // Return error status of an appropriate type.
    Status Status_NotFound(const Slice& msg, const Slice& msg2);
    Status Status_NotFound(const Slice& msg);

    // Fast path for not found without malloc;
    Status Status_NotFound(SubCode msg);
    Status Status_NotFound();

    Status Status_NotFound(SubCode sc, const Slice& msg, const Slice& msg2);
    Status Status_NotFound(SubCode sc, const Slice& msg);

    Status Status_Corruption(const Slice& msg, const Slice& msg2 = Slice());
    Status Status_Corruption(SubCode msg);
    Status Status_Corruption();

    Status Status_NotSupported(const Slice& msg, const Slice& msg2);
    Status Status_NotSupported(const Slice& msg);
    Status Status_NotSupported(SubCode msg);
    Status Status_NotSupported();

    Status Status_InvalidArgument(const Slice& msg, const Slice& msg2);
    Status Status_InvalidArgument(const Slice& msg);
    Status Status_InvalidArgument(SubCode msg);
    Status Status_InvalidArgument();

    Status Status_IOError(const Slice& msg, const Slice& msg2);
    Status Status_IOError(const Slice& msg);
    Status Status_IOError(SubCode msg);
    Status Status_IOError();

    Status Status_MergeInProgress(const Slice& msg, const Slice& msg2);
    Status Status_MergeInProgress(const Slice& msg);
    Status Status_MergeInProgress(SubCode msg);
    Status Status_MergeInProgress();

    Status Status_Incomplete(const Slice& msg, const Slice& msg2);
    Status Status_Incomplete(const Slice& msg);
    Status Status_Incomplete(SubCode msg);
    Status Status_Incomplete();

    Status Status_ShutdownInProgress(SubCode msg);
    Status Status_ShutdownInProgress();
    Status Status_ShutdownInProgress(const Slice& msg, const Slice& msg2);
    Status Status_ShutdownInProgress(const Slice& msg);

    Status Status_Aborted(SubCode msg);
    Status Status_Aborted();
    Status Status_Aborted(const Slice& msg, const Slice& msg2);
    Status Status_Aborted(const Slice& msg);

    Status Status_Busy(SubCode msg);
    Status Status_Busy();
    Status Status_Busy(const Slice& msg, const Slice& msg2);
    Status Status_Busy(const Slice& msg);

    Status Status_TimedOut(SubCode msg);
    Status Status_TimedOut();
    Status Status_TimedOut(const Slice& msg, const Slice& msg2);
    Status Status_TimedOut(const Slice& msg);

    Status Status_Expired(SubCode msg);
    Status Status_Expired();
    Status Status_Expired(const Slice& msg, const Slice& msg2);
    Status Status_Expired(const Slice& msg);

    Status Status_TryAgain(SubCode msg);
    Status Status_TryAgain();
    Status Status_TryAgain(const Slice& msg, const Slice& msg2);
    Status Status_TryAgain(const Slice& msg);

    Status Status_CompactionTooLarge(SubCode msg);
    Status Status_CompactionTooLarge();
    Status Status_CompactionTooLarge(const Slice& msg, const Slice& msg2);
    Status Status_CompactionTooLarge(const Slice& msg);

    Status Status_ColumnFamilyDropped(SubCode msg);
    Status Status_ColumnFamilyDropped();
    Status Status_ColumnFamilyDropped(const Slice& msg, const Slice& msg2);
    Status Status_ColumnFamilyDropped(const Slice& msg);

    Status Status_NoSpace();
    Status Status_NoSpace(const Slice& msg, const Slice& msg2);
    Status Status_NoSpace(const Slice& msg);

    Status Status_MemoryLimit();
    Status Status_MemoryLimit(const Slice& msg, const Slice& msg2);
    Status Status_MemoryLimit(const Slice& msg);

    Status Status_SpaceLimit();
    Status Status_SpaceLimit(const Slice& msg, const Slice& msg2);
    Status Status_SpaceLimit(const Slice& msg);

    Status Status_PathNotFound();
    Status Status_PathNotFound(const Slice& msg, const Slice& msg2);
    Status Status_PathNotFound(const Slice& msg);

    Status Status_TxnNotPrepared();
    Status Status_TxnNotPrepared(const Slice& msg, const Slice& msg2);
    Status Status_TxnNotPrepared(const Slice& msg);

    // TODO: Rust
    std::unique_ptr<std::string> Status_CopyState(const std::string& s);
}  // namespace ROCKSDB_NAMESPACE
