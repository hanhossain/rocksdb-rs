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
enum class Code : unsigned char;
enum class SubCode : unsigned char;
enum class Severity : unsigned char;

class Status {
 public:
  // Create a success status.
  Status();
  ~Status();

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
  void PermitUncheckedError() const;

  void MustCheck() const;

  Code code() const;

  SubCode subcode() const;

  Status(const Status& s, Severity sev);

  Status(Code _code, SubCode _subcode, Severity _sev, const Slice& msg);

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
  static Status NotFound(SubCode msg);
  static Status NotFound();

  static Status NotFound(SubCode sc, const Slice& msg, const Slice& msg2 = Slice());

  static Status Corruption(const Slice& msg, const Slice& msg2 = Slice());
  static Status Corruption(SubCode msg);
  static Status Corruption();

  static Status NotSupported(const Slice& msg, const Slice& msg2 = Slice());
  static Status NotSupported(SubCode msg);
  static Status NotSupported();

  static Status InvalidArgument(const Slice& msg, const Slice& msg2 = Slice());
  static Status InvalidArgument(SubCode msg);
  static Status InvalidArgument();

  static Status IOError(const Slice& msg, const Slice& msg2 = Slice());
  static Status IOError(SubCode msg);
  static Status IOError();

  static Status MergeInProgress(const Slice& msg, const Slice& msg2 = Slice());
  static Status MergeInProgress(SubCode msg);
  static Status MergeInProgress();

  static Status Incomplete(const Slice& msg, const Slice& msg2 = Slice());
  static Status Incomplete(SubCode msg);
  static Status Incomplete();

  static Status ShutdownInProgress(SubCode msg);
  static Status ShutdownInProgress();
  static Status ShutdownInProgress(const Slice& msg, const Slice& msg2 = Slice());
  static Status Aborted(SubCode msg);
  static Status Aborted();
  static Status Aborted(const Slice& msg, const Slice& msg2 = Slice());

  static Status Busy(SubCode msg);
  static Status Busy();
  static Status Busy(const Slice& msg, const Slice& msg2 = Slice());

  static Status TimedOut(SubCode msg);
  static Status TimedOut();
  static Status TimedOut(const Slice& msg, const Slice& msg2 = Slice());

  static Status Expired(SubCode msg);
  static Status Expired();
  static Status Expired(const Slice& msg, const Slice& msg2 = Slice());

  static Status TryAgain(SubCode msg);
  static Status TryAgain();
  static Status TryAgain(const Slice& msg, const Slice& msg2 = Slice());

  static Status CompactionTooLarge(SubCode msg);
  static Status CompactionTooLarge();
  static Status CompactionTooLarge(const Slice& msg,
                                   const Slice& msg2 = Slice());

  static Status ColumnFamilyDropped(SubCode msg);
  static Status ColumnFamilyDropped();
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

  explicit Status(Code _code);
  explicit Status(Code _code, SubCode _subcode, bool retryable, bool data_loss,
                  unsigned char scope);

  Status(Code _code, SubCode _subcode, const Slice& msg, const Slice& msg2, Severity sev);

        Status(Code _code, const Slice& msg, const Slice& msg2);

  void MarkChecked() const;
};

    Status Status_TxnNotPrepared();
    Status Status_TxnNotPrepared(const Slice& msg, const Slice& msg2 = Slice());
    std::unique_ptr<const char[]> Status_CopyState(const char* s);


}  // namespace ROCKSDB_NAMESPACE
