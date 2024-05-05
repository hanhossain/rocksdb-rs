//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#pragma once
#include "util/coding.h"

#include "test_util/testharness.h"

namespace rocksdb {

// Test that encoding routines generate little-endian encodings
inline void Coding_EncodingOutput_Test() {
  std::string dst;
  rocksdb_rs::coding::PutFixed32(dst, 0x04030201);
  ASSERT_EQ(4U, dst.size());
  ASSERT_EQ(0x01, static_cast<int>(dst[0]));
  ASSERT_EQ(0x02, static_cast<int>(dst[1]));
  ASSERT_EQ(0x03, static_cast<int>(dst[2]));
  ASSERT_EQ(0x04, static_cast<int>(dst[3]));

  dst.clear();
  rocksdb_rs::coding::PutFixed64(dst, 0x0807060504030201ull);
  ASSERT_EQ(8U, dst.size());
  ASSERT_EQ(0x01, static_cast<int>(dst[0]));
  ASSERT_EQ(0x02, static_cast<int>(dst[1]));
  ASSERT_EQ(0x03, static_cast<int>(dst[2]));
  ASSERT_EQ(0x04, static_cast<int>(dst[3]));
  ASSERT_EQ(0x05, static_cast<int>(dst[4]));
  ASSERT_EQ(0x06, static_cast<int>(dst[5]));
  ASSERT_EQ(0x07, static_cast<int>(dst[6]));
  ASSERT_EQ(0x08, static_cast<int>(dst[7]));
}

inline void Coding_Varint32_Test() {
  std::string s;
  for (uint32_t i = 0; i < (32 * 32); i++) {
    uint32_t v = (i / 32) << (i % 32);
    PutVarint32(&s, v);
  }

  const char* p = s.data();
  const char* limit = p + s.size();
  for (uint32_t i = 0; i < (32 * 32); i++) {
    uint32_t expected = (i / 32) << (i % 32);
    uint32_t actual = 0;
    const char* start = p;
    p = rocksdb_rs::coding::GetVarint32Ptr(p, limit, &actual);
    ASSERT_TRUE(p != nullptr);
    ASSERT_EQ(expected, actual);
    ASSERT_EQ(rocksdb_rs::coding::VarintLength(actual), p - start);
  }
  ASSERT_EQ(p, s.data() + s.size());
}

inline void Coding_Varint64_Test() {
  // Construct the list of values to check
  std::vector<uint64_t> values;
  // Some special values
  values.push_back(0);
  values.push_back(100);
  values.push_back(~static_cast<uint64_t>(0));
  values.push_back(~static_cast<uint64_t>(0) - 1);
  for (uint32_t k = 0; k < 64; k++) {
    // Test values near powers of two
    const uint64_t power = 1ull << k;
    values.push_back(power);
    values.push_back(power - 1);
    values.push_back(power + 1);
  };

  std::string s;
  for (unsigned int i = 0; i < values.size(); i++) {
    PutVarint64(&s, values[i]);
  }

  const char* p = s.data();
  const char* limit = p + s.size();
  for (unsigned int i = 0; i < values.size(); i++) {
    ASSERT_TRUE(p < limit);
    uint64_t actual = 0;
    const char* start = p;
    p = rocksdb_rs::coding::GetVarint64Ptr(p, limit, &actual);
    ASSERT_TRUE(p != nullptr);
    ASSERT_EQ(values[i], actual);
    ASSERT_EQ(rocksdb_rs::coding::VarintLength(actual), p - start);
  }
  ASSERT_EQ(p, limit);
}

inline void Coding_Varint32Overflow_Test() {
  uint32_t result;
  std::string input("\x81\x82\x83\x84\x85\x11");
  ASSERT_TRUE(rocksdb_rs::coding::GetVarint32Ptr(input.data(), input.data() + input.size(),
                             &result) == nullptr);
}

inline void Coding_Varint32Truncation_Test() {
  uint32_t large_value = (1u << 31) + 100;
  std::string s;
  PutVarint32(&s, large_value);
  uint32_t result;
  for (unsigned int len = 0; len + 1 < s.size(); len++) {
    ASSERT_TRUE(rocksdb_rs::coding::GetVarint32Ptr(s.data(), s.data() + len, &result) == nullptr);
  }
  ASSERT_TRUE(rocksdb_rs::coding::GetVarint32Ptr(s.data(), s.data() + s.size(), &result) !=
              nullptr);
  ASSERT_EQ(large_value, result);
}

inline void Coding_Varint64Overflow_Test() {
  uint64_t result;
  std::string input("\x81\x82\x83\x84\x85\x81\x82\x83\x84\x85\x11");
  ASSERT_TRUE(rocksdb_rs::coding::GetVarint64Ptr(input.data(), input.data() + input.size(),
                             &result) == nullptr);
}

inline void Coding_Varint64Truncation_Test() {
  uint64_t large_value = (1ull << 63) + 100ull;
  std::string s;
  PutVarint64(&s, large_value);
  uint64_t result;
  for (unsigned int len = 0; len + 1 < s.size(); len++) {
    ASSERT_TRUE(rocksdb_rs::coding::GetVarint64Ptr(s.data(), s.data() + len, &result) == nullptr);
  }
  ASSERT_TRUE(rocksdb_rs::coding::GetVarint64Ptr(s.data(), s.data() + s.size(), &result) !=
              nullptr);
  ASSERT_EQ(large_value, result);
}

inline void Coding_Strings_Test() {
  std::string s;
  PutLengthPrefixedSlice(&s, Slice(""));
  PutLengthPrefixedSlice(&s, Slice("foo"));
  PutLengthPrefixedSlice(&s, Slice("bar"));
  PutLengthPrefixedSlice(&s, Slice(std::string(200, 'x')));

  Slice input(s);
  Slice v;
  ASSERT_TRUE(GetLengthPrefixedSlice(&input, &v));
  ASSERT_EQ("", v.ToString());
  ASSERT_TRUE(GetLengthPrefixedSlice(&input, &v));
  ASSERT_EQ("foo", v.ToString());
  ASSERT_TRUE(GetLengthPrefixedSlice(&input, &v));
  ASSERT_EQ("bar", v.ToString());
  ASSERT_TRUE(GetLengthPrefixedSlice(&input, &v));
  ASSERT_EQ(std::string(200, 'x'), v.ToString());
  ASSERT_EQ("", input.ToString());
}

}  // namespace rocksdb
