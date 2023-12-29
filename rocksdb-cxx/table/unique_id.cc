//  Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include <cstdint>
#include <rocksdb-rs/src/hash.rs.h>

#include "table/unique_id_impl.h"
#include "util/hash.h"
#include "util/string_util.h"

namespace rocksdb {

namespace {
// For InternalUniqueIdToExternal / ExternalUniqueIdToInternal we want all
// zeros in first 128 bits to map to itself, so that excluding zero in
// internal IDs (session_lower != 0 above) does the same for external IDs.
// These values are meaningless except for making that work.
constexpr uint64_t kHiOffsetForZero = 17391078804906429400U;
constexpr uint64_t kLoOffsetForZero = 6417269962128484497U;
}  // namespace

void InternalUniqueIdToExternal(UniqueIdPtr in_out) {
  uint64_t hi, lo;
  BijectiveHash2x64(in_out.ptr[1] + kHiOffsetForZero,
                    in_out.ptr[0] + kLoOffsetForZero, &hi, &lo);
  in_out.ptr[0] = lo;
  in_out.ptr[1] = hi;
  if (in_out.extended) {
    in_out.ptr[2] += lo + hi;
  }
}

void ExternalUniqueIdToInternal(UniqueIdPtr in_out) {
  uint64_t lo = in_out.ptr[0];
  uint64_t hi = in_out.ptr[1];
  if (in_out.extended) {
    in_out.ptr[2] -= lo + hi;
  }
  bijective_unhash2x64(hi, lo, hi, lo);
  in_out.ptr[0] = lo - kLoOffsetForZero;
  in_out.ptr[1] = hi - kHiOffsetForZero;
}

template <typename ID>
Status GetUniqueIdFromTablePropertiesHelper(const TableProperties &props,
                                            std::string& out_id) {
  ID tmp{};
  Status s = tmp.get_sst_internal_unique_id(props.db_id, props.db_session_id, props.orig_file_number, false);
  if (s.ok()) {
    InternalUniqueIdToExternal(tmp.as_unique_id_ptr());
    out_id = *tmp.encode_bytes();
  } else {
    out_id.clear();
  }
  return s;
}

Status GetExtendedUniqueIdFromTableProperties(const TableProperties &props,
                                              std::string& out_id) {
  return GetUniqueIdFromTablePropertiesHelper<UniqueId64x3>(props, out_id);
}

Status GetUniqueIdFromTableProperties(const TableProperties &props,
                                      std::string& out_id) {
  return GetUniqueIdFromTablePropertiesHelper<UniqueId64x2>(props, out_id);
}

}  // namespace rocksdb
