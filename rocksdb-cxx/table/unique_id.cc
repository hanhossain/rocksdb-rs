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

template <typename ID>
rocksdb_rs::status::Status GetUniqueIdFromTablePropertiesHelper(const TableProperties &props,
                                            std::string& out_id) {
  ID tmp{};
  rocksdb_rs::status::Status s = tmp.get_sst_internal_unique_id(props.db_id, props.db_session_id, props.orig_file_number, false);
  if (s.ok()) {
    auto tmp_ptr = tmp.as_unique_id_ptr();
    rocksdb_rs::unique_id::InternalUniqueIdToExternal(tmp_ptr);
    out_id = *tmp.encode_bytes();
  } else {
    out_id.clear();
  }
  return s;
}

rocksdb_rs::status::Status GetExtendedUniqueIdFromTableProperties(const TableProperties &props,
                                              std::string& out_id) {
  return GetUniqueIdFromTablePropertiesHelper<rocksdb_rs::unique_id::UniqueId64x3>(props, out_id);
}

rocksdb_rs::status::Status GetUniqueIdFromTableProperties(const TableProperties &props,
                                      std::string& out_id) {
  return GetUniqueIdFromTablePropertiesHelper<rocksdb_rs::unique_id::UniqueId64x2>(props, out_id);
}

}  // namespace rocksdb
