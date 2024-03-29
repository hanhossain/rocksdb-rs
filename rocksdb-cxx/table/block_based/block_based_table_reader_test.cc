//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include "table/block_based/block_based_table_reader.h"

#include <cmath>
#include <memory>
#include <string>

#include "cache/cache_reservation_manager.h"
#include "db/db_test_util.h"
#include "db/table_properties_collector.h"
#include "file/file_util.h"
#include "options/options_helper.h"
#include "port/port.h"
#include "port/stack_trace.h"
#include "rocksdb/db.h"
#include "rocksdb/file_system.h"
#include "table/block_based/block_based_table_builder.h"
#include "table/block_based/block_based_table_factory.h"
#include "table/block_based/partitioned_index_iterator.h"
#include "table/format.h"
#include "test_util/testharness.h"
#include "test_util/testutil.h"
#include "util/random.h"

#include "rocksdb-rs/src/compression_type.rs.h"

namespace rocksdb {

class BlockBasedTableReaderBaseTest : public testing::Test {
 protected:
  // Prepare key-value pairs to occupy multiple blocks.
  // Each value is 256B, every 16 pairs constitute 1 block.
  // If mixed_with_human_readable_string_value == true,
  // then adjacent blocks contain values with different compression
  // complexity: human readable strings are easier to compress than random
  // strings.
  static std::map<std::string, std::string> GenerateKVMap(
      int num_block = 100, bool mixed_with_human_readable_string_value = false,
      size_t ts_sz = 0) {
    std::map<std::string, std::string> kv;

    Random rnd(101);
    uint32_t key = 0;
    for (int block = 0; block < num_block; block++) {
      for (int i = 0; i < 16; i++) {
        char k[9] = {0};
        // Internal key is constructed directly from this key,
        // and internal key size is required to be >= 8 bytes,
        // so use %08u as the format string.
        snprintf(k, sizeof(k), "%08u", key);
        std::string v;
        if (mixed_with_human_readable_string_value) {
          v = (block % 2) ? rnd.HumanReadableString(256)
                          : rnd.RandomString(256);
        } else {
          v = rnd.RandomString(256);
        }
        if (ts_sz > 0) {
          std::string user_key;
          AppendKeyWithMinTimestamp(&user_key, std::string(k), ts_sz);
          kv[user_key] = v;
        } else {
          kv[std::string(k)] = v;
        }
        key++;
      }
    }
    return kv;
  }

  void SetUp() override {
    SetupSyncPointsToMockDirectIO();
    test_dir_ = test::PerThreadDBPath("block_based_table_reader_test");
    env_ = Env::Default();
    fs_ = FileSystem::Default();
    ASSERT_OK(fs_->CreateDir(test_dir_, IOOptions(), nullptr));
    ConfigureTableFactory();
  }

  virtual void ConfigureTableFactory() = 0;

  void TearDown() override { EXPECT_OK(DestroyDir(env_, test_dir_)); }

  // Creates a table with the specificied key value pairs (kv).
  void CreateTable(const std::string& table_name,
                   const ImmutableOptions& ioptions,
                   const rocksdb_rs::compression_type::CompressionType& compression_type,
                   const std::map<std::string, std::string>& kv,
                   uint32_t compression_parallel_threads = 1,
                   uint32_t compression_dict_bytes = 0) {
    std::unique_ptr<WritableFileWriter> writer;
    NewFileWriter(table_name, &writer);

    InternalKeyComparator comparator(ioptions.user_comparator);
    ColumnFamilyOptions cf_options;
    cf_options.prefix_extractor = options_.prefix_extractor;
    MutableCFOptions moptions(cf_options);
    CompressionOptions compression_opts;
    compression_opts.parallel_threads = compression_parallel_threads;
    // Enable compression dictionary and set a buffering limit that is the same
    // as each block's size.
    compression_opts.max_dict_bytes = compression_dict_bytes;
    compression_opts.max_dict_buffer_bytes = compression_dict_bytes;
    IntTblPropCollectorFactories factories;
    std::unique_ptr<TableBuilder> table_builder(
        options_.table_factory->NewTableBuilder(
            TableBuilderOptions(ioptions, moptions, comparator, &factories,
                                compression_type, compression_opts,
                                0 /* column_family_id */,
                                kDefaultColumnFamilyName, -1 /* level */),
            writer.get()));

    // Build table.
    for (auto it = kv.begin(); it != kv.end(); it++) {
      std::string k = ToInternalKey(it->first);
      std::string v = it->second;
      table_builder->Add(k, v);
    }
    ASSERT_OK(table_builder->Finish());
  }

