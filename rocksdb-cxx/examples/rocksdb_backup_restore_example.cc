// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include <cstdio>
#include <string>
#include <vector>

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/utilities/backup_engine.h"

using rocksdb::BackupEngine;
using rocksdb::BackupEngineOptions;
using rocksdb::BackupEngineReadOnly;
using rocksdb::BackupInfo;
using rocksdb::DB;
using rocksdb::Env;
using rocksdb::Options;
using rocksdb::ReadOptions;
using rocksdb_rs::status::Status;
using rocksdb::WriteOptions;

#if defined(OS_WIN)
std::string kDBPath = "C:\\Windows\\TEMP\\rocksdb_example";
#else
std::string kDBPath = "/tmp/rocksdb_example";
#endif

int main() {
  DB* db;
  Options options;
  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  // create the DB if it's not already present
  options.create_if_missing = true;

  // open DB
  rocksdb_rs::status::Status s = DB::Open(options, kDBPath, &db);
  assert(s.ok());

  // Put key-value
  db->Put(WriteOptions(), "key1", "value1");
  assert(s.ok());

  // create backup
  BackupEngine* backup_engine;
  s = BackupEngine::Open(Env::Default(),
                         BackupEngineOptions("/tmp/rocksdb_example_backup"),
                         &backup_engine);
  assert(s.ok());

  backup_engine->CreateNewBackup(db);
  assert(s.ok());

  std::vector<BackupInfo> backup_info;
  backup_engine->GetBackupInfo(&backup_info);

  s = backup_engine->VerifyBackup(1);
  assert(s.ok());

  // Put key-value
  db->Put(WriteOptions(), "key2", "value2");
  assert(s.ok());

  db->Close();
  delete db;
  db = nullptr;

  // restore db to backup 1
  BackupEngineReadOnly* backup_engine_ro;
  s = BackupEngineReadOnly::Open(
      Env::Default(), BackupEngineOptions("/tmp/rocksdb_example_backup"),
      &backup_engine_ro);
  assert(s.ok());

  s = backup_engine_ro->RestoreDBFromBackup(1, "/tmp/rocksdb_example",
                                            "/tmp/rocksdb_example");
  assert(s.ok());

  // open db again
  s = DB::Open(options, kDBPath, &db);
  assert(s.ok());

  std::string value;
  s = db->Get(ReadOptions(), "key1", &value);
  assert(!s.IsNotFound());

  s = db->Get(ReadOptions(), "key2", &value);
  assert(s.IsNotFound());

  delete backup_engine;
  delete backup_engine_ro;
  delete db;

  return 0;
}
