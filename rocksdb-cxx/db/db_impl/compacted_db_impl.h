//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once
#include <string>
#include <vector>

#include "db/db_impl/db_impl.h"

namespace rocksdb {

// TODO: Share common structure with DBImplSecondary and DBImplReadOnly
class CompactedDBImpl : public DBImpl {
 public:
  CompactedDBImpl(const DBOptions& options, const std::string& dbname);
  // No copying allowed
  CompactedDBImpl(const CompactedDBImpl&) = delete;
  void operator=(const CompactedDBImpl&) = delete;

  ~CompactedDBImpl() override;

  static rocksdb_rs::status::Status Open(const Options& options, const std::string& dbname,
                     DB** dbptr);

  // Implementations of the DB interface
  using DB::Get;
  virtual rocksdb_rs::status::Status Get(const ReadOptions& options,
                     ColumnFamilyHandle* column_family, const Slice& key,
                     PinnableSlice* value) override;

  rocksdb_rs::status::Status Get(const ReadOptions& options, ColumnFamilyHandle* column_family,
             const Slice& key, PinnableSlice* value,
             std::string* timestamp) override;

  using DB::MultiGet;
  // Note that CompactedDBImpl::MultiGet is not the optimized version of
  // MultiGet to use.
  // TODO: optimize CompactedDBImpl::MultiGet, see DBImpl::MultiGet for details.
  virtual rust::Vec<rocksdb_rs::status::Status> MultiGet(
      const ReadOptions& options, const std::vector<ColumnFamilyHandle*>&,
      const std::vector<Slice>& keys,
      std::vector<std::string>* values) override;

  rust::Vec<rocksdb_rs::status::Status> MultiGet(const ReadOptions& options,
                               const std::vector<ColumnFamilyHandle*>&,
                               const std::vector<Slice>& keys,
                               std::vector<std::string>* values,
                               std::vector<std::string>* timestamps) override;

  using DBImpl::Put;
  virtual rocksdb_rs::status::Status Put(const WriteOptions& /*options*/,
                     ColumnFamilyHandle* /*column_family*/,
                     const Slice& /*key*/, const Slice& /*value*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }

  using DBImpl::PutEntity;
  rocksdb_rs::status::Status PutEntity(const WriteOptions& /* options */,
                   ColumnFamilyHandle* /* column_family */,
                   const Slice& /* key */,
                   const WideColumns& /* columns */) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }

  using DBImpl::Merge;
  virtual rocksdb_rs::status::Status Merge(const WriteOptions& /*options*/,
                       ColumnFamilyHandle* /*column_family*/,
                       const Slice& /*key*/, const Slice& /*value*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }

  using DBImpl::Delete;
  virtual rocksdb_rs::status::Status Delete(const WriteOptions& /*options*/,
                        ColumnFamilyHandle* /*column_family*/,
                        const Slice& /*key*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }
  virtual rocksdb_rs::status::Status Write(const WriteOptions& /*options*/,
                       WriteBatch* /*updates*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }
  using DBImpl::CompactRange;
  virtual rocksdb_rs::status::Status CompactRange(const CompactRangeOptions& /*options*/,
                              ColumnFamilyHandle* /*column_family*/,
                              const Slice* /*begin*/,
                              const Slice* /*end*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }

  virtual rocksdb_rs::status::Status DisableFileDeletions() override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }
  virtual rocksdb_rs::status::Status EnableFileDeletions(bool /*force*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }
  virtual rocksdb_rs::status::Status GetLiveFiles(std::vector<std::string>& ret,
                              uint64_t* manifest_file_size,
                              bool /*flush_memtable*/) override {
    return DBImpl::GetLiveFiles(ret, manifest_file_size,
                                false /* flush_memtable */);
  }
  using DBImpl::Flush;
  virtual rocksdb_rs::status::Status Flush(const FlushOptions& /*options*/,
                       ColumnFamilyHandle* /*column_family*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }

  virtual rocksdb_rs::status::Status SyncWAL() override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }

  using DB::IngestExternalFile;
  virtual rocksdb_rs::status::Status IngestExternalFile(
      ColumnFamilyHandle* /*column_family*/,
      const std::vector<std::string>& /*external_files*/,
      const IngestExternalFileOptions& /*ingestion_options*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }

  using DB::CreateColumnFamilyWithImport;
  virtual rocksdb_rs::status::Status CreateColumnFamilyWithImport(
      const ColumnFamilyOptions& /*options*/,
      const std::string& /*column_family_name*/,
      const ImportColumnFamilyOptions& /*import_options*/,
      const std::vector<const ExportImportFilesMetaData*>& /*metadatas*/,
      ColumnFamilyHandle** /*handle*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }

  using DB::ClipColumnFamily;
  virtual rocksdb_rs::status::Status ClipColumnFamily(ColumnFamilyHandle* /*column_family*/,
                                  const Slice& /*begin*/,
                                  const Slice& /*end*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported in compacted db mode.");
  }

  // FIXME: some missing overrides for more "write" functions
  // Share with DBImplReadOnly?

 protected:
  rocksdb_rs::status::Status FlushForGetLiveFiles() override {
    // No-op for read-only DB
    return rocksdb_rs::status::Status_OK();
  }

 private:
  friend class DB;
  inline size_t FindFile(const Slice& key);
  rocksdb_rs::status::Status Init(const Options& options);

  ColumnFamilyData* cfd_;
  Version* version_;
  const Comparator* user_comparator_;
  LevelFilesBrief files_;
};
}  // namespace rocksdb
