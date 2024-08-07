//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Encoding independent of machine byte order:
// * Fixed-length numbers are encoded with least-significant byte first
//   (little endian, native order on Intel and others)
// * In addition we support variable length "varint" encoding
// * Strings are encoded prefixed by their length in varint format
//
// Some related functions are provided in coding_lean.h

#pragma once
#include <algorithm>
#include <string>

#include "port/port.h"
#include "rocksdb/slice.h"
#include "rocksdb-rs/src/coding.rs.h"
#include "rocksdb-rs/src/coding_lean.rs.h"

// Some processors does not allow unaligned access to memory
#if defined(__sparc)
#define PLATFORM_UNALIGNED_ACCESS_NOT_ALLOWED
#endif

namespace rocksdb {

// The maximum length of a varint in bytes for 64-bit.
const uint32_t kMaxVarint64Length = 10;

// Standard Put... routines append to a string
extern void PutVarint32(std::string* dst, uint32_t value);
extern void PutVarint32Varint32(std::string* dst, uint32_t value1,
                                uint32_t value2);
extern void PutVarint32Varint32Varint32(std::string* dst, uint32_t value1,
                                        uint32_t value2, uint32_t value3);
extern void PutVarint64(std::string* dst, uint64_t value);
extern void PutVarint64Varint64(std::string* dst, uint64_t value1,
                                uint64_t value2);
extern void PutVarint32Varint64(std::string* dst, uint32_t value1,
                                uint64_t value2);
extern void PutVarint32Varint32Varint64(std::string* dst, uint32_t value1,
                                        uint32_t value2, uint64_t value3);
extern void PutLengthPrefixedSlice(std::string* dst, const Slice& value);
extern void PutLengthPrefixedSliceParts(std::string* dst,
                                        const SliceParts& slice_parts);
extern void PutLengthPrefixedSlicePartsWithPadding(
    std::string* dst, const SliceParts& slice_parts, size_t pad_sz);

extern bool GetVarint32(Slice* input, uint32_t* value);
extern bool GetVarint64(Slice* input, uint64_t* value);
extern bool GetVarsignedint64(Slice* input, int64_t* value);
extern bool GetLengthPrefixedSlice(Slice* input, Slice* result);
// This function assumes data is well-formed.
extern Slice GetLengthPrefixedSlice(const char* data);

// Pointer-based variants of GetVarint...  These either store a value
// in *v and return a pointer just past the parsed value, or return
// nullptr on error.  These routines only look at bytes in the range
// [p..limit-1]
inline const char* GetVarsignedint64Ptr(const char* p, const char* limit,
                                        int64_t* value) {
  uint64_t u = 0;
  const char* ret = rocksdb_rs::coding::GetVarint64Ptr(p, limit, &u);
  *value = rocksdb_rs::coding::zigzagToI64(u);
  return ret;
}


// Lower-level versions of Put... that write directly into a character buffer
// and return a pointer just past the last byte written.
// REQUIRES: dst has enough space for the value being written
extern char* EncodeVarint64(char* dst, uint64_t value);

inline void PutVarint32(std::string* dst, uint32_t v) {
  char buf[5];
  char* ptr = rocksdb_rs::coding::EncodeVarint32(buf, v);
  dst->append(buf, static_cast<size_t>(ptr - buf));
}

inline void PutVarint32Varint32(std::string* dst, uint32_t v1, uint32_t v2) {
  char buf[10];
  char* ptr = rocksdb_rs::coding::EncodeVarint32(buf, v1);
  ptr = rocksdb_rs::coding::EncodeVarint32(ptr, v2);
  dst->append(buf, static_cast<size_t>(ptr - buf));
}

inline void PutVarint32Varint32Varint32(std::string* dst, uint32_t v1,
                                        uint32_t v2, uint32_t v3) {
  char buf[15];
  char* ptr = rocksdb_rs::coding::EncodeVarint32(buf, v1);
  ptr = rocksdb_rs::coding::EncodeVarint32(ptr, v2);
  ptr = rocksdb_rs::coding::EncodeVarint32(ptr, v3);
  dst->append(buf, static_cast<size_t>(ptr - buf));
}

inline char* EncodeVarint64(char* dst, uint64_t v) {
  static const unsigned int B = 128;
  unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
  while (v >= B) {
    *(ptr++) = (v & (B - 1)) | B;
    v >>= 7;
  }
  *(ptr++) = static_cast<unsigned char>(v);
  return reinterpret_cast<char*>(ptr);
}

inline void PutVarint64(std::string* dst, uint64_t v) {
  char buf[kMaxVarint64Length];
  char* ptr = EncodeVarint64(buf, v);
  dst->append(buf, static_cast<size_t>(ptr - buf));
}

inline void PutVarsignedint64(std::string* dst, int64_t v) {
  char buf[kMaxVarint64Length];
  // Using Zigzag format to convert signed to unsigned
  char* ptr = EncodeVarint64(buf, rocksdb_rs::coding::i64ToZigzag(v));
  dst->append(buf, static_cast<size_t>(ptr - buf));
}

inline void PutVarint64Varint64(std::string* dst, uint64_t v1, uint64_t v2) {
  char buf[20];
  char* ptr = EncodeVarint64(buf, v1);
  ptr = EncodeVarint64(ptr, v2);
  dst->append(buf, static_cast<size_t>(ptr - buf));
}

inline void PutVarint32Varint64(std::string* dst, uint32_t v1, uint64_t v2) {
  char buf[15];
  char* ptr = rocksdb_rs::coding::EncodeVarint32(buf, v1);
  ptr = EncodeVarint64(ptr, v2);
  dst->append(buf, static_cast<size_t>(ptr - buf));
}

inline void PutVarint32Varint32Varint64(std::string* dst, uint32_t v1,
                                        uint32_t v2, uint64_t v3) {
  char buf[20];
  char* ptr = rocksdb_rs::coding::EncodeVarint32(buf, v1);
  ptr = rocksdb_rs::coding::EncodeVarint32(ptr, v2);
  ptr = EncodeVarint64(ptr, v3);
  dst->append(buf, static_cast<size_t>(ptr - buf));
}

inline void PutLengthPrefixedSlice(std::string* dst, const Slice& value) {
  PutVarint32(dst, static_cast<uint32_t>(value.size()));
  dst->append(value.data(), value.size());
}

inline void PutLengthPrefixedSliceParts(std::string* dst, size_t total_bytes,
                                        const SliceParts& slice_parts) {
  for (int i = 0; i < slice_parts.num_parts; ++i) {
    total_bytes += slice_parts.parts[i].size();
  }
  PutVarint32(dst, static_cast<uint32_t>(total_bytes));
  for (int i = 0; i < slice_parts.num_parts; ++i) {
    dst->append(slice_parts.parts[i].data(), slice_parts.parts[i].size());
  }
}

inline void PutLengthPrefixedSliceParts(std::string* dst,
                                        const SliceParts& slice_parts) {
  PutLengthPrefixedSliceParts(dst, /*total_bytes=*/0, slice_parts);
}

inline void PutLengthPrefixedSlicePartsWithPadding(
    std::string* dst, const SliceParts& slice_parts, size_t pad_sz) {
  PutLengthPrefixedSliceParts(dst, /*total_bytes=*/pad_sz, slice_parts);
  dst->append(pad_sz, '\0');
}

inline bool GetVarint32(Slice* input, uint32_t* value) {
  const char* p = input->data();
  const char* limit = p + input->size();
  const char* q = rocksdb_rs::coding::GetVarint32Ptr(p, limit, value);
  if (q == nullptr) {
    return false;
  } else {
    *input = Slice(q, static_cast<size_t>(limit - q));
    return true;
  }
}

inline bool GetVarint64(Slice* input, uint64_t* value) {
  const char* p = input->data();
  const char* limit = p + input->size();
  const char* q = rocksdb_rs::coding::GetVarint64Ptr(p, limit, value);
  if (q == nullptr) {
    return false;
  } else {
    *input = Slice(q, static_cast<size_t>(limit - q));
    return true;
  }
}

inline bool GetVarsignedint64(Slice* input, int64_t* value) {
  const char* p = input->data();
  const char* limit = p + input->size();
  const char* q = GetVarsignedint64Ptr(p, limit, value);
  if (q == nullptr) {
    return false;
  } else {
    *input = Slice(q, static_cast<size_t>(limit - q));
    return true;
  }
}

inline bool GetLengthPrefixedSlice(Slice* input, Slice* result) {
  uint32_t len = 0;
  if (GetVarint32(input, &len) && input->size() >= len) {
    *result = Slice(input->data(), len);
    input->remove_prefix(len);
    return true;
  } else {
    return false;
  }
}

inline Slice GetLengthPrefixedSlice(const char* data) {
  uint32_t len = 0;
  // +5: we assume "data" is not corrupted
  // unsigned char is 7 bits, uint32_t is 32 bits, need 5 unsigned char
  auto p = rocksdb_rs::coding::GetVarint32Ptr(data, data + 5 /* limit */, &len);
  return Slice(p, len);
}

template <class T>
#ifdef ROCKSDB_UBSAN_RUN
#if defined(__clang__)
__attribute__((__no_sanitize__("alignment")))
#elif defined(__GNUC__)
__attribute__((__no_sanitize_undefined__))
#endif
#endif
inline void
PutUnaligned(T* memory, const T& value) {
#if defined(PLATFORM_UNALIGNED_ACCESS_NOT_ALLOWED)
  char* nonAlignedMemory = reinterpret_cast<char*>(memory);
  memcpy(nonAlignedMemory, reinterpret_cast<const char*>(&value), sizeof(T));
#else
  *memory = value;
#endif
}

template <class T>
#ifdef ROCKSDB_UBSAN_RUN
#if defined(__clang__)
__attribute__((__no_sanitize__("alignment")))
#elif defined(__GNUC__)
__attribute__((__no_sanitize_undefined__))
#endif
#endif
inline void
GetUnaligned(const T* memory, T* value) {
#if defined(PLATFORM_UNALIGNED_ACCESS_NOT_ALLOWED)
  char* nonAlignedMemory = reinterpret_cast<char*>(value);
  memcpy(nonAlignedMemory, reinterpret_cast<const char*>(memory), sizeof(T));
#else
  *value = *memory;
#endif
}

}  // namespace rocksdb