  void NewBlockBasedTableReader(const FileOptions& foptions,
                                const ImmutableOptions& ioptions,
                                const InternalKeyComparator& comparator,
                                const std::string& table_name,
                                std::unique_ptr<BlockBasedTable>* table,
                                bool prefetch_index_and_filter_in_cache = true,
                                rocksdb_rs::status::Status* status = nullptr,
                                bool user_defined_timestamps_persisted = true) {
    const MutableCFOptions moptions(options_);
    TableReaderOptions table_reader_options = TableReaderOptions(
        ioptions, moptions.prefix_extractor, EnvOptions(), comparator,
        0 /* block_protection_bytes_per_key */, false /* _skip_filters */,
        false /* _immortal */, false /* _force_direct_prefetch */,
        -1 /* _level */, nullptr /* _block_cache_tracer */,
        0 /* _max_file_size_for_l0_meta_pin */, "" /* _cur_db_session_id */,
        0 /* _cur_file_num */, {} /* _unique_id */, 0 /* _largest_seqno */,
        0 /* _tail_size */, user_defined_timestamps_persisted);

    std::unique_ptr<RandomAccessFileReader> file;
    NewFileReader(table_name, foptions, &file);

    uint64_t file_size = 0;
    ASSERT_OK(env_->GetFileSize(Path(table_name), &file_size));

    ReadOptions read_opts;
    read_opts.verify_checksums = true;
    std::unique_ptr<TableReader> general_table;
    rocksdb_rs::status::Status s = options_.table_factory->NewTableReader(
        read_opts, table_reader_options, std::move(file), file_size,
        &general_table, prefetch_index_and_filter_in_cache);

    if (s.ok()) {
      table->reset(reinterpret_cast<BlockBasedTable*>(general_table.release()));
    }

    if (status) {
      status->copy_from(s);
    }
  }

  std::string Path(const std::string& fname) { return test_dir_ + "/" + fname; }

  std::string test_dir_;
  Env* env_;
  std::shared_ptr<FileSystem> fs_;
  Options options_;

  std::string ToInternalKey(const std::string& key) {
    InternalKey internal_key(key, 0, ValueType::kTypeValue);
    return internal_key.Encode().ToString();
  }

 private:
  void WriteToFile(const std::string& content, const std::string& filename) {
    std::unique_ptr<FSWritableFile> f;
    ASSERT_OK(fs_->NewWritableFile(Path(filename), FileOptions(), &f, nullptr));
    ASSERT_OK(f->Append(content, IOOptions(), nullptr));
    ASSERT_OK(f->Close(IOOptions(), nullptr));
  }

  void NewFileWriter(const std::string& filename,
                     std::unique_ptr<WritableFileWriter>* writer) {
    std::string path = Path(filename);
    EnvOptions env_options;
    FileOptions foptions;
    std::unique_ptr<FSWritableFile> file;
    ASSERT_OK(fs_->NewWritableFile(path, foptions, &file, nullptr));
    writer->reset(new WritableFileWriter(std::move(file), path, env_options));
  }

