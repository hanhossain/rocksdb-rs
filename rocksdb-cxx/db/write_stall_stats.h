//  Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <array>

#include "db/internal_stats.h"
#include "rocksdb/types.h"

namespace rocksdb {
extern const std::string& InvalidWriteStallHyphenString();

extern const std::string& WriteStallCauseToHyphenString(rocksdb_rs::types::WriteStallCause cause);

extern const std::string& WriteStallConditionToHyphenString(
    WriteStallCondition condition);

// REQUIRES:
// cause` is CF-scope `WriteStallCause`, see `WriteStallCause` for more
//
// REQUIRES:
// `condition` != `WriteStallCondition::kNormal`
extern InternalStats::InternalCFStatsType InternalCFStat(
    rocksdb_rs::types::WriteStallCause cause, WriteStallCondition condition);

// REQUIRES:
// cause` is DB-scope `WriteStallCause`, see `WriteStallCause` for more
//
// REQUIRES:
// `condition` != `WriteStallCondition::kNormal`
extern InternalStats::InternalDBStatsType InternalDBStat(
    rocksdb_rs::types::WriteStallCause cause, WriteStallCondition condition);

extern bool isCFScopeWriteStallCause(rocksdb_rs::types::WriteStallCause cause);
extern bool isDBScopeWriteStallCause(rocksdb_rs::types::WriteStallCause cause);

constexpr uint32_t kNumCFScopeWriteStallCauses =
    static_cast<uint32_t>(rocksdb_rs::types::WriteStallCause::kCFScopeWriteStallCauseEnumMax) -
    static_cast<uint32_t>(rocksdb_rs::types::WriteStallCause::kMemtableLimit);

constexpr uint32_t kNumDBScopeWriteStallCauses =
    static_cast<uint32_t>(rocksdb_rs::types::WriteStallCause::kDBScopeWriteStallCauseEnumMax) -
    static_cast<uint32_t>(rocksdb_rs::types::WriteStallCause::kWriteBufferManagerLimit);
}  // namespace rocksdb
