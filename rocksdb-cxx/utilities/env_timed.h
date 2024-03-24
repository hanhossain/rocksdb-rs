// Copyright (c) 2019-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//

#pragma once
#include "rocksdb/file_system.h"
namespace rocksdb {
class TimedFileSystem : public FileSystemWrapper {
 public:
  explicit TimedFileSystem(const std::shared_ptr<FileSystem>& base);

  static const char* kClassName() { return "TimedFS"; }
  const char* Name() const override { return kClassName(); }

  rocksdb_rs::io_status::IOStatus NewSequentialFile(const std::string& fname,
                             const FileOptions& options,
                             std::unique_ptr<FSSequentialFile>* result,
                             IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus NewRandomAccessFile(const std::string& fname,
                               const FileOptions& options,
                               std::unique_ptr<FSRandomAccessFile>* result,
                               IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus NewWritableFile(const std::string& fname, const FileOptions& options,
                           std::unique_ptr<FSWritableFile>* result,
                           IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus ReuseWritableFile(const std::string& fname,
                             const std::string& old_fname,
                             const FileOptions& options,
                             std::unique_ptr<FSWritableFile>* result,
                             IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus NewRandomRWFile(const std::string& fname, const FileOptions& options,
                           std::unique_ptr<FSRandomRWFile>* result,
                           IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus NewDirectory(const std::string& name, const IOOptions& options,
                        std::unique_ptr<FSDirectory>* result,
                        IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus FileExists(const std::string& fname, const IOOptions& options,
                      IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus GetChildren(const std::string& dir, const IOOptions& options,
                       std::vector<std::string>* result,
                       IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus GetChildrenFileAttributes(const std::string& dir,
                                     const IOOptions& options,
                                     std::vector<FileAttributes>* result,
                                     IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus DeleteFile(const std::string& fname, const IOOptions& options,
                      IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus CreateDir(const std::string& dirname, const IOOptions& options,
                     IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus CreateDirIfMissing(const std::string& dirname,
                              const IOOptions& options,
                              IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus DeleteDir(const std::string& dirname, const IOOptions& options,
                     IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus GetFileSize(const std::string& fname, const IOOptions& options,
                       uint64_t* file_size, IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus GetFileModificationTime(const std::string& fname,
                                   const IOOptions& options,
                                   uint64_t* file_mtime,
                                   IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus RenameFile(const std::string& src, const std::string& dst,
                      const IOOptions& options, IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus LinkFile(const std::string& src, const std::string& dst,
                    const IOOptions& options, IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus LockFile(const std::string& fname, const IOOptions& options,
                    FileLock** lock, IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus UnlockFile(FileLock* lock, const IOOptions& options,
                      IODebugContext* dbg) override;

  rocksdb_rs::io_status::IOStatus NewLogger(const std::string& fname, const IOOptions& options,
                     std::shared_ptr<Logger>* result,
                     IODebugContext* dbg) override;
};

}  // namespace rocksdb
