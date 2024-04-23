// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <stdint.h>

#include "rocksdb-rs/src/types.rs.h"

namespace rocksdb {

// Define all public custom types here.

using ColumnFamilyId = uint32_t;

// Represents a sequence number in a WAL file.
using SequenceNumber = uint64_t;

const SequenceNumber kMinUnCommittedSeq = 1;  // 0 is always committed

enum class WriteStallCondition {
  kDelayed,
  kStopped,
  // Always add new WriteStallCondition before `kNormal`
  kNormal,
};

}  // namespace rocksdb
