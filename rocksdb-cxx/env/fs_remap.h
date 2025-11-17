//  Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <utility>

#include "rocksdb/file_system.h"

namespace rocksdb {

// An abstract FileSystem wrapper that creates a view of an existing
// FileSystem by remapping names in some way.
//
// This class has not been fully analyzed for providing strong security
// guarantees.
class RemapFileSystem : public FileSystemWrapper {
 public:
  explicit RemapFileSystem(const std::shared_ptr<FileSystem>& base);

 protected:
  // Returns status and mapped-to path in the wrapped filesystem.
  // If it returns non-OK status, the returned path should not be used.
  virtual std::pair<rocksdb_rs::io_status::IOStatus, std::string> EncodePath(
      const std::string& path) = 0;

  // Similar to EncodePath() except used in cases in which it is OK for
  // no file or directory on 'path' to already exist, such as if the
  // operation would create one. However, the parent of 'path' is expected
  // to exist for the operation to succeed.
  // Default implementation: call EncodePath
  virtual std::pair<rocksdb_rs::io_status::IOStatus, std::string>
  EncodePathWithNewBasename(const std::string& path);

 public:
  // Left abstract:
  // const char* Name() const override { ... }
  static const char* kClassName() { return "RemapFileSystem"; }
  bool IsInstanceOf(const std::string& id) const override {
    if (id == kClassName()) {
      return true;
    } else {
      return FileSystemWrapper::IsInstanceOf(id);
    }
  }

  rocksdb_rs::status::Status RegisterDbPaths(
      const std::vector<std::string>& paths) override;

  rocksdb_rs::status::Status UnregisterDbPaths(
      const std::vector<std::string>& paths) override;

  rocksdb_rs::io_status::IOStatus NewSequentialFile(
      const std::string& fname, const FileOptions& options,
      std::unique_ptr<FSSequentialFile>* result, IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus NewRandomAccessFile(
      const std::string& fname, const FileOptions& options,
      std::unique_ptr<FSRandomAccessFile>* result,
      IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus NewWritableFile(
      const std::string& fname, const FileOptions& options,
      std::unique_ptr<FSWritableFile>* result, IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus ReuseWritableFile(
      const std::string& fname, const std::string& old_fname,
      const FileOptions& options, std::unique_ptr<FSWritableFile>* result,
      IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus NewRandomRWFile(
      const std::string& fname, const FileOptions& options,
      std::unique_ptr<FSRandomRWFile>* result, IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus NewDirectory(
      const std::string& dir, const IOOptions& options,
      std::unique_ptr<FSDirectory>* result, IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus FileExists(const std::string& fname,
                                             const IOOptions& options,
                                             IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus GetChildren(const std::string& dir,
                                              const IOOptions& options,
                                              std::vector<std::string>* result,
                                              IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus GetChildrenFileAttributes(
      const std::string& dir, const IOOptions& options,
      std::vector<FileAttributes>* result, IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus DeleteFile(const std::string& fname,
                                             const IOOptions& options,
                                             IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus CreateDir(const std::string& dirname,
                                            const IOOptions& options,
                                            IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus CreateDirIfMissing(
      const std::string& dirname, const IOOptions& options,
      IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus DeleteDir(const std::string& dirname,
                                            const IOOptions& options,
                                            IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus GetFileSize(const std::string& fname,
                                              const IOOptions& options,
                                              uint64_t* file_size,
                                              IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus GetFileModificationTime(
      const std::string& fname, const IOOptions& options, uint64_t* file_mtime,
      IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus IsDirectory(const std::string& path,
                                              const IOOptions& options,
                                              bool* is_dir,
                                              IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus RenameFile(const std::string& src,
                                             const std::string& dest,
                                             const IOOptions& options,
                                             IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus LinkFile(const std::string& src,
                                           const std::string& dest,
                                           const IOOptions& options,
                                           IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus LockFile(const std::string& fname,
                                           const IOOptions& options,
                                           FileLock** lock,
                                           IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus NewLogger(const std::string& fname,
                                            const IOOptions& options,
                                            std::shared_ptr<Logger>* result,
                                            IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus GetAbsolutePath(const std::string& db_path,
                                                  const IOOptions& options,
                                                  std::string* output_path,
                                                  IODebugContext* dbg) override;
};

}  // namespace rocksdb
