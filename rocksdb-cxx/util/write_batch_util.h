//  Copyright (c) Meta Platforms, Inc. and affiliates.
//
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once
#include <unordered_set>
#include <vector>

#include "rocksdb/slice.h"
#include "rocksdb/write_batch.h"

#include "rocksdb-rs/src/status.rs.h"

namespace rocksdb {
// ColumnFamilyCollector is a write batch handler which does nothing
// except recording unique column family IDs
class ColumnFamilyCollector : public WriteBatch::Handler {
  std::unordered_set<uint32_t> column_family_ids_;

  rocksdb_rs::status::Status AddColumnFamilyId(rocksdb_rs::types::ColumnFamilyId column_family_id) {
    column_family_ids_.insert(column_family_id);
    return rocksdb_rs::status::Status_OK();
  }

 public:
  explicit ColumnFamilyCollector() {}

  ~ColumnFamilyCollector() override {}

  rocksdb_rs::status::Status PutCF(rocksdb_rs::types::ColumnFamilyId column_family_id, const Slice&, const Slice&) override {
    return AddColumnFamilyId(column_family_id);
  }

  rocksdb_rs::status::Status DeleteCF(rocksdb_rs::types::ColumnFamilyId column_family_id, const Slice&) override {
    return AddColumnFamilyId(column_family_id);
  }

  rocksdb_rs::status::Status SingleDeleteCF(rocksdb_rs::types::ColumnFamilyId column_family_id, const Slice&) override {
    return AddColumnFamilyId(column_family_id);
  }

  rocksdb_rs::status::Status DeleteRangeCF(rocksdb_rs::types::ColumnFamilyId column_family_id, const Slice&,
                       const Slice&) override {
    return AddColumnFamilyId(column_family_id);
  }

  rocksdb_rs::status::Status MergeCF(rocksdb_rs::types::ColumnFamilyId column_family_id, const Slice&,
                 const Slice&) override {
    return AddColumnFamilyId(column_family_id);
  }

  rocksdb_rs::status::Status PutBlobIndexCF(rocksdb_rs::types::ColumnFamilyId column_family_id, const Slice&,
                        const Slice&) override {
    return AddColumnFamilyId(column_family_id);
  }

  rocksdb_rs::status::Status MarkBeginPrepare(bool) override { return rocksdb_rs::status::Status_OK(); }

  rocksdb_rs::status::Status MarkEndPrepare(const Slice&) override { return rocksdb_rs::status::Status_OK(); }

  rocksdb_rs::status::Status MarkRollback(const Slice&) override { return rocksdb_rs::status::Status_OK(); }

  rocksdb_rs::status::Status MarkCommit(const Slice&) override { return rocksdb_rs::status::Status_OK(); }

  rocksdb_rs::status::Status MarkCommitWithTimestamp(const Slice&, const Slice&) override {
    return rocksdb_rs::status::Status_OK();
  }

  rocksdb_rs::status::Status MarkNoop(bool) override { return rocksdb_rs::status::Status_OK(); }

  const std::unordered_set<uint32_t>& column_families() const {
    return column_family_ids_;
  }
};

rocksdb_rs::status::Status CollectColumnFamilyIdsFromWriteBatch(
    const WriteBatch& batch, std::vector<uint32_t>* column_family_ids);

}  // namespace rocksdb
