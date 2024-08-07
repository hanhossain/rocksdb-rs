//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "rocksdb/cache.h"

#include "cache/lru_cache.h"
#include "rocksdb/secondary_cache.h"
#include "rocksdb/utilities/customizable_util.h"
#include "rocksdb/utilities/options_type.h"
#include "util/string_util.h"

namespace rocksdb {
const Cache::CacheItemHelper kNoopCacheItemHelper{};

static std::unordered_map<std::string, OptionTypeInfo>
    lru_cache_options_type_info = {
        {"capacity",
         {offsetof(struct LRUCacheOptions, capacity), rocksdb_rs::utilities::options_type::OptionType::kSizeT,
          rocksdb_rs::utilities::options_type::OptionVerificationType::kNormal, rocksdb_rs::utilities::options_type::OptionTypeFlags::kMutable}},
        {"num_shard_bits",
         {offsetof(struct LRUCacheOptions, num_shard_bits), rocksdb_rs::utilities::options_type::OptionType::kInt,
          rocksdb_rs::utilities::options_type::OptionVerificationType::kNormal, rocksdb_rs::utilities::options_type::OptionTypeFlags::kMutable}},
        {"strict_capacity_limit",
         {offsetof(struct LRUCacheOptions, strict_capacity_limit),
          rocksdb_rs::utilities::options_type::OptionType::kBoolean, rocksdb_rs::utilities::options_type::OptionVerificationType::kNormal,
          rocksdb_rs::utilities::options_type::OptionTypeFlags::kMutable}},
        {"high_pri_pool_ratio",
         {offsetof(struct LRUCacheOptions, high_pri_pool_ratio),
          rocksdb_rs::utilities::options_type::OptionType::kDouble, rocksdb_rs::utilities::options_type::OptionVerificationType::kNormal,
          rocksdb_rs::utilities::options_type::OptionTypeFlags::kMutable}},
        {"low_pri_pool_ratio",
         {offsetof(struct LRUCacheOptions, low_pri_pool_ratio),
          rocksdb_rs::utilities::options_type::OptionType::kDouble, rocksdb_rs::utilities::options_type::OptionVerificationType::kNormal,
          rocksdb_rs::utilities::options_type::OptionTypeFlags::kMutable}},
};

static std::unordered_map<std::string, OptionTypeInfo>
    comp_sec_cache_options_type_info = {
        {"capacity",
         {offsetof(struct CompressedSecondaryCacheOptions, capacity),
          rocksdb_rs::utilities::options_type::OptionType::kSizeT, rocksdb_rs::utilities::options_type::OptionVerificationType::kNormal,
          rocksdb_rs::utilities::options_type::OptionTypeFlags::kMutable}},
        {"num_shard_bits",
         {offsetof(struct CompressedSecondaryCacheOptions, num_shard_bits),
          rocksdb_rs::utilities::options_type::OptionType::kInt, rocksdb_rs::utilities::options_type::OptionVerificationType::kNormal,
          rocksdb_rs::utilities::options_type::OptionTypeFlags::kMutable}},
        {"compression_type",
         {offsetof(struct CompressedSecondaryCacheOptions, compression_type),
          rocksdb_rs::utilities::options_type::OptionType::kCompressionType, rocksdb_rs::utilities::options_type::OptionVerificationType::kNormal,
          rocksdb_rs::utilities::options_type::OptionTypeFlags::kMutable}},
        {"compress_format_version",
         {offsetof(struct CompressedSecondaryCacheOptions,
                   compress_format_version),
          rocksdb_rs::utilities::options_type::OptionType::kUInt32T, rocksdb_rs::utilities::options_type::OptionVerificationType::kNormal,
          rocksdb_rs::utilities::options_type::OptionTypeFlags::kMutable}},
        {"enable_custom_split_merge",
         {offsetof(struct CompressedSecondaryCacheOptions,
                   enable_custom_split_merge),
          rocksdb_rs::utilities::options_type::OptionType::kBoolean, rocksdb_rs::utilities::options_type::OptionVerificationType::kNormal,
          rocksdb_rs::utilities::options_type::OptionTypeFlags::kMutable}},
};

rocksdb_rs::status::Status SecondaryCache::CreateFromString(
    const ConfigOptions& config_options, const std::string& value,
    std::shared_ptr<SecondaryCache>* result) {
  if (value.find("compressed_secondary_cache://") == 0) {
    std::string args = value;
    args.erase(0, std::strlen("compressed_secondary_cache://"));
    rocksdb_rs::status::Status status = rocksdb_rs::status::Status_new();
    std::shared_ptr<SecondaryCache> sec_cache;

    CompressedSecondaryCacheOptions sec_cache_opts;
    status = OptionTypeInfo::ParseStruct(config_options, "",
                                         &comp_sec_cache_options_type_info, "",
                                         args, &sec_cache_opts);
    if (status.ok()) {
      sec_cache = NewCompressedSecondaryCache(sec_cache_opts);
    }


    if (status.ok()) {
      result->swap(sec_cache);
    }
    return status;
  } else {
    return LoadSharedObject<SecondaryCache>(config_options, value, result);
  }
}

rocksdb_rs::status::Status Cache::CreateFromString(const ConfigOptions& config_options,
                               const std::string& value,
                               std::shared_ptr<Cache>* result) {
  rocksdb_rs::status::Status status = rocksdb_rs::status::Status_new();
  std::shared_ptr<Cache> cache;
  if (value.find('=') == std::string::npos) {
    cache = NewLRUCache(ParseSizeT(value));
  } else {
    LRUCacheOptions cache_opts;
    status = OptionTypeInfo::ParseStruct(config_options, "",
                                         &lru_cache_options_type_info, "",
                                         value, &cache_opts);
    if (status.ok()) {
      cache = NewLRUCache(cache_opts);
    }
  }
  if (status.ok()) {
    result->swap(cache);
  }
  return status;
}

bool Cache::AsyncLookupHandle::IsReady() {
  return pending_handle == nullptr || pending_handle->IsReady();
}

bool Cache::AsyncLookupHandle::IsPending() { return pending_handle != nullptr; }

Cache::Handle* Cache::AsyncLookupHandle::Result() {
  assert(!IsPending());
  return result_handle;
}

void Cache::StartAsyncLookup(AsyncLookupHandle& async_handle) {
  async_handle.found_dummy_entry = false;  // in case re-used
  assert(!async_handle.IsPending());
  async_handle.result_handle =
      Lookup(async_handle.key, async_handle.helper, async_handle.create_context,
             async_handle.priority, async_handle.stats);
}

Cache::Handle* Cache::Wait(AsyncLookupHandle& async_handle) {
  WaitAll(&async_handle, 1);
  return async_handle.Result();
}

void Cache::WaitAll(AsyncLookupHandle* async_handles, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (async_handles[i].IsPending()) {
      // If a pending handle gets here, it should be marked at "to be handled
      // by a caller" by that caller erasing the pending_cache on it.
      assert(async_handles[i].pending_cache == nullptr);
    }
  }
}

void Cache::SetEvictionCallback(EvictionCallback&& fn) {
  // Overwriting non-empty with non-empty could indicate a bug
  assert(!eviction_callback_ || !fn);
  eviction_callback_ = std::move(fn);
}

}  // namespace rocksdb
