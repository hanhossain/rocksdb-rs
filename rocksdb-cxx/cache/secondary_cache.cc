//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include "rocksdb/secondary_cache.h"

#include "rocksdb/cache.h"

namespace rocksdb {

namespace {

void NoopDelete(Cache::ObjectPtr, MemoryAllocator*) {}

size_t SliceSize(Cache::ObjectPtr obj) {
  return static_cast<Slice*>(obj)->size();
}

rocksdb_rs::status::Status SliceSaveTo(Cache::ObjectPtr from_obj,
                                       size_t from_offset, size_t length,
                                       char* out) {
  const Slice& slice = *static_cast<Slice*>(from_obj);
  std::memcpy(out, slice.data() + from_offset, length);
  return rocksdb_rs::status::Status_OK();
}

rocksdb_rs::status::Status FailCreate(const Slice&, Cache::CreateContext*,
                                      MemoryAllocator*, Cache::ObjectPtr*,
                                      size_t*) {
  return rocksdb_rs::status::Status_NotSupported(
      "Only for dumping data into SecondaryCache");
}

}  // namespace

rocksdb_rs::status::Status SecondaryCache::InsertSaved(const Slice& key,
                                                       const Slice& saved) {
  static Cache::CacheItemHelper helper_no_secondary{
      rocksdb_rs::cache::CacheEntryRole::kMisc, &NoopDelete};
  static Cache::CacheItemHelper helper{rocksdb_rs::cache::CacheEntryRole::kMisc,
                                       &NoopDelete,
                                       &SliceSize,
                                       &SliceSaveTo,
                                       &FailCreate,
                                       &helper_no_secondary};
  // NOTE: depends on Insert() being synchronous, not keeping pointer `&saved`
  return Insert(key, const_cast<Slice*>(&saved), &helper);
}

}  // namespace rocksdb
