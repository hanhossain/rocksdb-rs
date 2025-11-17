//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//

#include "db/blob/blob_log_format.h"

#include "util/coding.h"
#include "util/crc32c.h"

namespace rocksdb {

void BlobLogHeader::EncodeTo(std::string* dst) {
  assert(dst != nullptr);
  dst->clear();
  dst->reserve(BlobLogHeader::kSize);
  rocksdb_rs::coding::PutFixed32(*dst, kMagicNumber);
  rocksdb_rs::coding::PutFixed32(*dst, version);
  rocksdb_rs::coding::PutFixed32(*dst, column_family_id);
  unsigned char flags = (has_ttl ? 1 : 0);
  dst->push_back(flags);
  dst->push_back(static_cast<uint8_t>(compression));
  rocksdb_rs::coding::PutFixed64(*dst, expiration_range.first);
  rocksdb_rs::coding::PutFixed64(*dst, expiration_range.second);
}

rocksdb_rs::status::Status BlobLogHeader::DecodeFrom(Slice src) {
  const char* kErrorMessage = "Error while decoding blob log header";
  if (src.size() != BlobLogHeader::kSize) {
    return rocksdb_rs::status::Status_Corruption(
        kErrorMessage, "Unexpected blob file header size");
  }
  uint32_t magic_number;
  unsigned char flags;
  if (!rocksdb_rs::coding::GetFixed32(src, magic_number) ||
      !rocksdb_rs::coding::GetFixed32(src, version) ||
      !rocksdb_rs::coding::GetFixed32(src, column_family_id)) {
    return rocksdb_rs::status::Status_Corruption(
        kErrorMessage,
        "Error decoding magic number, version and column family id");
  }
  if (magic_number != kMagicNumber) {
    return rocksdb_rs::status::Status_Corruption(kErrorMessage,
                                                 "Magic number mismatch");
  }
  if (version != kVersion1) {
    return rocksdb_rs::status::Status_Corruption(kErrorMessage,
                                                 "Unknown header version");
  }
  flags = src.data()[0];
  compression =
      static_cast<rocksdb_rs::compression_type::CompressionType>(src.data()[1]);
  has_ttl = (flags & 1) == 1;
  src.remove_prefix(2);
  if (!rocksdb_rs::coding::GetFixed64(src, expiration_range.first) ||
      !rocksdb_rs::coding::GetFixed64(src, expiration_range.second)) {
    return rocksdb_rs::status::Status_Corruption(
        kErrorMessage, "Error decoding expiration range");
  }
  return rocksdb_rs::status::Status_OK();
}

void BlobLogFooter::EncodeTo(std::string* dst) {
  assert(dst != nullptr);
  dst->clear();
  dst->reserve(BlobLogFooter::kSize);
  rocksdb_rs::coding::PutFixed32(*dst, kMagicNumber);
  rocksdb_rs::coding::PutFixed64(*dst, blob_count);
  rocksdb_rs::coding::PutFixed64(*dst, expiration_range.first);
  rocksdb_rs::coding::PutFixed64(*dst, expiration_range.second);
  crc = crc32c::Value(dst->c_str(), dst->size());
  crc = crc32c::Mask(crc);
  rocksdb_rs::coding::PutFixed32(*dst, crc);
}

rocksdb_rs::status::Status BlobLogFooter::DecodeFrom(Slice src) {
  const char* kErrorMessage = "Error while decoding blob log footer";
  if (src.size() != BlobLogFooter::kSize) {
    return rocksdb_rs::status::Status_Corruption(
        kErrorMessage, "Unexpected blob file footer size");
  }
  uint32_t src_crc = 0;
  src_crc = crc32c::Value(src.data(), BlobLogFooter::kSize - sizeof(uint32_t));
  src_crc = crc32c::Mask(src_crc);
  uint32_t magic_number = 0;
  if (!rocksdb_rs::coding::GetFixed32(src, magic_number) ||
      !rocksdb_rs::coding::GetFixed64(src, blob_count) ||
      !rocksdb_rs::coding::GetFixed64(src, expiration_range.first) ||
      !rocksdb_rs::coding::GetFixed64(src, expiration_range.second) ||
      !rocksdb_rs::coding::GetFixed32(src, crc)) {
    return rocksdb_rs::status::Status_Corruption(kErrorMessage,
                                                 "Error decoding content");
  }
  if (magic_number != kMagicNumber) {
    return rocksdb_rs::status::Status_Corruption(kErrorMessage,
                                                 "Magic number mismatch");
  }
  if (src_crc != crc) {
    return rocksdb_rs::status::Status_Corruption(kErrorMessage, "CRC mismatch");
  }
  return rocksdb_rs::status::Status_OK();
}

void BlobLogRecord::EncodeHeaderTo(std::string* dst) {
  assert(dst != nullptr);
  dst->clear();
  dst->reserve(BlobLogRecord::kHeaderSize + key.size() + value.size());
  rocksdb_rs::coding::PutFixed64(*dst, key.size());
  rocksdb_rs::coding::PutFixed64(*dst, value.size());
  rocksdb_rs::coding::PutFixed64(*dst, expiration);
  header_crc = crc32c::Value(dst->c_str(), dst->size());
  header_crc = crc32c::Mask(header_crc);
  rocksdb_rs::coding::PutFixed32(*dst, header_crc);
  blob_crc = crc32c::Value(key.data(), key.size());
  blob_crc = crc32c::Extend(blob_crc, value.data(), value.size());
  blob_crc = crc32c::Mask(blob_crc);
  rocksdb_rs::coding::PutFixed32(*dst, blob_crc);
}

rocksdb_rs::status::Status BlobLogRecord::DecodeHeaderFrom(Slice src) {
  const char* kErrorMessage = "Error while decoding blob record";
  if (src.size() != BlobLogRecord::kHeaderSize) {
    return rocksdb_rs::status::Status_Corruption(
        kErrorMessage, "Unexpected blob record header size");
  }
  uint32_t src_crc = 0;
  src_crc = crc32c::Value(src.data(), BlobLogRecord::kHeaderSize - 8);
  src_crc = crc32c::Mask(src_crc);
  if (!rocksdb_rs::coding::GetFixed64(src, key_size) ||
      !rocksdb_rs::coding::GetFixed64(src, value_size) ||
      !rocksdb_rs::coding::GetFixed64(src, expiration) ||
      !rocksdb_rs::coding::GetFixed32(src, header_crc) ||
      !rocksdb_rs::coding::GetFixed32(src, blob_crc)) {
    return rocksdb_rs::status::Status_Corruption(kErrorMessage,
                                                 "Error decoding content");
  }
  if (src_crc != header_crc) {
    return rocksdb_rs::status::Status_Corruption(kErrorMessage,
                                                 "Header CRC mismatch");
  }
  return rocksdb_rs::status::Status_OK();
}

rocksdb_rs::status::Status BlobLogRecord::CheckBlobCRC() const {
  uint32_t expected_crc = 0;
  expected_crc = crc32c::Value(key.data(), key.size());
  expected_crc = crc32c::Extend(expected_crc, value.data(), value.size());
  expected_crc = crc32c::Mask(expected_crc);
  if (expected_crc != blob_crc) {
    return rocksdb_rs::status::Status_Corruption("Blob CRC mismatch");
  }
  return rocksdb_rs::status::Status_OK();
}

}  // namespace rocksdb
