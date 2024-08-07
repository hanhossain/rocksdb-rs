//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include "rocksdb/table.h"
#include "util/gflags_compat.h"
#include "util/random.h"

DECLARE_int32(mark_for_compaction_one_file_in);

namespace rocksdb {

// A `DbStressTablePropertiesCollector` ignores what keys/values were added to
// the table, adds no properties to the table, and decides at random whether the
// table will be marked for compaction according to
// `FLAGS_mark_for_compaction_one_file_in`.
class DbStressTablePropertiesCollector : public TablePropertiesCollector {
 public:
  DbStressTablePropertiesCollector()
      : need_compact_(Random::GetTLSInstance()->OneInOpt(
            FLAGS_mark_for_compaction_one_file_in)) {}

  virtual rocksdb_rs::status::Status AddUserKey(const Slice& /* key */, const Slice& /* value */,
                            rocksdb_rs::types::EntryType /*type*/, SequenceNumber /*seq*/,
                            uint64_t /*file_size*/) override {
    return rocksdb_rs::status::Status_OK();
  }

  virtual rocksdb_rs::status::Status Finish(UserCollectedProperties* /* properties */) override {
    return rocksdb_rs::status::Status_OK();
  }

  virtual UserCollectedProperties GetReadableProperties() const override {
    return UserCollectedProperties{};
  }

  virtual const char* Name() const override {
    return "DbStressTablePropertiesCollector";
  }

  virtual bool NeedCompact() const override { return need_compact_; }

 private:
  const bool need_compact_;
};

// A `DbStressTablePropertiesCollectorFactory` creates
// `DbStressTablePropertiesCollectorFactory`s.
class DbStressTablePropertiesCollectorFactory
    : public TablePropertiesCollectorFactory {
 public:
  virtual TablePropertiesCollector* CreateTablePropertiesCollector(
      TablePropertiesCollectorFactory::Context /* context */) override {
    return new DbStressTablePropertiesCollector();
  }

  virtual const char* Name() const override {
    return "DbStressTablePropertiesCollectorFactory";
  }
};

}  // namespace rocksdb
