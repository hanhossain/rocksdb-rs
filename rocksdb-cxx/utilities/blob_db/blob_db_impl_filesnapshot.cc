//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).


#include "file/filename.h"
#include "logging/logging.h"
#include "util/cast_util.h"
#include "util/mutexlock.h"
#include "utilities/blob_db/blob_db_impl.h"

// BlobDBImpl methods to get snapshot of files, e.g. for replication.

namespace rocksdb {
namespace blob_db {

rocksdb_rs::status::Status BlobDBImpl::DisableFileDeletions() {
  // Disable base DB file deletions.
  rocksdb_rs::status::Status s = db_impl_->DisableFileDeletions();
  if (!s.ok()) {
    return s;
  }

  int count = 0;
  {
    // Hold delete_file_mutex_ to make sure no DeleteObsoleteFiles job
    // is running.
    MutexLock l(&delete_file_mutex_);
    count = ++disable_file_deletions_;
  }

  ROCKS_LOG_INFO(db_options_.info_log,
                 "Disabled blob file deletions. count: %d", count);
  return rocksdb_rs::status::Status_OK();
}

rocksdb_rs::status::Status BlobDBImpl::EnableFileDeletions(bool force) {
  // Enable base DB file deletions.
  rocksdb_rs::status::Status s = db_impl_->EnableFileDeletions(force);
  if (!s.ok()) {
    return s;
  }

  int count = 0;
  {
    MutexLock l(&delete_file_mutex_);
    if (force) {
      disable_file_deletions_ = 0;
    } else if (disable_file_deletions_ > 0) {
      count = --disable_file_deletions_;
    }
    assert(count >= 0);
  }

  ROCKS_LOG_INFO(db_options_.info_log, "Enabled blob file deletions. count: %d",
                 count);
  // Consider trigger DeleteobsoleteFiles once after re-enabled, if we are to
  // make DeleteobsoleteFiles re-run interval configuration.
  return rocksdb_rs::status::Status_OK();
}

rocksdb_rs::status::Status BlobDBImpl::GetLiveFiles(std::vector<std::string>& ret,
                                uint64_t* manifest_file_size,
                                bool flush_memtable) {
  if (!bdb_options_.path_relative) {
    return rocksdb_rs::status::Status_NotSupported(
        "Not able to get relative blob file path from absolute blob_dir.");
  }
  // Hold a lock in the beginning to avoid updates to base DB during the call
  ReadLock rl(&mutex_);
  rocksdb_rs::status::Status s = db_->GetLiveFiles(ret, manifest_file_size, flush_memtable);
  if (!s.ok()) {
    return s;
  }
  ret.reserve(ret.size() + blob_files_.size());
  for (auto bfile_pair : blob_files_) {
    auto blob_file = bfile_pair.second;
    // Path should be relative to db_name, but begin with slash.
    ret.emplace_back(
        rocksdb_rs::filename::BlobFileName("", bdb_options_.blob_dir, blob_file->BlobFileNumber()));
  }
  return rocksdb_rs::status::Status_OK();
}

void BlobDBImpl::GetLiveFilesMetaData(std::vector<LiveFileMetaData>* metadata) {
  // Path should be relative to db_name.
  assert(bdb_options_.path_relative);
  // Hold a lock in the beginning to avoid updates to base DB during the call
  ReadLock rl(&mutex_);
  db_->GetLiveFilesMetaData(metadata);
  for (auto bfile_pair : blob_files_) {
    auto blob_file = bfile_pair.second;
    LiveFileMetaData filemetadata;
    filemetadata.size = blob_file->GetFileSize();
    const uint64_t file_number = blob_file->BlobFileNumber();
    // Path should be relative to db_name, but begin with slash.
    filemetadata.name = static_cast<std::string>(rocksdb_rs::filename::BlobFileName("", bdb_options_.blob_dir, file_number));
    filemetadata.file_number = file_number;
    if (blob_file->HasTTL()) {
      filemetadata.oldest_ancester_time = blob_file->GetExpirationRange().first;
    }
    auto cfh =
        static_cast_with_check<ColumnFamilyHandleImpl>(DefaultColumnFamily());
    filemetadata.column_family_name = cfh->GetName();
    metadata->emplace_back(filemetadata);
  }
}

}  // namespace blob_db
}  // namespace rocksdb
