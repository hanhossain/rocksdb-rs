//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
#pragma once

#include <cinttypes>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "rocksdb-rs/src/compression_type.rs.h"
#include "rocksdb/advanced_options.h"
#include "rocksdb/env.h"
#include "rocksdb/types.h"

namespace rocksdb::status {
struct Status;
}

namespace rocksdb {

class VersionSet;
class FileSystem;
class SystemClock;
struct ImmutableOptions;
struct MutableCFOptions;
struct FileOptions;
class BlobFileAddition;
class Slice;
class BlobLogWriter;
class IOTracer;
class BlobFileCompletionCallback;

class BlobFileBuilder {
 public:
  BlobFileBuilder(VersionSet* versions, FileSystem* fs,
                  const ImmutableOptions* immutable_options,
                  const MutableCFOptions* mutable_cf_options,
                  const FileOptions* file_options, std::string db_id,
                  std::string db_session_id, int job_id,
                  uint32_t column_family_id,
                  const std::string& column_family_name,
                  Env::IOPriority io_priority,
                  Env::WriteLifeTimeHint write_hint,
                  const std::shared_ptr<IOTracer>& io_tracer,
                  BlobFileCompletionCallback* blob_callback,
                  rocksdb_rs::types::BlobFileCreationReason creation_reason,
                  std::vector<std::string>* blob_file_paths,
                  std::vector<BlobFileAddition>* blob_file_additions);

  BlobFileBuilder(std::function<uint64_t()> file_number_generator,
                  FileSystem* fs, const ImmutableOptions* immutable_options,
                  const MutableCFOptions* mutable_cf_options,
                  const FileOptions* file_options, std::string db_id,
                  std::string db_session_id, int job_id,
                  uint32_t column_family_id,
                  const std::string& column_family_name,
                  Env::IOPriority io_priority,
                  Env::WriteLifeTimeHint write_hint,
                  const std::shared_ptr<IOTracer>& io_tracer,
                  BlobFileCompletionCallback* blob_callback,
                  rocksdb_rs::types::BlobFileCreationReason creation_reason,
                  std::vector<std::string>* blob_file_paths,
                  std::vector<BlobFileAddition>* blob_file_additions);

  BlobFileBuilder(const BlobFileBuilder&) = delete;
  BlobFileBuilder& operator=(const BlobFileBuilder&) = delete;

  ~BlobFileBuilder();

  rocksdb_rs::status::Status Add(const Slice& key, const Slice& value,
                                 std::string* blob_index);
  rocksdb_rs::status::Status Finish();
  void Abandon(const rocksdb_rs::status::Status& s);

 private:
  bool IsBlobFileOpen() const;
  rocksdb_rs::status::Status OpenBlobFileIfNeeded();
  rocksdb_rs::status::Status CompressBlobIfNeeded(
      Slice* blob, std::string* compressed_blob) const;
  rocksdb_rs::status::Status WriteBlobToFile(const Slice& key,
                                             const Slice& blob,
                                             uint64_t* blob_file_number,
                                             uint64_t* blob_offset);
  rocksdb_rs::status::Status CloseBlobFile();
  rocksdb_rs::status::Status CloseBlobFileIfNeeded();

  rocksdb_rs::status::Status PutBlobIntoCacheIfNeeded(
      const Slice& blob, uint64_t blob_file_number, uint64_t blob_offset) const;

  std::function<uint64_t()> file_number_generator_;
  FileSystem* fs_;
  const ImmutableOptions* immutable_options_;
  uint64_t min_blob_size_;
  uint64_t blob_file_size_;
  rocksdb_rs::compression_type::CompressionType blob_compression_type_;
  PrepopulateBlobCache prepopulate_blob_cache_;
  const FileOptions* file_options_;
  const std::string db_id_;
  const std::string db_session_id_;
  int job_id_;
  uint32_t column_family_id_;
  std::string column_family_name_;
  Env::IOPriority io_priority_;
  Env::WriteLifeTimeHint write_hint_;
  std::shared_ptr<IOTracer> io_tracer_;
  BlobFileCompletionCallback* blob_callback_;
  rocksdb_rs::types::BlobFileCreationReason creation_reason_;
  std::vector<std::string>* blob_file_paths_;
  std::vector<BlobFileAddition>* blob_file_additions_;
  std::unique_ptr<BlobLogWriter> writer_;
  uint64_t blob_count_;
  uint64_t blob_bytes_;
};

}  // namespace rocksdb
