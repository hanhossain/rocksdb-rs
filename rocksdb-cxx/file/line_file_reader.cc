//  Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include "file/line_file_reader.h"

#include <cstring>

#include "monitoring/iostats_context_imp.h"

namespace rocksdb {

rocksdb_rs::io_status::IOStatus LineFileReader::Create(const std::shared_ptr<FileSystem>& fs,
                                const std::string& fname,
                                const FileOptions& file_opts,
                                std::unique_ptr<LineFileReader>* reader,
                                IODebugContext* dbg,
                                RateLimiter* rate_limiter) {
  std::unique_ptr<FSSequentialFile> file;
  rocksdb_rs::io_status::IOStatus io_s = fs->NewSequentialFile(fname, file_opts, &file, dbg);
  if (io_s.ok()) {
    reader->reset(new LineFileReader(
        std::move(file), fname, nullptr,
        std::vector<std::shared_ptr<EventListener>>{}, rate_limiter));
  }
  return io_s;
}

bool LineFileReader::ReadLine(std::string* out,
                              Env::IOPriority rate_limiter_priority) {
  assert(out);
  if (!io_status_.ok()) {
    return false;
  }
  out->clear();
  for (;;) {
    // Look for line delimiter
    const char* found = static_cast<const char*>(
        std::memchr(buf_begin_, '\n', buf_end_ - buf_begin_));
    if (found) {
      size_t len = found - buf_begin_;
      out->append(buf_begin_, len);
      buf_begin_ += len + /*delim*/ 1;
      ++line_number_;
      return true;
    }
    if (at_eof_) {
      return false;
    }
    // else flush and reload buffer
    out->append(buf_begin_, buf_end_ - buf_begin_);
    Slice result;
    io_status_ =
        sfr_.Read(buf_.size(), &result, buf_.data(), rate_limiter_priority);
    IOSTATS_ADD(bytes_read, result.size());
    if (!io_status_.ok()) {
      return false;
    }
    if (result.size() != buf_.size()) {
      // The obscure way of indicating EOF
      at_eof_ = true;
    }
    buf_begin_ = result.data();
    buf_end_ = result.data() + result.size();
  }
}

}  // namespace rocksdb
