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

rust::Str GetCacheEntryRoleName(const CacheEntryRole role) {
  return rs::GetCacheEntryRoleName(role);
}

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

namespace {

std::string GetPrefixedCacheEntryRoleName(const std::string& prefix,
                                          CacheEntryRole role) {
  rust::Str role_name = GetCacheEntryRoleName(role);
  std::string prefixed_role_name;
  prefixed_role_name.reserve(prefix.size() + role_name.size());
  prefixed_role_name.append(prefix);
  prefixed_role_name.append(std::string(role_name));
  return prefixed_role_name;
}

}  // namespace

std::string BlockCacheEntryStatsMapKeys::EntryCount(CacheEntryRole role) {
  const static std::string kPrefix = "count.";
  return GetPrefixedCacheEntryRoleName(kPrefix, role);
}

std::string BlockCacheEntryStatsMapKeys::UsedBytes(CacheEntryRole role) {
  const static std::string kPrefix = "bytes.";
  return GetPrefixedCacheEntryRoleName(kPrefix, role);
}

std::string BlockCacheEntryStatsMapKeys::UsedPercent(CacheEntryRole role) {
  const static std::string kPrefix = "percent.";
  return GetPrefixedCacheEntryRoleName(kPrefix, role);
}

}  // namespace ROCKSDB_NAMESPACE
