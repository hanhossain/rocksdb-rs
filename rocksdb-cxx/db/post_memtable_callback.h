//  Copyright (c) Meta Platforms, Inc. and affiliates. All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include "rocksdb/types.h"

#include "rocksdb-rs/src/status.rs.h"

namespace rocksdb {

// Callback invoked after finishing writing to the memtable but before
// publishing the sequence number to readers.
// Note that with write-prepared/write-unprepared transactions with
// two-write-queues, PreReleaseCallback is called before publishing the
// sequence numbers to readers.
class PostMemTableCallback {
 public:
  virtual ~PostMemTableCallback() {}

  virtual rocksdb_rs::status::Status operator()(SequenceNumber seq, bool disable_memtable) = 0;
};

}  // namespace rocksdb