  void NewFileReader(const std::string& filename, const FileOptions& opt,
                     std::unique_ptr<RandomAccessFileReader>* reader) {
    std::string path = Path(filename);
    std::unique_ptr<FSRandomAccessFile> f;
    ASSERT_OK(fs_->NewRandomAccessFile(path, opt, &f, nullptr));
    reader->reset(new RandomAccessFileReader(std::move(f), path,
                                             env_->GetSystemClock().get()));
  }
};

// Param 1: compression type
// Param 2: whether to use direct reads
// Param 3: Block Based Table Index type
// Param 4: BBTO no_block_cache option
// Param 5: test mode for the user-defined timestamp feature
// Param 6: number of parallel compression threads
// Param 7: CompressionOptions.max_dict_bytes and
//          CompressionOptions.max_dict_buffer_bytes to enable/disable
//          compression dictionary.
class BlockBasedTableReaderTest
    : public BlockBasedTableReaderBaseTest,
      public testing::WithParamInterface<std::tuple<
          rocksdb_rs::compression_type::CompressionType, bool, BlockBasedTableOptions::IndexType, bool,
          test::UserDefinedTimestampTestMode, uint32_t, uint32_t>> {
 protected:
  void SetUp() override {
    compression_type_ = std::get<0>(GetParam());
    use_direct_reads_ = std::get<1>(GetParam());
    test::UserDefinedTimestampTestMode udt_test_mode = std::get<4>(GetParam());
    udt_enabled_ = test::IsUDTEnabled(udt_test_mode);
    persist_udt_ = test::ShouldPersistUDT(udt_test_mode);
    compression_parallel_threads_ = std::get<5>(GetParam());
    compression_dict_bytes_ = std::get<6>(GetParam());
    BlockBasedTableReaderBaseTest::SetUp();
  }

  void ConfigureTableFactory() override {
    BlockBasedTableOptions opts;
    opts.index_type = std::get<2>(GetParam());
    opts.no_block_cache = std::get<3>(GetParam());
    opts.filter_policy.reset(NewBloomFilterPolicy(10, false));
    opts.partition_filters =
        opts.index_type ==
        BlockBasedTableOptions::IndexType::kTwoLevelIndexSearch;
    options_.table_factory.reset(
        static_cast<BlockBasedTableFactory*>(NewBlockBasedTableFactory(opts)));
    options_.prefix_extractor =
        std::shared_ptr<const SliceTransform>(NewFixedPrefixTransform(3));
  }

  rocksdb_rs::compression_type::CompressionType compression_type_;
  bool use_direct_reads_;
  bool udt_enabled_;
  bool persist_udt_;
  uint32_t compression_parallel_threads_;
  uint32_t compression_dict_bytes_;
};

// Tests MultiGet in both direct IO and non-direct IO mode.
// The keys should be in cache after MultiGet.
TEST_P(BlockBasedTableReaderTest, MultiGet) {
  Options options;
  ReadOptions read_opts;
  std::string dummy_ts(sizeof(uint64_t), '\0');
  Slice read_timestamp = dummy_ts;
  if (udt_enabled_) {
    options.comparator = test::BytewiseComparatorWithU64TsWrapper();
    read_opts.timestamp = &read_timestamp;
  }
  options.persist_user_defined_timestamps = persist_udt_;
  size_t ts_sz = options.comparator->timestamp_size();
  std::map<std::string, std::string> kv =
      BlockBasedTableReaderBaseTest::GenerateKVMap(
          100 /* num_block */,
          true /* mixed_with_human_readable_string_value */, ts_sz);

  // Prepare keys, values, and statuses for MultiGet.
  autovector<Slice, MultiGetContext::MAX_BATCH_SIZE> keys;
  autovector<Slice, MultiGetContext::MAX_BATCH_SIZE> keys_without_timestamps;
  autovector<PinnableSlice, MultiGetContext::MAX_BATCH_SIZE> values;
  std::vector<rocksdb_rs::status::Status> statuses;
  {
    const int step =
        static_cast<int>(kv.size()) / MultiGetContext::MAX_BATCH_SIZE;
    auto it = kv.begin();
    for (int i = 0; i < MultiGetContext::MAX_BATCH_SIZE; i++) {
      keys.emplace_back(it->first);
      if (ts_sz > 0) {
        Slice ukey_without_ts = StripTimestampFromUserKey(it->first, ts_sz);
        keys_without_timestamps.push_back(ukey_without_ts);
      } else {
        keys_without_timestamps.emplace_back(it->first);
      }
      values.emplace_back();
      statuses.push_back(rocksdb_rs::status::Status_new());
      std::advance(it, step);
    }
  }

  std::string table_name = "BlockBasedTableReaderTest_MultiGet" +
                           CompressionTypeToString(compression_type_);

  ImmutableOptions ioptions(options);
  CreateTable(table_name, ioptions, compression_type_, kv,
              compression_parallel_threads_, compression_dict_bytes_);

  std::unique_ptr<BlockBasedTable> table;
  FileOptions foptions;
  foptions.use_direct_reads = use_direct_reads_;
  InternalKeyComparator comparator(options.comparator);
  NewBlockBasedTableReader(foptions, ioptions, comparator, table_name, &table,
                           true /* bool prefetch_index_and_filter_in_cache */,
                           nullptr /* status */, persist_udt_);

  ASSERT_OK(
      table->VerifyChecksum(read_opts, TableReaderCaller::kUserVerifyChecksum));

  // Ensure that keys are not in cache before MultiGet.
  for (auto& key : keys) {
    std::string ikey = ToInternalKey(key.ToString());
    ASSERT_FALSE(table->TEST_KeyInCache(read_opts, ikey));
  }

  // Prepare MultiGetContext.
  autovector<GetContext, MultiGetContext::MAX_BATCH_SIZE> get_context;
  autovector<KeyContext, MultiGetContext::MAX_BATCH_SIZE> key_context;
  autovector<KeyContext*, MultiGetContext::MAX_BATCH_SIZE> sorted_keys;
  for (size_t i = 0; i < keys.size(); ++i) {
    get_context.emplace_back(options.comparator, nullptr, nullptr, nullptr,
                             GetContext::kNotFound, keys[i], &values[i],
                             nullptr, nullptr, nullptr, nullptr,
                             true /* do_merge */, nullptr, nullptr, nullptr,
                             nullptr, nullptr, nullptr);
    key_context.emplace_back(nullptr, keys_without_timestamps[i], &values[i],
                             nullptr, nullptr, &statuses.back());
    key_context.back().get_context = &get_context.back();
  }
  for (auto& key_ctx : key_context) {
    sorted_keys.emplace_back(&key_ctx);
  }
  MultiGetContext ctx(&sorted_keys, 0, sorted_keys.size(), 0, read_opts,
                      fs_.get(), nullptr);

  // Execute MultiGet.
  MultiGetContext::Range range = ctx.GetMultiGetRange();
  PerfContext* perf_ctx = get_perf_context();
  perf_ctx->Reset();
  table->MultiGet(read_opts, &range, nullptr);

  ASSERT_GE(perf_ctx->block_read_count - perf_ctx->index_block_read_count -
                perf_ctx->filter_block_read_count -
                perf_ctx->compression_dict_block_read_count,
            1);
  ASSERT_GE(perf_ctx->block_read_byte, 1);

  for (const rocksdb_rs::status::Status& status : statuses) {
    ASSERT_OK(status);
  }
  // Check that keys are in cache after MultiGet.
  for (size_t i = 0; i < keys.size(); i++) {
    std::string ikey = ToInternalKey(keys[i].ToString());
    ASSERT_TRUE(table->TEST_KeyInCache(read_opts, ikey));
    ASSERT_EQ(values[i].ToString(), kv[keys[i].ToString()]);
  }
}

TEST_P(BlockBasedTableReaderTest, NewIterator) {
  Options options;
  ReadOptions read_opts;
  std::string dummy_ts(sizeof(uint64_t), '\0');
  Slice read_timestamp = dummy_ts;
  if (udt_enabled_) {
    options.comparator = test::BytewiseComparatorWithU64TsWrapper();
    read_opts.timestamp = &read_timestamp;
  }
  options.persist_user_defined_timestamps = persist_udt_;
  size_t ts_sz = options.comparator->timestamp_size();
  std::map<std::string, std::string> kv =
      BlockBasedTableReaderBaseTest::GenerateKVMap(
          100 /* num_block */,
          true /* mixed_with_human_readable_string_value */, ts_sz);

  std::string table_name = "BlockBasedTableReaderTest_NewIterator" +
                           CompressionTypeToString(compression_type_);

  ImmutableOptions ioptions(options);
  CreateTable(table_name, ioptions, compression_type_, kv,
              compression_parallel_threads_, compression_dict_bytes_);

  std::unique_ptr<BlockBasedTable> table;
  FileOptions foptions;
  foptions.use_direct_reads = use_direct_reads_;
  InternalKeyComparator comparator(options.comparator);
  NewBlockBasedTableReader(foptions, ioptions, comparator, table_name, &table,
                           true /* bool prefetch_index_and_filter_in_cache */,
                           nullptr /* status */, persist_udt_);
  ASSERT_OK(
      table->VerifyChecksum(read_opts, TableReaderCaller::kUserVerifyChecksum));

  std::unique_ptr<InternalIterator> iter;
  iter.reset(table->NewIterator(
      read_opts, options_.prefix_extractor.get(), /*arena=*/nullptr,
      /*skip_filters=*/false, TableReaderCaller::kUncategorized));

  // Test forward scan.
  ASSERT_TRUE(!iter->Valid());
  iter->SeekToFirst();
  ASSERT_OK(iter->status());
  for (auto kv_iter = kv.begin(); kv_iter != kv.end(); kv_iter++) {
    std::string ikey = ToInternalKey(kv_iter->first);
    ASSERT_EQ(iter->key().ToString(), ikey);
    ASSERT_EQ(iter->value().ToString(), kv_iter->second);
    iter->Next();
    ASSERT_OK(iter->status());
  }
  ASSERT_TRUE(!iter->Valid());
  ASSERT_OK(iter->status());

  // Test backward scan.
  iter->SeekToLast();
  ASSERT_OK(iter->status());
  for (auto kv_iter = kv.rbegin(); kv_iter != kv.rend(); kv_iter++) {
    std::string ikey = ToInternalKey(kv_iter->first);
    ASSERT_EQ(iter->key().ToString(), ikey);
    ASSERT_EQ(iter->value().ToString(), kv_iter->second);
    iter->Prev();
    ASSERT_OK(iter->status());
  }
  ASSERT_TRUE(!iter->Valid());
  ASSERT_OK(iter->status());
}

class ChargeTableReaderTest
    : public BlockBasedTableReaderBaseTest,
      public testing::WithParamInterface<
          CacheEntryRoleOptions::Decision /* charge_table_reader_mem */> {
 protected:
  static std::size_t CalculateMaxTableReaderNumBeforeCacheFull(
      std::size_t cache_capacity, std::size_t approx_table_reader_mem) {
    // To make calculation easier for testing
    assert(cache_capacity % CacheReservationManagerImpl<
                                rocksdb_rs::cache::CacheEntryRole::kBlockBasedTableReader>::
                                GetDummyEntrySize() ==
               0 &&
           cache_capacity >= 2 * CacheReservationManagerImpl<
                                     rocksdb_rs::cache::CacheEntryRole::kBlockBasedTableReader>::
                                     GetDummyEntrySize());

    // We need to subtract 1 for max_num_dummy_entry to account for dummy
    // entries' overhead, assumed the overhead is no greater than 1 dummy entry
    // size
    std::size_t max_num_dummy_entry =
        (size_t)std::floor((
            1.0 * cache_capacity /
            CacheReservationManagerImpl<
                rocksdb_rs::cache::CacheEntryRole::kBlockBasedTableReader>::GetDummyEntrySize())) -
        1;
    std::size_t cache_capacity_rounded_to_dummy_entry_multiples =
        max_num_dummy_entry *
        CacheReservationManagerImpl<
            rocksdb_rs::cache::CacheEntryRole::kBlockBasedTableReader>::GetDummyEntrySize();
    std::size_t max_table_reader_num_capped = static_cast<std::size_t>(
        std::floor(1.0 * cache_capacity_rounded_to_dummy_entry_multiples /
                   approx_table_reader_mem));

    return max_table_reader_num_capped;
  }

  void SetUp() override {
    // To cache and re-use the same kv map and compression type in the test
    // suite for elimiating variance caused by these two factors
    kv_ = BlockBasedTableReaderBaseTest::GenerateKVMap();
    compression_type_ = rocksdb_rs::compression_type::CompressionType::kNoCompression;

    table_reader_charge_tracking_cache_ = std::make_shared<
        TargetCacheChargeTrackingCache<
            rocksdb_rs::cache::CacheEntryRole::kBlockBasedTableReader>>((NewLRUCache(
        4 * CacheReservationManagerImpl<
                rocksdb_rs::cache::CacheEntryRole::kBlockBasedTableReader>::GetDummyEntrySize(),
        0 /* num_shard_bits */, true /* strict_capacity_limit */)));

    // To ApproximateTableReaderMem() without being affected by
    // the feature of charging its memory, we turn off the feature
    charge_table_reader_ = CacheEntryRoleOptions::Decision::kDisabled;
    BlockBasedTableReaderBaseTest::SetUp();
    approx_table_reader_mem_ = ApproximateTableReaderMem();

    // Now we condtionally turn on the feature to test
    charge_table_reader_ = GetParam();
    ConfigureTableFactory();
  }

  void ConfigureTableFactory() override {
    BlockBasedTableOptions table_options;
    table_options.cache_usage_options.options_overrides.insert(
        {rocksdb_rs::cache::CacheEntryRole::kBlockBasedTableReader,
         {/*.charged = */ charge_table_reader_}});
    table_options.block_cache = table_reader_charge_tracking_cache_;

    table_options.cache_index_and_filter_blocks = false;
    table_options.filter_policy.reset(NewBloomFilterPolicy(10, false));
    table_options.partition_filters = true;
    table_options.index_type = BlockBasedTableOptions::kTwoLevelIndexSearch;

    options_.table_factory.reset(NewBlockBasedTableFactory(table_options));
  }

  CacheEntryRoleOptions::Decision charge_table_reader_;
  std::shared_ptr<
      TargetCacheChargeTrackingCache<rocksdb_rs::cache::CacheEntryRole::kBlockBasedTableReader>>
      table_reader_charge_tracking_cache_;
  std::size_t approx_table_reader_mem_;
  std::map<std::string, std::string> kv_;
  rocksdb_rs::compression_type::CompressionType compression_type_;

 private:
  std::size_t ApproximateTableReaderMem() {
    std::size_t approx_table_reader_mem = 0;

    std::string table_name = "table_for_approx_table_reader_mem";
    ImmutableOptions ioptions(options_);
    CreateTable(table_name, ioptions, compression_type_, kv_);

    std::unique_ptr<BlockBasedTable> table;
    rocksdb_rs::status::Status s = rocksdb_rs::status::Status_new();
    NewBlockBasedTableReader(
        FileOptions(), ImmutableOptions(options_),
        InternalKeyComparator(options_.comparator), table_name, &table,
        false /* prefetch_index_and_filter_in_cache */, &s);
    assert(s.ok());

    approx_table_reader_mem = table->ApproximateMemoryUsage();
    assert(approx_table_reader_mem > 0);
    return approx_table_reader_mem;
  }
};

INSTANTIATE_TEST_CASE_P(
    ChargeTableReaderTest, ChargeTableReaderTest,
    ::testing::Values(CacheEntryRoleOptions::Decision::kEnabled,
                      CacheEntryRoleOptions::Decision::kDisabled));

TEST_P(ChargeTableReaderTest, Basic) {
  const std::size_t max_table_reader_num_capped =
      ChargeTableReaderTest::CalculateMaxTableReaderNumBeforeCacheFull(
          table_reader_charge_tracking_cache_->GetCapacity(),
          approx_table_reader_mem_);

  // Acceptable estimtation errors coming from
  // 1. overstimate max_table_reader_num_capped due to # dummy entries is high
  // and results in metadata charge overhead greater than 1 dummy entry size
  // (violating our assumption in calculating max_table_reader_num_capped)
  // 2. overestimate/underestimate max_table_reader_num_capped due to the gap
  // between ApproximateTableReaderMem() and actual table reader mem
  std::size_t max_table_reader_num_capped_upper_bound =
      (std::size_t)(max_table_reader_num_capped * 1.05);
  std::size_t max_table_reader_num_capped_lower_bound =
      (std::size_t)(max_table_reader_num_capped * 0.95);
  std::size_t max_table_reader_num_uncapped =
      (std::size_t)(max_table_reader_num_capped * 1.1);
  ASSERT_GT(max_table_reader_num_uncapped,
            max_table_reader_num_capped_upper_bound)
      << "We need `max_table_reader_num_uncapped` > "
         "`max_table_reader_num_capped_upper_bound` to differentiate cases "
         "between "
         "charge_table_reader_ == kDisabled and == kEnabled)";

  rocksdb_rs::status::Status s = rocksdb_rs::status::Status_OK();
  std::size_t opened_table_reader_num = 0;
  std::string table_name;
  std::vector<std::unique_ptr<BlockBasedTable>> tables;
  ImmutableOptions ioptions(options_);
  // Keep creating BlockBasedTableReader till hiting the memory limit based on
  // cache capacity and creation fails (when charge_table_reader_ ==
  // kEnabled) or reaching a specfied big number of table readers (when
  // charge_table_reader_ == kDisabled)
  while (s.ok() && opened_table_reader_num < max_table_reader_num_uncapped) {
    table_name = "table_" + std::to_string(opened_table_reader_num);
    CreateTable(table_name, ioptions, compression_type_, kv_);
    tables.push_back(std::unique_ptr<BlockBasedTable>());
    NewBlockBasedTableReader(
        FileOptions(), ImmutableOptions(options_),
        InternalKeyComparator(options_.comparator), table_name, &tables.back(),
        false /* prefetch_index_and_filter_in_cache */, &s);
    if (s.ok()) {
      ++opened_table_reader_num;
    }
  }

  if (charge_table_reader_ == CacheEntryRoleOptions::Decision::kEnabled) {
    EXPECT_TRUE(s.IsMemoryLimit()) << "s: " << *s.ToString();
    EXPECT_TRUE(s.ToString()->find(
                    static_cast<std::string>(CacheEntryRole_ToCamelString(
                        rocksdb_rs::cache::CacheEntryRole::kBlockBasedTableReader))) !=
                std::string::npos);
    EXPECT_TRUE(s.ToString()->find("memory limit based on cache capacity") !=
                std::string::npos);

    EXPECT_GE(opened_table_reader_num, max_table_reader_num_capped_lower_bound);
    EXPECT_LE(opened_table_reader_num, max_table_reader_num_capped_upper_bound);

    std::size_t updated_max_table_reader_num_capped =
        ChargeTableReaderTest::CalculateMaxTableReaderNumBeforeCacheFull(
            table_reader_charge_tracking_cache_->GetCapacity() / 2,
            approx_table_reader_mem_);

    // Keep deleting BlockBasedTableReader to lower down memory usage from the
    // memory limit to make the next creation succeeds
    while (opened_table_reader_num >= updated_max_table_reader_num_capped) {
      tables.pop_back();
      --opened_table_reader_num;
    }
    table_name = "table_for_successful_table_reader_open";
    CreateTable(table_name, ioptions, compression_type_, kv_);
    tables.push_back(std::unique_ptr<BlockBasedTable>());
    NewBlockBasedTableReader(
        FileOptions(), ImmutableOptions(options_),
        InternalKeyComparator(options_.comparator), table_name, &tables.back(),
        false /* prefetch_index_and_filter_in_cache */, &s);
    EXPECT_TRUE(s.ok()) <<*s.ToString();

    tables.clear();
    EXPECT_EQ(table_reader_charge_tracking_cache_->GetCacheCharge(), 0);
  } else {
    EXPECT_TRUE(s.ok() &&
                opened_table_reader_num == max_table_reader_num_uncapped)
        << "s: " << *s.ToString() << " opened_table_reader_num: "
        << std::to_string(opened_table_reader_num);
    EXPECT_EQ(table_reader_charge_tracking_cache_->GetCacheCharge(), 0);
  }
}

class BlockBasedTableReaderTestVerifyChecksum
    : public BlockBasedTableReaderTest {
 public:
  BlockBasedTableReaderTestVerifyChecksum() : BlockBasedTableReaderTest() {}
};

TEST_P(BlockBasedTableReaderTestVerifyChecksum, ChecksumMismatch) {
  Options options;
  ReadOptions read_opts;
  std::string dummy_ts(sizeof(uint64_t), '\0');
  Slice read_timestamp = dummy_ts;
  if (udt_enabled_) {
    options.comparator = test::BytewiseComparatorWithU64TsWrapper();
    read_opts.timestamp = &read_timestamp;
  }
  options.persist_user_defined_timestamps = persist_udt_;
  size_t ts_sz = options.comparator->timestamp_size();
  std::map<std::string, std::string> kv =
      BlockBasedTableReaderBaseTest::GenerateKVMap(
          800 /* num_block */,
          false /* mixed_with_human_readable_string_value=*/, ts_sz);

  options.statistics = CreateDBStatistics();
  ImmutableOptions ioptions(options);
  std::string table_name =
      "BlockBasedTableReaderTest" + CompressionTypeToString(compression_type_);
  CreateTable(table_name, ioptions, compression_type_, kv,
              compression_parallel_threads_, compression_dict_bytes_);

  std::unique_ptr<BlockBasedTable> table;
  FileOptions foptions;
  foptions.use_direct_reads = use_direct_reads_;
  InternalKeyComparator comparator(options.comparator);
  NewBlockBasedTableReader(foptions, ioptions, comparator, table_name, &table,
                           true /* bool prefetch_index_and_filter_in_cache */,
                           nullptr /* status */, persist_udt_);

  // Use the top level iterator to find the offset/size of the first
  // 2nd level index block and corrupt the block
  IndexBlockIter iiter_on_stack;
  BlockCacheLookupContext context{TableReaderCaller::kUserVerifyChecksum};
  InternalIteratorBase<IndexValue>* iiter = table->NewIndexIterator(
      read_opts, /*need_upper_bound_check=*/false, &iiter_on_stack,
      /*get_context=*/nullptr, &context);
  std::unique_ptr<InternalIteratorBase<IndexValue>> iiter_unique_ptr;
  if (iiter != &iiter_on_stack) {
    iiter_unique_ptr = std::unique_ptr<InternalIteratorBase<IndexValue>>(iiter);
  }
  ASSERT_OK(iiter->status());
  iiter->SeekToFirst();
  BlockHandle handle = static_cast<PartitionedIndexIterator*>(iiter)
                           ->index_iter_->value()
                           .handle;
  table.reset();

  // Corrupt the block pointed to by handle
  ASSERT_OK(test::CorruptFile(options.env, Path(table_name),
                              static_cast<int>(handle.offset()), 128));

  NewBlockBasedTableReader(foptions, ioptions, comparator, table_name, &table,
                           true /* bool prefetch_index_and_filter_in_cache */,
                           nullptr /* status */, persist_udt_);
  ASSERT_EQ(0,
            options.statistics->getTickerCount(BLOCK_CHECKSUM_MISMATCH_COUNT));
  rocksdb_rs::status::Status s =
      table->VerifyChecksum(read_opts, TableReaderCaller::kUserVerifyChecksum);
  ASSERT_EQ(1,
            options.statistics->getTickerCount(BLOCK_CHECKSUM_MISMATCH_COUNT));
  ASSERT_EQ(s.code(), rocksdb_rs::status::Code::kCorruption);
}

// Param 1: compression type
// Param 2: whether to use direct reads
// Param 3: Block Based Table Index type, partitioned filters are also enabled
//          when index type is kTwoLevelIndexSearch
// Param 4: BBTO no_block_cache option
// Param 5: test mode for the user-defined timestamp feature
// Param 6: number of parallel compression threads
// Param 7: CompressionOptions.max_dict_bytes and
//          CompressionOptions.max_dict_buffer_bytes. This enable/disables
//          compression dictionary.
INSTANTIATE_TEST_CASE_P(
    BlockBasedTableReaderTest, BlockBasedTableReaderTest,
    ::testing::Combine(
        ::testing::ValuesIn(GetSupportedCompressions()), ::testing::Bool(),
        ::testing::Values(
            BlockBasedTableOptions::IndexType::kBinarySearch,
            BlockBasedTableOptions::IndexType::kHashSearch,
            BlockBasedTableOptions::IndexType::kTwoLevelIndexSearch,
            BlockBasedTableOptions::IndexType::kBinarySearchWithFirstKey),
        ::testing::Values(false), ::testing::ValuesIn(test::GetUDTTestModes()),
        ::testing::Values(1, 2), ::testing::Values(0, 4096)));
INSTANTIATE_TEST_CASE_P(
    VerifyChecksum, BlockBasedTableReaderTestVerifyChecksum,
    ::testing::Combine(
        ::testing::ValuesIn(GetSupportedCompressions()),
        ::testing::Values(false),
        ::testing::Values(
            BlockBasedTableOptions::IndexType::kTwoLevelIndexSearch),
        ::testing::Values(true), ::testing::ValuesIn(test::GetUDTTestModes()),
        ::testing::Values(1, 2), ::testing::Values(0)));

}  // namespace rocksdb

int main(int argc, char** argv) {
  rocksdb::port::InstallStackTraceHandler();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
