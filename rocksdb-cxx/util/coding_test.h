//
// Created by Han Hossain on 5/5/24.
//

#pragma once

#include "util/coding.h"
#include "test_util/testharness.h"

#include <iostream>

namespace rocksdb {
class Coding{};

inline void Coding_EncodingOutput() {
  std::string dst;
  rocksdb_rs::coding::PutFixed32(dst, 0x04030201);
  ASSERT_EQ(4U, dst.size());
  ASSERT_EQ(0x01, static_cast<int>(dst[0]));
  ASSERT_EQ(0x02, static_cast<int>(dst[1]));
  ASSERT_EQ(0x03, static_cast<int>(dst[2]));
  ASSERT_EQ(0x04, static_cast<int>(dst[3]));

  std::cerr << "hello world" << std::endl;

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

}
