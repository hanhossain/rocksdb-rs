// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
#pragma once

#include <memory>
#include <string>

#include "db/dbformat.h"
#include "file/writable_file_writer.h"
#include "options/cf_options.h"
#include "rocksdb/advanced_options.h"

namespace rocksdb {

class SstFileDumper {
 public:
  explicit SstFileDumper(const Options& options, const std::string& file_name,
                         Temperature file_temp, size_t readahead_size,
                         bool verify_checksum, bool output_hex,
                         bool decode_blob_index,
                         const EnvOptions& soptions = EnvOptions(),
                         bool silent = false);

  rocksdb_rs::status::Status ReadSequential(bool print_kv, uint64_t read_num, bool has_from,
                        const std::string& from_key, bool has_to,
                        const std::string& to_key,
                        bool use_from_as_prefix = false);

  rocksdb_rs::status::Status ReadTableProperties(
      std::shared_ptr<const TableProperties>* table_properties);
  uint64_t GetReadNumber() { return read_num_; }
  TableProperties* GetInitTableProperties() { return table_properties_.get(); }

  rocksdb_rs::status::Status VerifyChecksum();
  rocksdb_rs::status::Status DumpTable(const std::string& out_filename);
  rocksdb_rs::status::Status getStatus() { return init_result_.Clone(); }

  rocksdb_rs::status::Status ShowAllCompressionSizes(
      size_t block_size,
      const std::vector<std::pair<rocksdb_rs::compression_type::CompressionType, const char*>>&
          compression_types,
      int32_t compress_level_from, int32_t compress_level_to,
      uint32_t max_dict_bytes, uint32_t zstd_max_train_bytes,
      uint64_t max_dict_buffer_bytes, bool use_zstd_dict_trainer);

  rocksdb_rs::status::Status ShowCompressionSize(size_t block_size, rocksdb_rs::compression_type::CompressionType compress_type,
                             const CompressionOptions& compress_opt);

 private:
  // Get the TableReader implementation for the sst file
  rocksdb_rs::status::Status GetTableReader(const std::string& file_path);
  rocksdb_rs::status::Status ReadTableProperties(uint64_t table_magic_number,
                             RandomAccessFileReader* file, uint64_t file_size,
                             FilePrefetchBuffer* prefetch_buffer);

  rocksdb_rs::status::Status CalculateCompressedTableSize(const TableBuilderOptions& tb_options,
                                      size_t block_size,
                                      uint64_t* num_data_blocks,
                                      uint64_t* compressed_table_size);

  rocksdb_rs::status::Status SetTableOptionsByMagicNumber(uint64_t table_magic_number);
  rocksdb_rs::status::Status SetOldTableOptions();

  // Helper function to call the factory with settings specific to the
  // factory implementation
  rocksdb_rs::status::Status NewTableReader(const ImmutableOptions& ioptions,
                        const EnvOptions& soptions,
                        const InternalKeyComparator& internal_comparator,
                        uint64_t file_size,
                        std::unique_ptr<TableReader>* table_reader);

  std::string file_name_;
  uint64_t read_num_;
  Temperature file_temp_;
  bool output_hex_;
  bool decode_blob_index_;
  EnvOptions soptions_;
  // less verbose in stdout/stderr
  bool silent_;

  // options_ and internal_comparator_ will also be used in
  // ReadSequential internally (specifically, seek-related operations)
  Options options_;

  rocksdb_rs::status::Status init_result_;
  std::unique_ptr<TableReader> table_reader_;
  std::unique_ptr<RandomAccessFileReader> file_;

  const ImmutableOptions ioptions_;
  const MutableCFOptions moptions_;
  ReadOptions read_options_;
  InternalKeyComparator internal_comparator_;
  std::unique_ptr<TableProperties> table_properties_;
};

}  // namespace rocksdb

