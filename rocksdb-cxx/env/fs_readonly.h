//  Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include "rocksdb/file_system.h"

namespace rocksdb {

// A FileSystem wrapper that only allows read-only operation.
//
// This class has not been fully analyzed for providing strong security
// guarantees.
class ReadOnlyFileSystem : public FileSystemWrapper {
  static inline rocksdb_rs::io_status::IOStatus FailReadOnly() {
    rocksdb_rs::io_status::IOStatus s = rocksdb_rs::io_status::IOStatus_IOError(
        "Attempted write to ReadOnlyFileSystem");
    assert(s.GetRetryable() == false);
    return s;
  }

 public:
  explicit ReadOnlyFileSystem(const std::shared_ptr<FileSystem>& base)
      : FileSystemWrapper(base) {}

  static const char* kClassName() { return "ReadOnlyFileSystem"; }
  const char* Name() const override { return kClassName(); }

  rocksdb_rs::io_status::IOStatus NewWritableFile(
      const std::string& /*fname*/, const FileOptions& /*options*/,
      std::unique_ptr<FSWritableFile>* /*result*/,
      IODebugContext* /*dbg*/) override {
    return FailReadOnly();
  }
  rocksdb_rs::io_status::IOStatus ReuseWritableFile(
      const std::string& /*fname*/, const std::string& /*old_fname*/,
      const FileOptions& /*options*/,
      std::unique_ptr<FSWritableFile>* /*result*/,
      IODebugContext* /*dbg*/) override {
    return FailReadOnly();
  }
  rocksdb_rs::io_status::IOStatus NewRandomRWFile(
      const std::string& /*fname*/, const FileOptions& /*options*/,
      std::unique_ptr<FSRandomRWFile>* /*result*/,
      IODebugContext* /*dbg*/) override {
    return FailReadOnly();
  }
  rocksdb_rs::io_status::IOStatus NewDirectory(
      const std::string& /*dir*/, const IOOptions& /*options*/,
      std::unique_ptr<FSDirectory>* /*result*/,
      IODebugContext* /*dbg*/) override {
    return FailReadOnly();
  }
  rocksdb_rs::io_status::IOStatus DeleteFile(const std::string& /*fname*/,
                                             const IOOptions& /*options*/,
                                             IODebugContext* /*dbg*/) override {
    return FailReadOnly();
  }
  rocksdb_rs::io_status::IOStatus CreateDir(const std::string& /*dirname*/,
                                            const IOOptions& /*options*/,
                                            IODebugContext* /*dbg*/) override {
    return FailReadOnly();
  }
  rocksdb_rs::io_status::IOStatus CreateDirIfMissing(
      const std::string& dirname, const IOOptions& options,
      IODebugContext* dbg) override {
    // Allow if dir already exists
    bool is_dir = false;
    rocksdb_rs::io_status::IOStatus s =
        IsDirectory(dirname, options, &is_dir, dbg);
    if (s.ok() && is_dir) {
      return s;
    } else {
      return FailReadOnly();
    }
  }
  rocksdb_rs::io_status::IOStatus DeleteDir(const std::string& /*dirname*/,
                                            const IOOptions& /*options*/,
                                            IODebugContext* /*dbg*/) override {
    return FailReadOnly();
  }
  rocksdb_rs::io_status::IOStatus RenameFile(const std::string& /*src*/,
                                             const std::string& /*dest*/,
                                             const IOOptions& /*options*/,
                                             IODebugContext* /*dbg*/) override {
    return FailReadOnly();
  }
  rocksdb_rs::io_status::IOStatus LinkFile(const std::string& /*src*/,
                                           const std::string& /*dest*/,
                                           const IOOptions& /*options*/,
                                           IODebugContext* /*dbg*/) override {
    return FailReadOnly();
  }
  rocksdb_rs::io_status::IOStatus LockFile(const std::string& /*fname*/,
                                           const IOOptions& /*options*/,
                                           FileLock** /*lock*/,
                                           IODebugContext* /*dbg*/) override {
    return FailReadOnly();
  }
  rocksdb_rs::io_status::IOStatus NewLogger(const std::string& /*fname*/,
                                            const IOOptions& /*options*/,
                                            std::shared_ptr<Logger>* /*result*/,
                                            IODebugContext* /*dbg*/) override {
    return FailReadOnly();
  }
};

}  // namespace rocksdb
