//  Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include "rocksdb/table_properties.h"

#include "rocksdb-rs/src/unique_id.rs.h"

namespace rocksdb {

// Computes a stable, universally unique 128-bit (16 binary char) identifier
// for an SST file from TableProperties. This is supported for table (SST)
// files created with RocksDB 6.24 and later. NotSupported will be returned
// for other cases. The first 16 bytes (128 bits) is of sufficient quality
// for almost all applications, and shorter prefixes are usable as a
// hash of the full unique id.
//
// Note: .c_str() is not compatible with binary char strings, so using
// .c_str() on the result will often result in information loss and very
// poor uniqueness probability.
//
// More detail: the value is *guaranteed* unique for SST files
// generated in the same process (even different DBs, RocksDB >= 6.26),
// and first 128 bits are guaranteed not "all zeros" (RocksDB >= 6.26)
// so that the "all zeros" value can be used reliably for a null ID.
// These IDs are more than sufficient for SST uniqueness within each of
// many DBs or hosts. For an extreme example assuming random IDs, consider
// 10^9 hosts each with 10^9 live SST files being replaced at 10^6/second.
// Such a service would need to run for 10 million years to see an ID
// collision among live SST files on any host.
//
// And assuming one generates many SST files in the lifetime of each process,
// the probability of ID collisions is much "better than random"; see
// https://github.com/pdillinger/unique_id
rocksdb_rs::status::Status GetUniqueIdFromTableProperties(const TableProperties &props,
                                      std::string& out_id);

// Computes a 192-bit (24 binary char) stable, universally unique ID
// with an extra 64 bits of uniqueness compared to the standard ID. It is only
// appropriate to use this ID instead of the 128-bit ID if ID collisions
// between files among any hosts in a vast fleet is a problem, such as a shared
// global namespace for SST file backups. Under this criteria, the extreme
// example above would expect a global file ID collision every 4 days with
// 128-bit IDs (using some worst-case assumptions about process lifetime).
// It's 10^17 years with 192-bit IDs.
rocksdb_rs::status::Status GetExtendedUniqueIdFromTableProperties(const TableProperties &props,
                                              std::string& out_id);

}  // namespace rocksdb
