//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once


#include <string>
#include <vector>

#include "db/db_impl/db_impl.h"

namespace rocksdb {

// TODO: Share common structure with CompactedDBImpl and DBImplSecondary
class DBImplReadOnly : public DBImpl {
 public:
  DBImplReadOnly(const DBOptions& options, const std::string& dbname);
  // No copying allowed
  DBImplReadOnly(const DBImplReadOnly&) = delete;
  void operator=(const DBImplReadOnly&) = delete;

  virtual ~DBImplReadOnly();

  // Implementations of the DB interface
  using DB::Get;
  virtual rocksdb_rs::status::Status Get(const ReadOptions& options,
                     ColumnFamilyHandle* column_family, const Slice& key,
                     PinnableSlice* value) override;
  rocksdb_rs::status::Status Get(const ReadOptions& options, ColumnFamilyHandle* column_family,
             const Slice& key, PinnableSlice* value,
             std::string* timestamp) override;

  // TODO: Implement ReadOnly MultiGet?

  using DBImpl::NewIterator;
  virtual Iterator* NewIterator(const ReadOptions&,
                                ColumnFamilyHandle* column_family) override;

  virtual rocksdb_rs::status::Status NewIterators(
      const ReadOptions& options,
      const std::vector<ColumnFamilyHandle*>& column_families,
      std::vector<Iterator*>* iterators) override;

  using DBImpl::Put;
  virtual rocksdb_rs::status::Status Put(const WriteOptions& /*options*/,
                     ColumnFamilyHandle* /*column_family*/,
                     const Slice& /*key*/, const Slice& /*value*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }

  using DBImpl::PutEntity;
  rocksdb_rs::status::Status PutEntity(const WriteOptions& /* options */,
                   ColumnFamilyHandle* /* column_family */,
                   const Slice& /* key */,
                   const WideColumns& /* columns */) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }

  using DBImpl::Merge;
  virtual rocksdb_rs::status::Status Merge(const WriteOptions& /*options*/,
                       ColumnFamilyHandle* /*column_family*/,
                       const Slice& /*key*/, const Slice& /*value*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }
  using DBImpl::Delete;
  virtual rocksdb_rs::status::Status Delete(const WriteOptions& /*options*/,
                        ColumnFamilyHandle* /*column_family*/,
                        const Slice& /*key*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }
  using DBImpl::SingleDelete;
  virtual rocksdb_rs::status::Status SingleDelete(const WriteOptions& /*options*/,
                              ColumnFamilyHandle* /*column_family*/,
                              const Slice& /*key*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }
  virtual rocksdb_rs::status::Status Write(const WriteOptions& /*options*/,
                       WriteBatch* /*updates*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }
  using DBImpl::CompactRange;
  virtual rocksdb_rs::status::Status CompactRange(const CompactRangeOptions& /*options*/,
                              ColumnFamilyHandle* /*column_family*/,
                              const Slice* /*begin*/,
                              const Slice* /*end*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }

  using DBImpl::CompactFiles;
  virtual rocksdb_rs::status::Status CompactFiles(
      const CompactionOptions& /*compact_options*/,
      ColumnFamilyHandle* /*column_family*/,
      const std::vector<std::string>& /*input_file_names*/,
      const int /*output_level*/, const int /*output_path_id*/ = -1,
      std::vector<std::string>* const /*output_file_names*/ = nullptr,
      CompactionJobInfo* /*compaction_job_info*/ = nullptr) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }

  virtual rocksdb_rs::status::Status DisableFileDeletions() override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }

  virtual rocksdb_rs::status::Status EnableFileDeletions(bool /*force*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
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
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }

  using DBImpl::SyncWAL;
  virtual rocksdb_rs::status::Status SyncWAL() override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }

  using DB::IngestExternalFile;
  virtual rocksdb_rs::status::Status IngestExternalFile(
      ColumnFamilyHandle* /*column_family*/,
      const std::vector<std::string>& /*external_files*/,
      const IngestExternalFileOptions& /*ingestion_options*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }

  using DB::CreateColumnFamilyWithImport;
  virtual rocksdb_rs::status::Status CreateColumnFamilyWithImport(
      const ColumnFamilyOptions& /*options*/,
      const std::string& /*column_family_name*/,
      const ImportColumnFamilyOptions& /*import_options*/,
      const ExportImportFilesMetaData& /*metadata*/,
      ColumnFamilyHandle** /*handle*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }

  virtual rocksdb_rs::status::Status CreateColumnFamilyWithImport(
      const ColumnFamilyOptions& /*options*/,
      const std::string& /*column_family_name*/,
      const ImportColumnFamilyOptions& /*import_options*/,
      const std::vector<const ExportImportFilesMetaData*>& /*metadatas*/,
      ColumnFamilyHandle** /*handle*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }

  using DB::ClipColumnFamily;
  virtual rocksdb_rs::status::Status ClipColumnFamily(ColumnFamilyHandle* /*column_family*/,
                                  const Slice& /*begin*/,
                                  const Slice& /*end*/) override {
    return rocksdb_rs::status::Status_NotSupported("Not supported operation in read only mode.");
  }

  // FIXME: some missing overrides for more "write" functions

 protected:
  rocksdb_rs::status::Status FlushForGetLiveFiles() override {
    // No-op for read-only DB
    return rocksdb_rs::status::Status_OK();
  }

 private:
  // A "helper" function for DB::OpenForReadOnly without column families
  // to reduce unnecessary I/O
  // It has the same functionality as DB::OpenForReadOnly with column families
  // but does not check the existence of dbname in the file system
  static rocksdb_rs::status::Status OpenForReadOnlyWithoutCheck(
      const DBOptions& db_options, const std::string& dbname,
      const std::vector<ColumnFamilyDescriptor>& column_families,
      std::vector<ColumnFamilyHandle*>* handles, DB** dbptr,
      bool error_if_wal_file_exists = false);
  friend class DB;
};
}  // namespace rocksdb

