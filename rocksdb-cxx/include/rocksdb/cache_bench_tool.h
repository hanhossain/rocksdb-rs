// Copyright (c) 2013-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
#pragma once

#include "rocksdb/types.h"

#ifndef ROCKSDB_RS
#include "rocksdb-rs/src/status.rs.h"
#else
#include "rocksdb-rs/src/status.rs.h"
#endif

namespace rocksdb {

int cache_bench_tool(int argc, char** argv);
}  // namespace rocksdb
