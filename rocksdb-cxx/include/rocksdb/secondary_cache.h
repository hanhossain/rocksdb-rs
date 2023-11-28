// Copyright (c) 2021, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
#pragma once

#include <stdint.h>

#include <memory>
#include <string>

#include "rocksdb/advanced_cache.h"
#include "rocksdb/customizable.h"
#include "rocksdb/slice.h"
#include "rocksdb/statistics.h"

#ifndef ROCKSDB_RS
#include "rocksdb-rs-cxx/status.h"
#else
#include "rocksdb-rs/src/status.rs.h"
#endif

namespace ROCKSDB_NAMESPACE {

// A handle for lookup result. Immediately after SecondaryCache::Lookup() with
// wait=false (and depending on the implementation), the handle could be in any
// of the below states. It must not be destroyed while in the pending state.
// * Pending state (IsReady() == false): result is not ready. Value() and Size()
// must not be called.
// * Ready + not found state (IsReady() == true, Value() == nullptr): the lookup
// has completed, finding no match. Or an error occurred that prevented
// normal completion of the Lookup.
// * Ready + found state (IsReady() == false, Value() != nullptr): the lookup
// has completed, finding an entry that has been loaded into an object that is
// now owned by the caller.
//
// Wait() or SecondaryCache::WaitAll() may be skipped if IsReady() happens to
// return true, but (depending on the implementation) IsReady() might never
// return true without Wait() or SecondaryCache::WaitAll(). After the handle
// is known ready, calling Value() is required to avoid a memory leak in case
// of a cache hit.
class SecondaryCacheResultHandle {
 public:
  virtual ~SecondaryCacheResultHandle() = default;

  // Returns whether the handle is ready or not
  virtual bool IsReady() = 0;

  // Block until handle becomes ready
  virtual void Wait() = 0;

  // Return the cache entry object (also known as value). If nullptr, it means
  // the lookup was unsuccessful.
  virtual Cache::ObjectPtr Value() = 0;

  // Return the out_charge from the helper->create_cb used to construct the
  // object.
  // WART: potentially confusing name
  virtual size_t Size() = 0;
};

// SecondaryCache
//
// Cache interface for caching blocks on a secondary tier (which can include
// non-volatile media, or alternate forms of caching such as compressed data)
//
// Exceptions MUST NOT propagate out of overridden functions into RocksDB,
// because RocksDB is not exception-safe. This could cause undefined behavior
// including data loss, unreported corruption, deadlocks, and more.
class SecondaryCache : public Customizable {
 public:
  ~SecondaryCache() override = default;

  static const char* Type() { return "SecondaryCache"; }
  static Status CreateFromString(const ConfigOptions& config_options,
                                 const std::string& id,
                                 std::shared_ptr<SecondaryCache>* result);

  // Suggest inserting an entry into this cache. The caller retains ownership
  // of `obj` (also called the "value"), so is only used directly by the
  // SecondaryCache during Insert(). When the cache chooses to perform the
  // suggested insertion, it uses the size_cb and saveto_cb provided by
  // `helper` to extract the persistable data (typically an uncompressed block)
  // and writes it to this cache tier. OK may be returned even if the insertion
  // is not made.
  virtual Status Insert(const Slice& key, Cache::ObjectPtr obj,
                        const Cache::CacheItemHelper* helper) = 0;

  // Insert a value from its saved/persistable data (typically uncompressed
  // block), as if generated by SaveToCallback/SizeCallback. This can be used
  // in "warming up" the cache from some auxiliary source, and like Insert()
  // may or may not write it to cache depending on the admission control
  // policy, even if the return status is success.
  //
  // The default implementation only assumes the entry helper's create_cb is
  // called at Lookup() time and not Insert() time, so should work for all
  // foreseeable implementations.
  virtual Status InsertSaved(const Slice& key, const Slice& saved);

  // Lookup the data for the given key in this cache. The create_cb
  // will be used to create the object. The handle returned may not be
  // ready yet, unless wait=true, in which case Lookup() will block until
  // the handle is ready.
  //
  // advise_erase is a hint from the primary cache indicating that the handle
  // will be cached there, so the secondary cache is advised to drop it from
  // the cache as an optimization. To use this feature, SupportForceErase()
  // needs to return true.
  // This hint can also be safely ignored.
  //
  // kept_in_sec_cache is to indicate whether the entry will be kept in the
  // secondary cache after the Lookup (rather than erased because of Lookup)
  virtual std::unique_ptr<SecondaryCacheResultHandle> Lookup(
      const Slice& key, const Cache::CacheItemHelper* helper,
      Cache::CreateContext* create_context, bool wait, bool advise_erase,
      bool& kept_in_sec_cache) = 0;

  // Indicate whether a handle can be erased in this secondary cache.
  [[nodiscard]] virtual bool SupportForceErase() const = 0;

  // At the discretion of the implementation, erase the data associated
  // with key.
  virtual void Erase(const Slice& key) = 0;

  // Wait for a collection of handles to become ready.
  virtual void WaitAll(std::vector<SecondaryCacheResultHandle*> handles) = 0;

  // Set the maximum configured capacity of the cache.
  // When the new capacity is less than the old capacity and the existing usage
  // is greater than new capacity, the implementation will do its best job to
  // purge the released entries from the cache in order to lower the usage.
  //
  // The derived class can make this function no-op and return NotSupported().
  virtual Status SetCapacity(size_t /* capacity */) {
    return Status_NotSupported();
  }

  // The derived class can make this function no-op and return NotSupported().
  virtual Status GetCapacity(size_t& /* capacity */) {
    return Status_NotSupported();
  }

  // Temporarily decrease the cache capacity in RAM by the specified amount.
  // The caller should call Inflate() to restore the cache capacity. This is
  // intended to be lighter weight than SetCapacity(). The latter evenly
  // distributes the new capacity across all shards and is meant for large
  // changes in capacity, whereas the former is meant for relatively small
  // changes and may be uneven by lowering capacity in a single shard.
  virtual Status Deflate(size_t /*decrease*/) { return Status_NotSupported(); }

  // Restore the capacity reduced by a prior call to Deflate().
  virtual Status Inflate(size_t /*increase*/) { return Status_NotSupported(); }
};

}  // namespace ROCKSDB_NAMESPACE
