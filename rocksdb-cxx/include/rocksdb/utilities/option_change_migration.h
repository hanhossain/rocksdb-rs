//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <string>

#include "rocksdb/options.h"

#ifndef ROCKSDB_RS
#include "rocksdb-rs-cxx/status.h"
#else
#include "rocksdb-rs/src/status.rs.h"
#endif

namespace rocksdb {
// Try to migrate DB created with old_opts to be use new_opts.
// Multiple column families is not supported.
// It is best-effort. No guarantee to succeed.
// A full compaction may be executed.
// If the target options use FIFO compaction, the FIFO condition might be
// sacrificed: for data migrated, data inserted later might be dropped
// earlier. This is to gurantee FIFO compaction won't drop all the
// migrated data to fit max_table_files_size.
Status OptionChangeMigration(std::string dbname, const Options& old_opts,
                             const Options& new_opts);
}  // namespace rocksdb
