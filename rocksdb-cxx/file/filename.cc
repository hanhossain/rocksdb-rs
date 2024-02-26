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

namespace rocksdb {

const std::string kCurrentFileName = "CURRENT";
const std::string kOptionsFileNamePrefix = "OPTIONS-";
const std::string kTempFileNameSuffix = "dbtmp";

static const std::string kRocksDbTFileExt = "sst";
static const std::string kLevelDbTFileExt = "ldb";
static const std::string kRocksDBBlobFileExt = "blob";
static const std::string kArchivalDirName = "archive";

std::string InfoLogFileName(const std::string& dbname,
                            const std::string& db_path,
                            const std::string& log_dir) {
  if (log_dir.empty()) {
    return dbname + "/LOG";
  }

  InfoLogPrefix info_log_prefix = InfoLogPrefix_new(true, db_path);
  return log_dir + "/" + static_cast<std::string>(info_log_prefix.prefix);
}

// Return the name of the old info log file for "dbname".
std::string OldInfoLogFileName(const std::string& dbname, uint64_t ts,
                               const std::string& db_path,
                               const std::string& log_dir) {
  char buf[50];
  snprintf(buf, sizeof(buf), "%llu", static_cast<unsigned long long>(ts));

  if (log_dir.empty()) {
    return dbname + "/LOG.old." + buf;
  }

  InfoLogPrefix info_log_prefix = InfoLogPrefix_new(true, db_path);
  return log_dir + "/" + static_cast<std::string>(info_log_prefix.prefix) + ".old." + buf;
}

// Owned filenames have the form:
//    dbname/IDENTITY
//    dbname/CURRENT
//    dbname/LOCK
//    dbname/<info_log_name_prefix>
//    dbname/<info_log_name_prefix>.old.[0-9]+
//    dbname/MANIFEST-[0-9]+
//    dbname/[0-9]+.(log|sst|blob)
//    dbname/METADB-[0-9]+
//    dbname/OPTIONS-[0-9]+
//    dbname/OPTIONS-[0-9]+.dbtmp
//    Disregards / at the beginning
bool ParseFileName(const std::string& fname, uint64_t* number, FileType* type,
                   WalFileType* log_type) {
  return ParseFileName(fname, number, "", type, log_type);
}

bool ParseFileName(const std::string& fname, uint64_t* number,
                   const Slice& info_log_name_prefix, FileType* type,
                   WalFileType* log_type) {
  Slice rest(fname);
  if (fname.length() > 1 && fname[0] == '/') {
    rest.remove_prefix(1);
  }
  if (rest == "IDENTITY") {
    *number = 0;
    *type = kIdentityFile;
  } else if (rest == "CURRENT") {
    *number = 0;
    *type = kCurrentFile;
  } else if (rest == "LOCK") {
    *number = 0;
    *type = kDBLockFile;
  } else if (info_log_name_prefix.size() > 0 &&
             rest.starts_with(info_log_name_prefix)) {
    rest.remove_prefix(info_log_name_prefix.size());
    if (rest == "" || rest == ".old") {
      *number = 0;
      *type = kInfoLogFile;
    } else if (rest.starts_with(".old.")) {
      uint64_t ts_suffix;
      // sizeof also counts the trailing '\0'.
      rest.remove_prefix(sizeof(".old.") - 1);
      if (!ConsumeDecimalNumber(&rest, &ts_suffix)) {
        return false;
      }
      *number = ts_suffix;
      *type = kInfoLogFile;
    }
  } else if (rest.starts_with("MANIFEST-")) {
    rest.remove_prefix(strlen("MANIFEST-"));
    uint64_t num;
    if (!ConsumeDecimalNumber(&rest, &num)) {
      return false;
    }
    if (!rest.empty()) {
      return false;
    }
    *type = kDescriptorFile;
    *number = num;
  } else if (rest.starts_with("METADB-")) {
    rest.remove_prefix(strlen("METADB-"));
    uint64_t num;
    if (!ConsumeDecimalNumber(&rest, &num)) {
      return false;
    }
    if (!rest.empty()) {
      return false;
    }
    *type = kMetaDatabase;
    *number = num;
  } else if (rest.starts_with(kOptionsFileNamePrefix)) {
    uint64_t ts_suffix;
    bool is_temp_file = false;
    rest.remove_prefix(kOptionsFileNamePrefix.size());
    const std::string kTempFileNameSuffixWithDot =
        std::string(".") + kTempFileNameSuffix;
    if (rest.ends_with(kTempFileNameSuffixWithDot)) {
      rest.remove_suffix(kTempFileNameSuffixWithDot.size());
      is_temp_file = true;
    }
    if (!ConsumeDecimalNumber(&rest, &ts_suffix)) {
      return false;
    }
    *number = ts_suffix;
    *type = is_temp_file ? kTempFile : kOptionsFile;
  } else {
    // Avoid strtoull() to keep filename format independent of the
    // current locale
    bool archive_dir_found = false;
    if (rest.starts_with(kArchivalDirName)) {
      if (rest.size() <= kArchivalDirName.size()) {
        return false;
      }
      rest.remove_prefix(kArchivalDirName.size() +
                         1);  // Add 1 to remove / also
      if (log_type) {
        *log_type = kArchivedLogFile;
      }
      archive_dir_found = true;
    }
    uint64_t num;
    if (!ConsumeDecimalNumber(&rest, &num)) {
      return false;
    }
    if (rest.size() <= 1 || rest[0] != '.') {
      return false;
    }
    rest.remove_prefix(1);

    Slice suffix = rest;
    if (suffix == Slice("log")) {
      *type = kWalFile;
      if (log_type && !archive_dir_found) {
        *log_type = kAliveLogFile;
      }
    } else if (archive_dir_found) {
      return false;  // Archive dir can contain only log files
    } else if (suffix == Slice(kRocksDbTFileExt) ||
               suffix == Slice(kLevelDbTFileExt)) {
      *type = kTableFile;
    } else if (suffix == Slice(kRocksDBBlobFileExt)) {
      *type = kBlobFile;
    } else if (suffix == Slice(kTempFileNameSuffix)) {
      *type = kTempFile;
    } else {
      return false;
    }
    *number = num;
  }
  return true;
}

IOStatus SetCurrentFile(FileSystem* fs, const std::string& dbname,
                        uint64_t descriptor_number,
                        FSDirectory* dir_contains_current_file) {
  // Remove leading "dbname/" and add newline to manifest file name
  std::string manifest = static_cast<std::string>(DescriptorFileName(dbname, descriptor_number));
  Slice contents = manifest;
  assert(contents.starts_with(dbname + "/"));
  contents.remove_prefix(dbname.size() + 1);
  std::string tmp = static_cast<std::string>(TempFileName(dbname, descriptor_number));
  IOStatus s = WriteStringToFile(fs, contents.ToString() + "\n", tmp, true);
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

Status SetIdentityFile(Env* env, const std::string& dbname,
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
  Status s = WriteStringToFile(env, id, tmp, true);
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
    Status temp_s = dir_obj->Close(IOOptions(), nullptr);
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

IOStatus SyncManifest(const ImmutableDBOptions* db_options,
                      WritableFileWriter* file) {
  TEST_KILL_RANDOM_WITH_WEIGHT("SyncManifest:0", REDUCE_ODDS2);
  StopWatch sw(db_options->clock, db_options->stats, MANIFEST_FILE_SYNC_MICROS);
  return file->Sync(db_options->use_fsync);
}

Status GetInfoLogFiles(const std::shared_ptr<FileSystem>& fs,
                       const std::string& db_log_dir, const std::string& dbname,
                       std::string* parent_dir,
                       std::vector<std::string>* info_log_list) {
  assert(parent_dir != nullptr);
  assert(info_log_list != nullptr);
  uint64_t number = 0;
  FileType type = kWalFile;

  if (!db_log_dir.empty()) {
    *parent_dir = db_log_dir;
  } else {
    *parent_dir = dbname;
  }

  InfoLogPrefix info_log_prefix = InfoLogPrefix_new(!db_log_dir.empty(), dbname);

  std::vector<std::string> file_names;
  Status s = fs->GetChildren(*parent_dir, IOOptions(), &file_names, nullptr);

  if (!s.ok()) {
    return s;
  }

  for (auto& f : file_names) {
    if (ParseFileName(f, &number, static_cast<std::string>(info_log_prefix.prefix), &type) &&
        (type == kInfoLogFile)) {
      info_log_list->push_back(f);
    }
  }
  return Status_OK();
}
}  // namespace rocksdb
