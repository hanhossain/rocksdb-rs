//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include "db/db_impl/compacted_db_impl.h"

#include "db/db_impl/db_impl.h"
#include "db/version_set.h"
#include "logging/logging.h"
#include "table/get_context.h"
#include "util/cast_util.h"

namespace rocksdb {

extern void MarkKeyMayExist(void* arg);
extern bool SaveValue(void* arg, const ParsedInternalKey& parsed_key,
                      const Slice& v, bool hit_and_return);

CompactedDBImpl::CompactedDBImpl(const DBOptions& options,
                                 const std::string& dbname)
    : DBImpl(options, dbname, /*seq_per_batch*/ false, +/*batch_per_txn*/ true,
             /*read_only*/ true),
      cfd_(nullptr),
      version_(nullptr),
      user_comparator_(nullptr) {}

CompactedDBImpl::~CompactedDBImpl() {}

size_t CompactedDBImpl::FindFile(const Slice& key) {
  size_t right = files_.num_files - 1;
  auto cmp = [&](const FdWithKeyRange& f, const Slice& k) -> bool {
    return user_comparator_->Compare(ExtractUserKey(f.largest_key), k) < 0;
  };
  return static_cast<size_t>(
      std::lower_bound(files_.files, files_.files + right, key, cmp) -
      files_.files);
}

rocksdb_rs::status::Status CompactedDBImpl::Get(const ReadOptions& options, ColumnFamilyHandle*,
                            const Slice& key, PinnableSlice* value) {
  return Get(options, /*column_family*/ nullptr, key, value,
             /*timestamp*/ nullptr);
}

rocksdb_rs::status::Status CompactedDBImpl::Get(const ReadOptions& options, ColumnFamilyHandle*,
                            const Slice& key, PinnableSlice* value,
                            std::string* timestamp) {
  if (options.io_activity != Env::IOActivity::kUnknown) {
    return rocksdb_rs::status::Status_InvalidArgument(
        "Cannot call Get with `ReadOptions::io_activity` != "
        "`Env::IOActivity::kUnknown`");
  }
  assert(user_comparator_);
  if (options.timestamp) {
    const rocksdb_rs::status::Status s = FailIfTsMismatchCf(
        DefaultColumnFamily(), *(options.timestamp), /*ts_for_read=*/true);
    if (!s.ok()) {
      return s.Clone();
    }
  } else {
    const rocksdb_rs::status::Status s = FailIfCfHasTs(DefaultColumnFamily());
    if (!s.ok()) {
      return s.Clone();
    }
  }

  // Clear the timestamps for returning results so that we can distinguish
  // between tombstone or key that has never been written
  if (timestamp) {
    timestamp->clear();
  }

  GetWithTimestampReadCallback read_cb(kMaxSequenceNumber);
  std::string* ts =
      user_comparator_->timestamp_size() > 0 ? timestamp : nullptr;
  LookupKey lkey(key, kMaxSequenceNumber, options.timestamp);
  GetContext get_context(user_comparator_, nullptr, nullptr, nullptr,
                         GetContext::kNotFound, lkey.user_key(), value,
                         /*columns=*/nullptr, ts, nullptr, nullptr, true,
                         nullptr, nullptr, nullptr, nullptr, &read_cb);

  const FdWithKeyRange& f = files_.files[FindFile(lkey.user_key())];
  if (user_comparator_->CompareWithoutTimestamp(
          key, /*a_has_ts=*/false,
          ExtractUserKeyAndStripTimestamp(f.smallest_key,
                                          user_comparator_->timestamp_size()),
          /*b_has_ts=*/false) < 0) {
    return rocksdb_rs::status::Status_NotFound();
  }
  rocksdb_rs::status::Status s = f.fd.table_reader->Get(options, lkey.internal_key(), &get_context,
                                    nullptr);
  if (!s.ok() && !s.IsNotFound()) {
    return s;
  }
  if (get_context.State() == GetContext::kFound) {
    return rocksdb_rs::status::Status_OK();
  }
  return rocksdb_rs::status::Status_NotFound();
}

rust::Vec<rocksdb_rs::status::Status> CompactedDBImpl::MultiGet(
    const ReadOptions& options, const std::vector<ColumnFamilyHandle*>&,
    const std::vector<Slice>& keys, std::vector<std::string>* values) {
  return MultiGet(options, keys, values, /*timestamps*/ nullptr);
}

rust::Vec<rocksdb_rs::status::Status> CompactedDBImpl::MultiGet(
    const ReadOptions& options, const std::vector<ColumnFamilyHandle*>&,
    const std::vector<Slice>& keys, std::vector<std::string>* values,
    std::vector<std::string>* timestamps) {
  assert(user_comparator_);
  size_t num_keys = keys.size();

  if (options.timestamp) {
    rocksdb_rs::status::Status s = FailIfTsMismatchCf(DefaultColumnFamily(), *(options.timestamp),
                                  /*ts_for_read=*/true);
    if (!s.ok()) {
      return s.create_vec(num_keys);
    }
  } else {
    rocksdb_rs::status::Status s = FailIfCfHasTs(DefaultColumnFamily());
    if (!s.ok()) {
      return s.create_vec(num_keys);
    }
  }

  // Clear the timestamps for returning results so that we can distinguish
  // between tombstone or key that has never been written
  if (timestamps) {
    for (auto& ts : *timestamps) {
      ts.clear();
    }
  }

  GetWithTimestampReadCallback read_cb(kMaxSequenceNumber);
  autovector<TableReader*, 16> reader_list;
  for (const auto& key : keys) {
    LookupKey lkey(key, kMaxSequenceNumber, options.timestamp);
    const FdWithKeyRange& f = files_.files[FindFile(lkey.user_key())];
    if (user_comparator_->CompareWithoutTimestamp(
            key, /*a_has_ts=*/false,
            ExtractUserKeyAndStripTimestamp(f.smallest_key,
                                            user_comparator_->timestamp_size()),
            /*b_has_ts=*/false) < 0) {
      reader_list.push_back(nullptr);
    } else {
      f.fd.table_reader->Prepare(lkey.internal_key());
      reader_list.push_back(f.fd.table_reader);
    }
  }
  rust::Vec<rocksdb_rs::status::Status> statuses = rocksdb_rs::status::Status_NotFound().create_vec(num_keys);
  values->resize(num_keys);
  if (timestamps) {
    timestamps->resize(num_keys);
  }
  int idx = 0;
  for (auto* r : reader_list) {
    if (r != nullptr) {
      PinnableSlice pinnable_val;
      std::string& value = (*values)[idx];
      LookupKey lkey(keys[idx], kMaxSequenceNumber, options.timestamp);
      std::string* timestamp = timestamps ? &(*timestamps)[idx] : nullptr;
      GetContext get_context(
          user_comparator_, nullptr, nullptr, nullptr, GetContext::kNotFound,
          lkey.user_key(), &pinnable_val, /*columns=*/nullptr,
          user_comparator_->timestamp_size() > 0 ? timestamp : nullptr, nullptr,
          nullptr, true, nullptr, nullptr, nullptr, nullptr, &read_cb);
      rocksdb_rs::status::Status s = r->Get(options, lkey.internal_key(), &get_context, nullptr);
      assert(static_cast<size_t>(idx) < statuses.size());
      if (!s.ok() && !s.IsNotFound()) {
        statuses[idx].copy_from(s);
      } else {
        value.assign(pinnable_val.data(), pinnable_val.size());
        if (get_context.State() == GetContext::kFound) {
          statuses[idx] = rocksdb_rs::status::Status_OK();
        }
      }
    }
    ++idx;
  }
  return statuses;
}

rocksdb_rs::status::Status CompactedDBImpl::Init(const Options& options) {
  SuperVersionContext sv_context(/* create_superversion */ true);
  mutex_.Lock();
  ColumnFamilyDescriptor cf(kDefaultColumnFamilyName,
                            ColumnFamilyOptions(options));
  rocksdb_rs::status::Status s = Recover({cf}, true /* read only */, false, true);
  if (s.ok()) {
    cfd_ = static_cast_with_check<ColumnFamilyHandleImpl>(DefaultColumnFamily())
               ->cfd();
    cfd_->InstallSuperVersion(&sv_context, &mutex_);
  }
  mutex_.Unlock();
  sv_context.Clean();
  if (!s.ok()) {
    return s;
  }
  NewThreadStatusCfInfo(cfd_);
  version_ = cfd_->GetSuperVersion()->current;
  user_comparator_ = cfd_->user_comparator();
  auto* vstorage = version_->storage_info();
  if (vstorage->num_non_empty_levels() == 0) {
    return rocksdb_rs::status::Status_NotSupported("no file exists");
  }
  const LevelFilesBrief& l0 = vstorage->LevelFilesBrief(0);
  // L0 should not have files
  if (l0.num_files > 1) {
    return rocksdb_rs::status::Status_NotSupported("L0 contain more than 1 file");
  }
  if (l0.num_files == 1) {
    if (vstorage->num_non_empty_levels() > 1) {
      return rocksdb_rs::status::Status_NotSupported("Both L0 and other level contain files");
    }
    files_ = l0;
    return rocksdb_rs::status::Status_OK();
  }

  for (int i = 1; i < vstorage->num_non_empty_levels() - 1; ++i) {
    if (vstorage->LevelFilesBrief(i).num_files > 0) {
      return rocksdb_rs::status::Status_NotSupported("Other levels also contain files");
    }
  }

  int level = vstorage->num_non_empty_levels() - 1;
  if (vstorage->LevelFilesBrief(level).num_files > 0) {
    files_ = vstorage->LevelFilesBrief(level);
    return rocksdb_rs::status::Status_OK();
  }
  return rocksdb_rs::status::Status_NotSupported("no file exists");
}

rocksdb_rs::status::Status CompactedDBImpl::Open(const Options& options, const std::string& dbname,
                             DB** dbptr) {
  *dbptr = nullptr;

  if (options.max_open_files != -1) {
    return rocksdb_rs::status::Status_InvalidArgument("require max_open_files = -1");
  }
  if (options.merge_operator.get() != nullptr) {
    return rocksdb_rs::status::Status_InvalidArgument("merge operator is not supported");
  }
  DBOptions db_options(options);
  std::unique_ptr<CompactedDBImpl> db(new CompactedDBImpl(db_options, dbname));
  rocksdb_rs::status::Status s = db->Init(options);
  if (s.ok()) {
    s = db->StartPeriodicTaskScheduler();
  }
  if (s.ok()) {
    ROCKS_LOG_INFO(db->immutable_db_options_.info_log,
                   "Opened the db as fully compacted mode");
    LogFlush(db->immutable_db_options_.info_log);
    *dbptr = db.release();
  }
  return s;
}

}  // namespace rocksdb
