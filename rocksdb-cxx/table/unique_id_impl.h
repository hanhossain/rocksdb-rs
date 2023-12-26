//  Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <array>

#include "rocksdb/unique_id.h"

#ifndef ROCKSDB_RS
#include "rocksdb-rs-cxx/unique_id.h"
#else
#include "rocksdb-rs/src/unique_id.rs.h"
#endif

namespace rocksdb {
// Helper for GetUniqueIdFromTableProperties. This function can also be used
// for temporary ids for files without sufficient information in table
// properties. The internal unique id is more structured than the public
// unique id, so can be manipulated in more ways but very carefully.
// These must be long term stable to ensure GetUniqueIdFromTableProperties
// is long term stable.
Status GetSstInternalUniqueId(const std::string &db_id,
                              const std::string &db_session_id,
                              uint64_t file_number, UniqueIdPtr out,
                              bool force = false);

// Helper for GetUniqueIdFromTableProperties. External unique ids go through
// this extra hashing layer so that prefixes of the unique id have predictable
// "full" entropy. This hashing layer is 1-to-1 on the first 128 bits and on
// the full 192 bits.
// This transformation must be long term stable to ensure
// GetUniqueIdFromTableProperties is long term stable.
void InternalUniqueIdToExternal(UniqueIdPtr in_out);

// Reverse of InternalUniqueIdToExternal mostly for testing purposes
// (demonstrably 1-to-1 on the first 128 bits and on the full 192 bits).
void ExternalUniqueIdToInternal(UniqueIdPtr in_out);

// Convert numerical format to byte format for public API
std::string EncodeUniqueIdBytes(UniqueIdPtr in);

// Reverse of EncodeUniqueIdBytes.
Status DecodeUniqueIdBytes(const std::string &unique_id, UniqueIdPtr out);

// Reformat a random value down to our "DB session id" format,
// which is intended to be compact and friendly for use in file names.
// `lower` is fully preserved and data is lost from `upper`.
//
// Detail: Encoded into 20 chars in base-36 ([0-9A-Z]), which is ~103 bits of
// entropy, which is enough to expect no collisions across a billion servers
// each opening DBs a million times (~2^50). Benefits vs. RFC-4122 unique id:
// * Save ~ dozen bytes per SST file
// * Shorter shared backup file names (some platforms have low limits)
// * Visually distinct from DB id format (usually RFC-4122)
std::string EncodeSessionId(uint64_t upper, uint64_t lower);

// Reverse of EncodeSessionId. Returns NotSupported on error rather than
// Corruption because non-standard session IDs should be allowed with degraded
// functionality.
Status DecodeSessionId(const std::string &db_session_id, uint64_t& upper,
                       uint64_t& lower);

}  // namespace rocksdb
