//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
#pragma once

#include <memory>
#include <string>
#include <utility>

#include "db/blob/blob_log_format.h"
#include "file/random_access_file_reader.h"
#include "rocksdb/slice.h"

#include "rocksdb-rs/src/status.rs.h"

namespace rocksdb {
namespace blob_db {

class BlobDumpTool {
 public:
  enum class DisplayType {
    kNone,
    kRaw,
    kHex,
    kDetail,
  };

  BlobDumpTool();

  rocksdb_rs::status::Status Run(const std::string& filename, DisplayType show_key,
             DisplayType show_blob, DisplayType show_uncompressed_blob,
             bool show_summary);

 private:
  std::unique_ptr<RandomAccessFileReader> reader_;
  std::unique_ptr<char[]> buffer_;
  size_t buffer_size_;

  rocksdb_rs::status::Status Read(uint64_t offset, size_t size, Slice* result);
  rocksdb_rs::status::Status DumpBlobLogHeader(uint64_t* offset, rocksdb_rs::compression_type::CompressionType* compression);
  rocksdb_rs::status::Status DumpBlobLogFooter(uint64_t file_size, uint64_t* footer_offset);
  rocksdb_rs::status::Status DumpRecord(DisplayType show_key, DisplayType show_blob,
                    DisplayType show_uncompressed_blob, bool show_summary,
                    rocksdb_rs::compression_type::CompressionType compression, uint64_t* offset,
                    uint64_t* total_records, uint64_t* total_key_size,
                    uint64_t* total_blob_size,
                    uint64_t* total_uncompressed_blob_size);
  void DumpSlice(const Slice s, DisplayType type);

  template <class T>
  std::string GetString(std::pair<T, T> p);
};

}  // namespace blob_db
}  // namespace rocksdb

