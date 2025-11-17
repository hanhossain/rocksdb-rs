//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// File names used by DB code

#pragma once
#include <stdint.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "options/db_options.h"
#include "port/port.h"
#include "rocksdb-rs/src/filename.rs.h"
#include "rocksdb-rs/src/status.rs.h"
#include "rocksdb/file_system.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"
#include "rocksdb/transaction_log.h"

namespace rocksdb {

class Env;
class Directory;
class SystemClock;
class WritableFileWriter;

#ifdef OS_WIN
constexpr char kFilePathSeparator = '\\';
#else
constexpr char kFilePathSeparator = '/';
#endif

extern const std::string kCurrentFileName;  // = "CURRENT"

extern const std::string kOptionsFileNamePrefix;  // = "OPTIONS-"
extern const std::string kTempFileNameSuffix;     // = "dbtmp"

// Make the CURRENT file point to the descriptor file with the
// specified number. On its success and when dir_contains_current_file is not
// nullptr, the function will fsync the directory containing the CURRENT file
// when
extern rocksdb_rs::io_status::IOStatus SetCurrentFile(
    FileSystem* fs, const std::string& dbname, uint64_t descriptor_number,
    FSDirectory* dir_contains_current_file);

// Make the IDENTITY file for the db
extern rocksdb_rs::status::Status SetIdentityFile(
    Env* env, const std::string& dbname, const std::string& db_id = {});

// Sync manifest file `file`.
extern rocksdb_rs::io_status::IOStatus SyncManifest(
    const ImmutableDBOptions* db_options, WritableFileWriter* file);

// Return list of file names of info logs in `file_names`.
// The list only contains file name. The parent directory name is stored
// in `parent_dir`.
// `db_log_dir` should be the one as in options.db_log_dir
extern rocksdb_rs::status::Status GetInfoLogFiles(
    const std::shared_ptr<FileSystem>& fs, const std::string& db_log_dir,
    const std::string& dbname, std::string* parent_dir,
    std::vector<std::string>* file_names);
}  // namespace rocksdb
