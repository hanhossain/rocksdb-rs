//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <cinttypes>
#include <memory>

#include "db/blob/blob_read_request.h"
#include "file/random_access_file_reader.h"
#include "util/autovector.h"

#include "rocksdb-rs/src/compression_type.rs.h"

namespace rocksdb_rs::status {
    struct Status;
}

namespace rocksdb {

struct ImmutableOptions;
struct FileOptions;
class HistogramImpl;
struct ReadOptions;
class Slice;
class FilePrefetchBuffer;
class BlobContents;
class Statistics;

class BlobFileReader {
 public:
  static rocksdb_rs::status::Status Create(const ImmutableOptions& immutable_options,
                       const ReadOptions& read_options,
                       const FileOptions& file_options,
                       uint32_t column_family_id,
                       HistogramImpl* blob_file_read_hist,
                       uint64_t blob_file_number,
                       const std::shared_ptr<IOTracer>& io_tracer,
                       std::unique_ptr<BlobFileReader>* reader);

  BlobFileReader(const BlobFileReader&) = delete;
  BlobFileReader& operator=(const BlobFileReader&) = delete;

  ~BlobFileReader();

  rocksdb_rs::status::Status GetBlob(const ReadOptions& read_options, const Slice& user_key,
                 uint64_t offset, uint64_t value_size,
                 rocksdb_rs::compression_type::CompressionType compression_type,
                 FilePrefetchBuffer* prefetch_buffer,
                 MemoryAllocator* allocator,
                 std::unique_ptr<BlobContents>* result,
                 uint64_t* bytes_read) const;

  // offsets must be sorted in ascending order by caller.
  void MultiGetBlob(
      const ReadOptions& read_options, MemoryAllocator* allocator,
      autovector<std::pair<BlobReadRequest*, std::unique_ptr<BlobContents>>>&
          blob_reqs,
      uint64_t* bytes_read) const;

  rocksdb_rs::compression_type::CompressionType GetCompressionType() const { return compression_type_; }

  uint64_t GetFileSize() const { return file_size_; }

 private:
  BlobFileReader(std::unique_ptr<RandomAccessFileReader>&& file_reader,
                 uint64_t file_size, rocksdb_rs::compression_type::CompressionType compression_type,
                 SystemClock* clock, Statistics* statistics);

  static rocksdb_rs::status::Status OpenFile(const ImmutableOptions& immutable_options,
                         const FileOptions& file_opts,
                         HistogramImpl* blob_file_read_hist,
                         uint64_t blob_file_number,
                         const std::shared_ptr<IOTracer>& io_tracer,
                         uint64_t* file_size,
                         std::unique_ptr<RandomAccessFileReader>* file_reader);

  static rocksdb_rs::status::Status ReadHeader(const RandomAccessFileReader* file_reader,
                           const ReadOptions& read_options,
                           uint32_t column_family_id, Statistics* statistics,
                           rocksdb_rs::compression_type::CompressionType* compression_type);

  static rocksdb_rs::status::Status ReadFooter(const RandomAccessFileReader* file_reader,
                           const ReadOptions& read_options, uint64_t file_size,
                           Statistics* statistics);

  using Buffer = std::unique_ptr<char[]>;

  static rocksdb_rs::status::Status ReadFromFile(const RandomAccessFileReader* file_reader,
                             const ReadOptions& read_options,
                             uint64_t read_offset, size_t read_size,
                             Statistics* statistics, Slice* slice, Buffer* buf,
                             AlignedBuf* aligned_buf,
                             Env::IOPriority rate_limiter_priority);

  static rocksdb_rs::status::Status VerifyBlob(const Slice& record_slice, const Slice& user_key,
                           uint64_t value_size);

  static rocksdb_rs::status::Status UncompressBlobIfNeeded(const Slice& value_slice,
                                       rocksdb_rs::compression_type::CompressionType compression_type,
                                       MemoryAllocator* allocator,
                                       SystemClock* clock,
                                       Statistics* statistics,
                                       std::unique_ptr<BlobContents>* result);

  std::unique_ptr<RandomAccessFileReader> file_reader_;
  uint64_t file_size_;
  rocksdb_rs::compression_type::CompressionType compression_type_;
  SystemClock* clock_;
  Statistics* statistics_;
};

}  // namespace rocksdb
