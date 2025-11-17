//  Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include "db/write_stall_stats.h"

namespace rocksdb {
const std::string& InvalidWriteStallHyphenString() {
  static const std::string kInvalidWriteStallHyphenString = "invalid";
  return kInvalidWriteStallHyphenString;
}

const std::string& WriteStallCauseToHyphenString(
    rocksdb_rs::types::WriteStallCause cause) {
  static const std::string kMemtableLimit = "memtable-limit";
  static const std::string kL0FileCountLimit = "l0-file-count-limit";
  static const std::string kPendingCompactionBytes = "pending-compaction-bytes";
  static const std::string kWriteBufferManagerLimit =
      "write-buffer-manager-limit";
  switch (cause) {
    case rocksdb_rs::types::WriteStallCause::kMemtableLimit:
      return kMemtableLimit;
    case rocksdb_rs::types::WriteStallCause::kL0FileCountLimit:
      return kL0FileCountLimit;
    case rocksdb_rs::types::WriteStallCause::kPendingCompactionBytes:
      return kPendingCompactionBytes;
    case rocksdb_rs::types::WriteStallCause::kWriteBufferManagerLimit:
      return kWriteBufferManagerLimit;
    default:
      break;
  }
  return InvalidWriteStallHyphenString();
}

const std::string& WriteStallConditionToHyphenString(
    rocksdb_rs::types::WriteStallCondition condition) {
  static const std::string kDelayed = "delays";
  static const std::string kStopped = "stops";
  switch (condition) {
    case rocksdb_rs::types::WriteStallCondition::kDelayed:
      return kDelayed;
    case rocksdb_rs::types::WriteStallCondition::kStopped:
      return kStopped;
    default:
      break;
  }
  return InvalidWriteStallHyphenString();
}

InternalStats::InternalCFStatsType InternalCFStat(
    rocksdb_rs::types::WriteStallCause cause,
    rocksdb_rs::types::WriteStallCondition condition) {
  switch (cause) {
    case rocksdb_rs::types::WriteStallCause::kMemtableLimit: {
      switch (condition) {
        case rocksdb_rs::types::WriteStallCondition::kDelayed:
          return InternalStats::MEMTABLE_LIMIT_DELAYS;
        case rocksdb_rs::types::WriteStallCondition::kStopped:
          return InternalStats::MEMTABLE_LIMIT_STOPS;
        case rocksdb_rs::types::WriteStallCondition::kNormal:
          break;
      }
      break;
    }
    case rocksdb_rs::types::WriteStallCause::kL0FileCountLimit: {
      switch (condition) {
        case rocksdb_rs::types::WriteStallCondition::kDelayed:
          return InternalStats::L0_FILE_COUNT_LIMIT_DELAYS;
        case rocksdb_rs::types::WriteStallCondition::kStopped:
          return InternalStats::L0_FILE_COUNT_LIMIT_STOPS;
        case rocksdb_rs::types::WriteStallCondition::kNormal:
          break;
      }
      break;
    }
    case rocksdb_rs::types::WriteStallCause::kPendingCompactionBytes: {
      switch (condition) {
        case rocksdb_rs::types::WriteStallCondition::kDelayed:
          return InternalStats::PENDING_COMPACTION_BYTES_LIMIT_DELAYS;
        case rocksdb_rs::types::WriteStallCondition::kStopped:
          return InternalStats::PENDING_COMPACTION_BYTES_LIMIT_STOPS;
        case rocksdb_rs::types::WriteStallCondition::kNormal:
          break;
      }
      break;
    }
    default:
      break;
  }
  return InternalStats::INTERNAL_CF_STATS_ENUM_MAX;
}

InternalStats::InternalDBStatsType InternalDBStat(
    rocksdb_rs::types::WriteStallCause cause,
    rocksdb_rs::types::WriteStallCondition condition) {
  switch (cause) {
    case rocksdb_rs::types::WriteStallCause::kWriteBufferManagerLimit: {
      switch (condition) {
        case rocksdb_rs::types::WriteStallCondition::kStopped:
          return InternalStats::kIntStatsWriteBufferManagerLimitStopsCounts;
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
  return InternalStats::kIntStatsNumMax;
}

bool isCFScopeWriteStallCause(rocksdb_rs::types::WriteStallCause cause) {
  uint32_t int_cause = static_cast<uint32_t>(cause);
  uint32_t lower_bound =
      static_cast<uint32_t>(
          rocksdb_rs::types::WriteStallCause::kCFScopeWriteStallCauseEnumMax) -
      kNumCFScopeWriteStallCauses;
  uint32_t upper_bound =
      static_cast<uint32_t>(
          rocksdb_rs::types::WriteStallCause::kCFScopeWriteStallCauseEnumMax) -
      1;
  return lower_bound <= int_cause && int_cause <= upper_bound;
}

bool isDBScopeWriteStallCause(rocksdb_rs::types::WriteStallCause cause) {
  uint32_t int_cause = static_cast<uint32_t>(cause);
  uint32_t lower_bound =
      static_cast<uint32_t>(
          rocksdb_rs::types::WriteStallCause::kDBScopeWriteStallCauseEnumMax) -
      kNumDBScopeWriteStallCauses;
  uint32_t upper_bound =
      static_cast<uint32_t>(
          rocksdb_rs::types::WriteStallCause::kDBScopeWriteStallCauseEnumMax) -
      1;
  return lower_bound <= int_cause && int_cause <= upper_bound;
}

const std::string& WriteStallStatsMapKeys::TotalStops() {
  static const std::string kTotalStops = "total-stops";
  return kTotalStops;
}

const std::string& WriteStallStatsMapKeys::TotalDelays() {
  static const std::string kTotalDelays = "total-delays";
  return kTotalDelays;
}

const std::string&
WriteStallStatsMapKeys::CFL0FileCountLimitDelaysWithOngoingCompaction() {
  static const std::string ret =
      "cf-l0-file-count-limit-delays-with-ongoing-compaction";
  return ret;
}

const std::string&
WriteStallStatsMapKeys::CFL0FileCountLimitStopsWithOngoingCompaction() {
  static const std::string ret =
      "cf-l0-file-count-limit-stops-with-ongoing-compaction";
  return ret;
}

std::string WriteStallStatsMapKeys::CauseConditionCount(
    rocksdb_rs::types::WriteStallCause cause,
    rocksdb_rs::types::WriteStallCondition condition) {
  std::string cause_condition_count_name;

  std::string cause_name;
  if (isCFScopeWriteStallCause(cause) || isDBScopeWriteStallCause(cause)) {
    cause_name = WriteStallCauseToHyphenString(cause);
  } else {
    assert(false);
    return "";
  }

  const std::string& condition_name =
      WriteStallConditionToHyphenString(condition);

  cause_condition_count_name.reserve(cause_name.size() + 1 +
                                     condition_name.size());
  cause_condition_count_name.append(cause_name);
  cause_condition_count_name.append("-");
  cause_condition_count_name.append(condition_name);

  return cause_condition_count_name;
}
}  // namespace rocksdb
