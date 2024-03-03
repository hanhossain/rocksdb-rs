// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
// Copyright (c) 2015, Red Hat, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.


#include "rocksdb/utilities/env_mirror.h"

namespace rocksdb {

// An implementation of Env that mirrors all work over two backend
// Env's.  This is useful for debugging purposes.
class SequentialFileMirror : public SequentialFile {
 public:
  std::unique_ptr<SequentialFile> a_, b_;
  std::string fname;
  explicit SequentialFileMirror(std::string f) : fname(f) {}

  rocksdb_rs::status::Status Read(size_t n, Slice* result, char* scratch) override {
    Slice aslice;
    rocksdb_rs::status::Status as = a_->Read(n, &aslice, scratch);
    if (as.eq(Status_OK())) {
      char* bscratch = new char[n];
      Slice bslice;
#ifndef NDEBUG
      size_t off = 0;
#endif
      size_t left = aslice.size();
      while (left) {
        rocksdb_rs::status::Status bs = b_->Read(left, &bslice, bscratch);
#ifndef NDEBUG
        assert(as.eq(bs));
        assert(memcmp(bscratch, scratch + off, bslice.size()) == 0);
        off += bslice.size();
#endif
        left -= bslice.size();
      }
      delete[] bscratch;
      *result = aslice;
    } else {
      rocksdb_rs::status::Status bs = b_->Read(n, result, scratch);
      assert(as.eq(bs));
    }
    return as;
  }

  rocksdb_rs::status::Status Skip(uint64_t n) override {
    rocksdb_rs::status::Status as = a_->Skip(n);
    rocksdb_rs::status::Status bs = b_->Skip(n);
    assert(as.eq(bs));
    return as;
  }
  rocksdb_rs::status::Status InvalidateCache(size_t offset, size_t length) override {
    rocksdb_rs::status::Status as = a_->InvalidateCache(offset, length);
    rocksdb_rs::status::Status bs = b_->InvalidateCache(offset, length);
    assert(as.eq(bs));
    return as;
  };
};

class RandomAccessFileMirror : public RandomAccessFile {
 public:
  std::unique_ptr<RandomAccessFile> a_, b_;
  std::string fname;
  explicit RandomAccessFileMirror(std::string f) : fname(f) {}

  rocksdb_rs::status::Status Read(uint64_t offset, size_t n, Slice* result,
              char* scratch) const override {
    rocksdb_rs::status::Status as = a_->Read(offset, n, result, scratch);
    if (as.eq(Status_OK())) {
      char* bscratch = new char[n];
      Slice bslice;
      size_t off = 0;
      size_t left = result->size();
      while (left) {
        rocksdb_rs::status::Status bs = b_->Read(offset + off, left, &bslice, bscratch);
        assert(as.eq(bs));
        assert(memcmp(bscratch, scratch + off, bslice.size()) == 0);
        off += bslice.size();
        left -= bslice.size();
      }
      delete[] bscratch;
    } else {
      rocksdb_rs::status::Status bs = b_->Read(offset, n, result, scratch);
      assert(as.eq(bs));
    }
    return as;
  }

  size_t GetUniqueId(char* id, size_t max_size) const override {
    // NOTE: not verified
    return a_->GetUniqueId(id, max_size);
  }
};

class WritableFileMirror : public WritableFile {
 public:
  std::unique_ptr<WritableFile> a_, b_;
  std::string fname;
  explicit WritableFileMirror(std::string f, const EnvOptions& options)
      : WritableFile(options), fname(f) {}

  rocksdb_rs::status::Status Append(const Slice& data) override {
    rocksdb_rs::status::Status as = a_->Append(data);
    rocksdb_rs::status::Status bs = b_->Append(data);
    assert(as.eq(bs));
    return as;
  }
  rocksdb_rs::status::Status Append(const Slice& data,
                const DataVerificationInfo& /* verification_info */) override {
    return Append(data);
  }
  rocksdb_rs::status::Status PositionedAppend(const Slice& data, uint64_t offset) override {
    rocksdb_rs::status::Status as = a_->PositionedAppend(data, offset);
    rocksdb_rs::status::Status bs = b_->PositionedAppend(data, offset);
    assert(as.eq(bs));
    return as;
  }
  rocksdb_rs::status::Status PositionedAppend(
      const Slice& data, uint64_t offset,
      const DataVerificationInfo& /* verification_info */) override {
    return PositionedAppend(data, offset);
  }
  rocksdb_rs::status::Status Truncate(uint64_t size) override {
    rocksdb_rs::status::Status as = a_->Truncate(size);
    rocksdb_rs::status::Status bs = b_->Truncate(size);
    assert(as.eq(bs));
    return as;
  }
  rocksdb_rs::status::Status Close() override {
    rocksdb_rs::status::Status as = a_->Close();
    rocksdb_rs::status::Status bs = b_->Close();
    assert(as.eq(bs));
    return as;
  }
  rocksdb_rs::status::Status Flush() override {
    rocksdb_rs::status::Status as = a_->Flush();
    rocksdb_rs::status::Status bs = b_->Flush();
    assert(as.eq(bs));
    return as;
  }
  rocksdb_rs::status::Status Sync() override {
    rocksdb_rs::status::Status as = a_->Sync();
    rocksdb_rs::status::Status bs = b_->Sync();
    assert(as.eq(bs));
    return as;
  }
  rocksdb_rs::status::Status Fsync() override {
    rocksdb_rs::status::Status as = a_->Fsync();
    rocksdb_rs::status::Status bs = b_->Fsync();
    assert(as.eq(bs));
    return as;
  }
  bool IsSyncThreadSafe() const override {
    bool as = a_->IsSyncThreadSafe();
    assert(as == b_->IsSyncThreadSafe());
    return as;
  }
  void SetIOPriority(Env::IOPriority pri) override {
    a_->SetIOPriority(pri);
    b_->SetIOPriority(pri);
  }
  Env::IOPriority GetIOPriority() override {
    // NOTE: we don't verify this one
    return a_->GetIOPriority();
  }
  uint64_t GetFileSize() override {
    uint64_t as = a_->GetFileSize();
    assert(as == b_->GetFileSize());
    return as;
  }
  void GetPreallocationStatus(size_t* block_size,
                              size_t* last_allocated_block) override {
    // NOTE: we don't verify this one
    return a_->GetPreallocationStatus(block_size, last_allocated_block);
  }
  size_t GetUniqueId(char* id, size_t max_size) const override {
    // NOTE: we don't verify this one
    return a_->GetUniqueId(id, max_size);
  }
  rocksdb_rs::status::Status InvalidateCache(size_t offset, size_t length) override {
    rocksdb_rs::status::Status as = a_->InvalidateCache(offset, length);
    rocksdb_rs::status::Status bs = b_->InvalidateCache(offset, length);
    assert(as.eq(bs));
    return as;
  }

