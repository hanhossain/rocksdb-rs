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

}  // namespace ROCKSDB_NAMESPACE
