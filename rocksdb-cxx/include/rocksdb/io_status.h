// Copyright (c) 2019-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// An IOStatus_ encapsulates the result of an operation.
// It may indicate success, or it may indicate an error with an associated error
// message.
//
// Multiple threads can invoke const methods on an
// IOStatus_ without external synchronization, but if any
// of the threads may call a non-const method, all threads accessing the same
// IOStatus_ must use external synchronization.

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

// Return a success status.
static rocksdb_rs::io_status::IOStatus IOStatus_OK() {
  return rocksdb_rs::io_status::IOStatus_OK();
}

static rocksdb_rs::io_status::IOStatus IOStatus_NotSupported(
    const Slice &msg, const Slice &msg2) {
  return rocksdb_rs::io_status::IOStatus_NotSupported(msg, msg2);
}

static rocksdb_rs::io_status::IOStatus IOStatus_NotSupported(const Slice &msg) {
  return rocksdb_rs::io_status::IOStatus_NotSupported(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_NotSupported(
    rocksdb_rs::status::SubCode msg) {
  return rocksdb_rs::io_status::IOStatus_NotSupported(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_NotSupported() {
  return rocksdb_rs::io_status::IOStatus_NotSupported();
}

// Return error status of an appropriate type.
static rocksdb_rs::io_status::IOStatus IOStatus_NotFound(const Slice &msg,
                                                         const Slice &msg2) {
  return rocksdb_rs::io_status::IOStatus_NotFound(msg, msg2);
}

// Return error status of an appropriate type.
static rocksdb_rs::io_status::IOStatus IOStatus_NotFound(const Slice &msg) {
  return rocksdb_rs::io_status::IOStatus_NotFound(msg);
}

// Fast path for not found without malloc;
static rocksdb_rs::io_status::IOStatus IOStatus_NotFound(
    rocksdb_rs::status::SubCode msg) {
  return rocksdb_rs::io_status::IOStatus_NotFound(msg);
}

// Fast path for not found without malloc;
static rocksdb_rs::io_status::IOStatus IOStatus_NotFound() {
  return rocksdb_rs::io_status::IOStatus_NotFound();
}

static rocksdb_rs::io_status::IOStatus IOStatus_Corruption(const Slice &msg,
                                                           const Slice &msg2) {
  return rocksdb_rs::io_status::IOStatus_Corruption(msg, msg2);
}

static rocksdb_rs::io_status::IOStatus IOStatus_Corruption(const Slice &msg) {
  return rocksdb_rs::io_status::IOStatus_Corruption(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_Corruption(
    rocksdb_rs::status::SubCode msg) {
  return rocksdb_rs::io_status::IOStatus_Corruption(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_Corruption() {
  return rocksdb_rs::io_status::IOStatus_Corruption();
}

static rocksdb_rs::io_status::IOStatus IOStatus_InvalidArgument(
    const Slice &msg, const Slice &msg2) {
  return rocksdb_rs::io_status::IOStatus_InvalidArgument(msg, msg2);
}

static rocksdb_rs::io_status::IOStatus IOStatus_InvalidArgument(
    const Slice &msg) {
  return rocksdb_rs::io_status::IOStatus_InvalidArgument(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_InvalidArgument(
    rocksdb_rs::status::SubCode msg) {
  return rocksdb_rs::io_status::IOStatus_InvalidArgument(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_InvalidArgument() {
  return rocksdb_rs::io_status::IOStatus_InvalidArgument();
}

static rocksdb_rs::io_status::IOStatus IOStatus_IOError(const Slice &msg,
                                                        const Slice &msg2) {
  return rocksdb_rs::io_status::IOStatus_IOError(msg, msg2);
}

static rocksdb_rs::io_status::IOStatus IOStatus_IOError(const Slice &msg) {
  return rocksdb_rs::io_status::IOStatus_IOError(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_IOError(
    rocksdb_rs::status::SubCode msg) {
  return rocksdb_rs::io_status::IOStatus_IOError(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_IOError() {
  return rocksdb_rs::io_status::IOStatus_IOError();
}

static rocksdb_rs::io_status::IOStatus IOStatus_Busy(
    rocksdb_rs::status::SubCode msg) {
  return rocksdb_rs::io_status::IOStatus_Busy(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_Busy() {
  return rocksdb_rs::io_status::IOStatus_Busy();
}

static rocksdb_rs::io_status::IOStatus IOStatus_Busy(const Slice &msg,
                                                     const Slice &msg2) {
  return rocksdb_rs::io_status::IOStatus_Busy(msg, msg2);
}

static rocksdb_rs::io_status::IOStatus IOStatus_Busy(const Slice &msg) {
  return rocksdb_rs::io_status::IOStatus_Busy(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_TimedOut(
    rocksdb_rs::status::SubCode msg) {
  return rocksdb_rs::io_status::IOStatus_TimedOut(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_TimedOut() {
  return rocksdb_rs::io_status::IOStatus_TimedOut();
}

static rocksdb_rs::io_status::IOStatus IOStatus_TimedOut(const Slice &msg,
                                                         const Slice &msg2) {
  return rocksdb_rs::io_status::IOStatus_TimedOut(msg, msg2);
}

static rocksdb_rs::io_status::IOStatus IOStatus_TimedOut(const Slice &msg) {
  return rocksdb_rs::io_status::IOStatus_TimedOut(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_NoSpace() {
  return rocksdb_rs::io_status::IOStatus_NoSpace();
}

static rocksdb_rs::io_status::IOStatus IOStatus_NoSpace(const Slice &msg,
                                                        const Slice &msg2) {
  return rocksdb_rs::io_status::IOStatus_NoSpace(msg, msg2);
}

static rocksdb_rs::io_status::IOStatus IOStatus_NoSpace(const Slice &msg) {
  return rocksdb_rs::io_status::IOStatus_NoSpace(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_PathNotFound() {
  return rocksdb_rs::io_status::IOStatus_PathNotFound();
}

static rocksdb_rs::io_status::IOStatus IOStatus_PathNotFound(
    const Slice &msg, const Slice &msg2) {
  return rocksdb_rs::io_status::IOStatus_PathNotFound(msg, msg2);
}

static rocksdb_rs::io_status::IOStatus IOStatus_PathNotFound(const Slice &msg) {
  return rocksdb_rs::io_status::IOStatus_PathNotFound(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_IOFenced() {
  return rocksdb_rs::io_status::IOStatus_IOFenced();
}

static rocksdb_rs::io_status::IOStatus IOStatus_IOFenced(const Slice &msg,
                                                         const Slice &msg2) {
  return rocksdb_rs::io_status::IOStatus_IOFenced(msg, msg2);
}

static rocksdb_rs::io_status::IOStatus IOStatus_IOFenced(const Slice &msg) {
  return rocksdb_rs::io_status::IOStatus_IOFenced(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_Aborted(
    rocksdb_rs::status::SubCode msg) {
  return rocksdb_rs::io_status::IOStatus_Aborted(msg);
}

static rocksdb_rs::io_status::IOStatus IOStatus_Aborted() {
  return rocksdb_rs::io_status::IOStatus_Aborted();
}

static rocksdb_rs::io_status::IOStatus IOStatus_Aborted(const Slice &msg,
                                                        const Slice &msg2) {
  return rocksdb_rs::io_status::IOStatus_Aborted(msg, msg2);
}

static rocksdb_rs::io_status::IOStatus IOStatus_Aborted(const Slice &msg) {
  return rocksdb_rs::io_status::IOStatus_Aborted(msg);
}
}  // namespace rocksdb
