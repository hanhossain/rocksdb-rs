//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include "db/blob/blob_log_writer.h"

#include <string>

#include "db/blob/blob_log_format.h"
#include "file/writable_file_writer.h"
#include "monitoring/statistics_impl.h"
#include "rocksdb/system_clock.h"
#include "test_util/sync_point.h"
#include "util/stop_watch.h"

namespace rocksdb {

BlobLogWriter::BlobLogWriter(std::unique_ptr<WritableFileWriter>&& dest,
                             SystemClock* clock, Statistics* statistics,
                             uint64_t log_number, bool use_fs, bool do_flush,
                             uint64_t boffset)
    : dest_(std::move(dest)),
      clock_(clock),
      statistics_(statistics),
      log_number_(log_number),
      block_offset_(boffset),
      use_fsync_(use_fs),
      do_flush_(do_flush),
      last_elem_type_(ElemType::kEtNone) {}

BlobLogWriter::~BlobLogWriter() = default;

rocksdb_rs::status::Status BlobLogWriter::Sync() {
  TEST_SYNC_POINT("BlobLogWriter::Sync");

  StopWatch sync_sw(clock_, statistics_, BLOB_DB_BLOB_FILE_SYNC_MICROS);
  rocksdb_rs::status::Status s = dest_->Sync(use_fsync_).status();
  RecordTick(statistics_, BLOB_DB_BLOB_FILE_SYNCED);
  return s;
}

rocksdb_rs::status::Status BlobLogWriter::WriteHeader(BlobLogHeader& header) {
  assert(block_offset_ == 0);
  assert(last_elem_type_ == ElemType::kEtNone);
  std::string str;
  header.EncodeTo(&str);

  rocksdb_rs::status::Status s = dest_->Append(Slice(str)).status();
  if (s.ok()) {
    block_offset_ += str.size();
    if (do_flush_) {
      s = dest_->Flush().status();
    }
  }
  last_elem_type_ = ElemType::kEtFileHdr;
  RecordTick(statistics_, BLOB_DB_BLOB_FILE_BYTES_WRITTEN,
             BlobLogHeader::kSize);
  return s;
}

rocksdb_rs::status::Status BlobLogWriter::AppendFooter(BlobLogFooter& footer,
                                   std::string* checksum_method,
                                   std::string* checksum_value) {
  assert(block_offset_ != 0);
  assert(last_elem_type_ == ElemType::kEtFileHdr || last_elem_type_ == ElemType::kEtRecord);

  std::string str;
  footer.EncodeTo(&str);

  rocksdb_rs::status::Status s = rocksdb_rs::status::Status_new();
  if (dest_->seen_error()) {
    return rocksdb_rs::status::Status_IOError("Seen Error. Skip closing.");
  } else {
    s = dest_->Append(Slice(str)).status();
    if (s.ok()) {
      block_offset_ += str.size();

      s = Sync();

      if (s.ok()) {
        s = dest_->Close().status();

        if (s.ok()) {
          assert(!!checksum_method == !!checksum_value);

          if (checksum_method) {
            assert(checksum_method->empty());

            std::string method = dest_->GetFileChecksumFuncName();
            if (method != kUnknownFileChecksumFuncName) {
              *checksum_method = std::move(method);
            }
          }
          if (checksum_value) {
            assert(checksum_value->empty());

            std::string value = dest_->GetFileChecksum();
            if (value != kUnknownFileChecksum) {
              *checksum_value = std::move(value);
            }
          }
        }
      }
    }

    dest_.reset();
  }

  last_elem_type_ = ElemType::kEtFileFooter;
  RecordTick(statistics_, BLOB_DB_BLOB_FILE_BYTES_WRITTEN,
             BlobLogFooter::kSize);
  return s;
}

rocksdb_rs::status::Status BlobLogWriter::AddRecord(const Slice& key, const Slice& val,
                                uint64_t expiration, uint64_t* key_offset,
                                uint64_t* blob_offset) {
  assert(block_offset_ != 0);
  assert(last_elem_type_ == ElemType::kEtFileHdr || last_elem_type_ == ElemType::kEtRecord);

  std::string buf;
  ConstructBlobHeader(&buf, key, val, expiration);

  rocksdb_rs::status::Status s = EmitPhysicalRecord(buf, key, val, key_offset, blob_offset);
  return s;
}

rocksdb_rs::status::Status BlobLogWriter::AddRecord(const Slice& key, const Slice& val,
                                uint64_t* key_offset, uint64_t* blob_offset) {
  assert(block_offset_ != 0);
  assert(last_elem_type_ == ElemType::kEtFileHdr || last_elem_type_ == ElemType::kEtRecord);

  std::string buf;
  ConstructBlobHeader(&buf, key, val, 0);

  rocksdb_rs::status::Status s = EmitPhysicalRecord(buf, key, val, key_offset, blob_offset);
  return s;
}

void BlobLogWriter::ConstructBlobHeader(std::string* buf, const Slice& key,
                                        const Slice& val, uint64_t expiration) {
  BlobLogRecord record;
  record.key = key;
  record.value = val;
  record.expiration = expiration;
  record.EncodeHeaderTo(buf);
}

rocksdb_rs::status::Status BlobLogWriter::EmitPhysicalRecord(const std::string& headerbuf,
                                         const Slice& key, const Slice& val,
                                         uint64_t* key_offset,
                                         uint64_t* blob_offset) {
  StopWatch write_sw(clock_, statistics_, BLOB_DB_BLOB_FILE_WRITE_MICROS);
  rocksdb_rs::status::Status s = dest_->Append(Slice(headerbuf)).status();
  if (s.ok()) {
    s = dest_->Append(key).status();
  }
  if (s.ok()) {
    s = dest_->Append(val).status();
  }
  if (do_flush_ && s.ok()) {
    s = dest_->Flush().status();
  }

  *key_offset = block_offset_ + BlobLogRecord::kHeaderSize;
  *blob_offset = *key_offset + key.size();
  block_offset_ = *blob_offset + val.size();
  last_elem_type_ = ElemType::kEtRecord;
  RecordTick(statistics_, BLOB_DB_BLOB_FILE_BYTES_WRITTEN,
             BlobLogRecord::kHeaderSize + key.size() + val.size());
  return s;
}

}  // namespace rocksdb