 protected:
  rocksdb_rs::status::Status Allocate(uint64_t offset, uint64_t length) override {
    rocksdb_rs::status::Status as = a_->Allocate(offset, length);
    rocksdb_rs::status::Status bs = b_->Allocate(offset, length);
    assert(as.eq(bs));
    return as;
  }
  rocksdb_rs::status::Status RangeSync(uint64_t offset, uint64_t nbytes) override {
    rocksdb_rs::status::Status as = a_->RangeSync(offset, nbytes);
    rocksdb_rs::status::Status bs = b_->RangeSync(offset, nbytes);
    assert(as.eq(bs));
    return as;
  }
};

rocksdb_rs::status::Status EnvMirror::NewSequentialFile(const std::string& f,
                                    std::unique_ptr<SequentialFile>* r,
                                    const EnvOptions& options) {
  if (f.find("/proc/") == 0) {
    return a_->NewSequentialFile(f, r, options);
  }
  SequentialFileMirror* mf = new SequentialFileMirror(f);
  rocksdb_rs::status::Status as = a_->NewSequentialFile(f, &mf->a_, options);
  rocksdb_rs::status::Status bs = b_->NewSequentialFile(f, &mf->b_, options);
  assert(as.eq(bs));
  if (as.ok())
    r->reset(mf);
  else
    delete mf;
  return as;
}

rocksdb_rs::status::Status EnvMirror::NewRandomAccessFile(const std::string& f,
                                      std::unique_ptr<RandomAccessFile>* r,
                                      const EnvOptions& options) {
  if (f.find("/proc/") == 0) {
    return a_->NewRandomAccessFile(f, r, options);
  }
  RandomAccessFileMirror* mf = new RandomAccessFileMirror(f);
  rocksdb_rs::status::Status as = a_->NewRandomAccessFile(f, &mf->a_, options);
  rocksdb_rs::status::Status bs = b_->NewRandomAccessFile(f, &mf->b_, options);
  assert(as.eq(bs));
  if (as.ok())
    r->reset(mf);
  else
    delete mf;
  return as;
}

rocksdb_rs::status::Status EnvMirror::NewWritableFile(const std::string& f,
                                  std::unique_ptr<WritableFile>* r,
                                  const EnvOptions& options) {
  if (f.find("/proc/") == 0) return a_->NewWritableFile(f, r, options);
  WritableFileMirror* mf = new WritableFileMirror(f, options);
  rocksdb_rs::status::Status as = a_->NewWritableFile(f, &mf->a_, options);
  rocksdb_rs::status::Status bs = b_->NewWritableFile(f, &mf->b_, options);
  assert(as.eq(bs));
  if (as.ok())
    r->reset(mf);
  else
    delete mf;
  return as;
}

rocksdb_rs::status::Status EnvMirror::ReuseWritableFile(const std::string& fname,
                                    const std::string& old_fname,
                                    std::unique_ptr<WritableFile>* r,
                                    const EnvOptions& options) {
  if (fname.find("/proc/") == 0)
    return a_->ReuseWritableFile(fname, old_fname, r, options);
  WritableFileMirror* mf = new WritableFileMirror(fname, options);
  rocksdb_rs::status::Status as = a_->ReuseWritableFile(fname, old_fname, &mf->a_, options);
  rocksdb_rs::status::Status bs = b_->ReuseWritableFile(fname, old_fname, &mf->b_, options);
  assert(as.eq(bs));
  if (as.ok())
    r->reset(mf);
  else
    delete mf;
  return as;
}

}  // namespace rocksdb
