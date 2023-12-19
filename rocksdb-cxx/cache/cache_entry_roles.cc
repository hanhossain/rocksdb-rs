//  Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include "cache/cache_entry_roles.h"

#include <mutex>

#include "port/lang.h"

namespace ROCKSDB_NAMESPACE {

std::array<std::string, kNumCacheEntryRoles> kCacheEntryRoleToCamelString{{
    "DataBlock",
    "FilterBlock",
    "FilterMetaBlock",
    "DeprecatedFilterBlock",
    "IndexBlock",
    "OtherBlock",
    "WriteBuffer",
    "CompressionDictionaryBuildingBuffer",
    "FilterConstruction",
    "BlockBasedTableReader",
    "FileMetadata",
    "BlobValue",
    "BlobCache",
    "Misc",
}};

rust::Str BlockCacheEntryStatsMapKeys_CacheId() {
  return rs::BlockCacheEntryStatsMapKeys_CacheId();
}

rust::Str BlockCacheEntryStatsMapKeys_CacheCapacityBytes() {
  return rs::BlockCacheEntryStatsMapKeys_CacheCapacityBytes();
}

rust::Str BlockCacheEntryStatsMapKeys_LastCollectionDurationSeconds() {
  return rs::BlockCacheEntryStatsMapKeys_LastCollectionDurationSeconds();
}

rust::Str BlockCacheEntryStatsMapKeys_LastCollectionAgeSeconds() {
  return rs::BlockCacheEntryStatsMapKeys_LastCollectionAgeSeconds();
}

rust::String BlockCacheEntryStatsMapKeys_EntryCount(CacheEntryRole role) {
  return rs::BlockCacheEntryStatsMapKeys_EntryCount(role);
}

rust::String BlockCacheEntryStatsMapKeys_UsedBytes(CacheEntryRole role) {
  return rs::BlockCacheEntryStatsMapKeys_UsedBytes(role);
}

rust::String BlockCacheEntryStatsMapKeys_UsedPercent(CacheEntryRole role) {
  return rs::BlockCacheEntryStatsMapKeys_UsedPercent(role);
}

}  // namespace ROCKSDB_NAMESPACE
