//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
#include "file/filename.h"

#include <ctype.h>
#include <stdio.h>

#include <cinttypes>
#include <vector>

#include "file/writable_file_writer.h"
#include "rocksdb/env.h"
#include "test_util/sync_point.h"
#include "util/stop_watch.h"
#include "util/string_util.h"

using rocksdb_rs::filename::CurrentFileName;
using rocksdb_rs::filename::DescriptorFileName;
using rocksdb_rs::filename::IdentityFileName;
using rocksdb_rs::filename::InfoLogPrefix;
using rocksdb_rs::filename::InfoLogPrefix_new;
using rocksdb_rs::filename::ParseFileName;
using rocksdb_rs::filename::TempFileName;

namespace rocksdb {

const std::string kCurrentFileName = "CURRENT";
const std::string kOptionsFileNamePrefix = "OPTIONS-";
const std::string kTempFileNameSuffix = "dbtmp";

static const std::string kRocksDbTFileExt = "sst";
static const std::string kLevelDbTFileExt = "ldb";
static const std::string kRocksDBBlobFileExt = "blob";
static const std::string kArchivalDirName = "archive";

rocksdb_rs::io_status::IOStatus SetCurrentFile(FileSystem* fs, const std::string& dbname,
                        uint64_t descriptor_number,
                        FSDirectory* dir_contains_current_file) {
  // Remove leading "dbname/" and add newline to manifest file name
  std::string manifest = static_cast<std::string>(DescriptorFileName(dbname, descriptor_number));
  Slice contents = manifest;
  assert(contents.starts_with(dbname + "/"));
  contents.remove_prefix(dbname.size() + 1);
  std::string tmp = static_cast<std::string>(TempFileName(dbname, descriptor_number));
  rocksdb_rs::io_status::IOStatus s = WriteStringToFile(fs, contents.ToString() + "\n", tmp, true);
  TEST_SYNC_POINT_CALLBACK("SetCurrentFile:BeforeRename", &s);
  if (s.ok()) {
    TEST_KILL_RANDOM_WITH_WEIGHT("SetCurrentFile:0", REDUCE_ODDS2);
    s = fs->RenameFile(tmp, static_cast<std::string>(CurrentFileName(dbname)), IOOptions(), nullptr);
    TEST_KILL_RANDOM_WITH_WEIGHT("SetCurrentFile:1", REDUCE_ODDS2);
    TEST_SYNC_POINT_CALLBACK("SetCurrentFile:AfterRename", &s);
  }
  if (s.ok()) {
    if (dir_contains_current_file != nullptr) {
      s = dir_contains_current_file->FsyncWithDirOptions(
          IOOptions(), nullptr, DirFsyncOptions(static_cast<std::string>(CurrentFileName(dbname))));
    }
  } else {
    fs->DeleteFile(tmp, IOOptions(), nullptr);
  }
  return s;
}

rocksdb_rs::status::Status SetIdentityFile(Env* env, const std::string& dbname,
                       const std::string& db_id) {
  std::string id;
  if (db_id.empty()) {
    id = env->GenerateUniqueId();
  } else {
    id = db_id;
  }
  assert(!id.empty());
  // Reserve the filename dbname/000000.dbtmp for the temporary identity file
  std::string tmp = static_cast<std::string>(TempFileName(dbname, 0));
  std::string identify_file_name = static_cast<std::string>(IdentityFileName(dbname));
  rocksdb_rs::status::Status s = WriteStringToFile(env, id, tmp, true);
  if (s.ok()) {
    s = env->RenameFile(tmp, identify_file_name);
  }
  std::unique_ptr<FSDirectory> dir_obj;
  if (s.ok()) {
    s = env->GetFileSystem()->NewDirectory(dbname, IOOptions(), &dir_obj,
                                           nullptr);
  }
  if (s.ok()) {
    s = dir_obj->FsyncWithDirOptions(IOOptions(), nullptr,
                                     DirFsyncOptions(identify_file_name));
  }

  // The default Close() could return "NotSupported" and we bypass it
  // if it is not impelmented. Detailed explanations can be found in
  // db/db_impl/db_impl.h
  if (s.ok()) {
    rocksdb_rs::status::Status temp_s = dir_obj->Close(IOOptions(), nullptr);
    if (!temp_s.ok()) {
      if (!temp_s.IsNotSupported()) {
        s.copy_from(temp_s);
      }
    }
  }
  if (!s.ok()) {
    env->DeleteFile(tmp);
  }
  return s;
}

rocksdb_rs::io_status::IOStatus SyncManifest(const ImmutableDBOptions* db_options,
                      WritableFileWriter* file) {
  TEST_KILL_RANDOM_WITH_WEIGHT("SyncManifest:0", REDUCE_ODDS2);
  StopWatch sw(db_options->clock, db_options->stats, MANIFEST_FILE_SYNC_MICROS);
  return file->Sync(db_options->use_fsync);
}

rocksdb_rs::status::Status GetInfoLogFiles(const std::shared_ptr<FileSystem>& fs,
                       const std::string& db_log_dir, const std::string& dbname,
                       std::string* parent_dir,
                       std::vector<std::string>* info_log_list) {
  assert(parent_dir != nullptr);
  assert(info_log_list != nullptr);
  uint64_t number = 0;
  rocksdb_rs::types::FileType type = rocksdb_rs::types::FileType::kWalFile;

  if (!db_log_dir.empty()) {
    *parent_dir = db_log_dir;
  } else {
    *parent_dir = dbname;
  }

  InfoLogPrefix info_log_prefix = InfoLogPrefix_new(!db_log_dir.empty(), dbname);

  std::vector<std::string> file_names;
  rocksdb_rs::status::Status s = fs->GetChildren(*parent_dir, IOOptions(), &file_names, nullptr);

  if (!s.ok()) {
    return s;
  }

  for (auto& f : file_names) {
    if (ParseFileName(f, &number, info_log_prefix.prefix, &type) &&
        (type == rocksdb_rs::types::FileType::kInfoLogFile)) {
      info_log_list->push_back(f);
    }
  }
  return rocksdb_rs::status::Status_OK();
}
}  // namespace rocksdb
