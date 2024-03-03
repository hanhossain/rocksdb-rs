//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright 2014 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

// This test uses a custom Env to keep track of the state of a filesystem as of
// the last "sync". It then checks for data loss errors by purposely dropping
// file data (or entire files) not protected by a "sync".

#include "utilities/fault_injection_env.h"

#include <functional>
#include <utility>

#include "util/random.h"
namespace rocksdb {

// Assume a filename, and not a directory name like "/foo/bar/"
std::string GetDirName(const std::string filename) {
  size_t found = filename.find_last_of("/\\");
  if (found == std::string::npos) {
    return "";
  } else {
    return filename.substr(0, found);
  }
}

// A basic file truncation function suitable for this test.
rocksdb_rs::status::Status Truncate(Env* env, const std::string& filename, uint64_t length) {
  std::unique_ptr<SequentialFile> orig_file;
  const EnvOptions options;
  rocksdb_rs::status::Status s = env->NewSequentialFile(filename, &orig_file, options);
  if (!s.ok()) {
    fprintf(stderr, "Cannot open file %s for truncation: %s\n",
            filename.c_str(), s.ToString()->c_str());
    return s;
  }

  std::unique_ptr<char[]> scratch(new char[length]);
  rocksdb::Slice result;
  s = orig_file->Read(length, &result, scratch.get());
#ifdef OS_WIN
  orig_file.reset();
#endif
  if (s.ok()) {
    std::string tmp_name = GetDirName(filename) + "/truncate.tmp";
    std::unique_ptr<WritableFile> tmp_file;
    s = env->NewWritableFile(tmp_name, &tmp_file, options);
    if (s.ok()) {
      s = tmp_file->Append(result);
      if (s.ok()) {
        s = env->RenameFile(tmp_name, filename);
      } else {
        fprintf(stderr, "Cannot rename file %s to %s: %s\n", tmp_name.c_str(),
                filename.c_str(), s.ToString()->c_str());
        env->DeleteFile(tmp_name);
      }
    }
  }
  if (!s.ok()) {
    fprintf(stderr, "Cannot truncate file %s: %s\n", filename.c_str(),
            s.ToString()->c_str());
  }

  return s;
}

// Trim the tailing "/" in the end of `str`
std::string TrimDirname(const std::string& str) {
  size_t found = str.find_last_not_of("/");
  if (found == std::string::npos) {
    return str;
  }
  return str.substr(0, found + 1);
}

// Return pair <parent directory name, file name> of a full path.
std::pair<std::string, std::string> GetDirAndName(const std::string& name) {
  std::string dirname = GetDirName(name);
  std::string fname = name.substr(dirname.size() + 1);
  return std::make_pair(dirname, fname);
}

rocksdb_rs::status::Status FileState::DropUnsyncedData(Env* env) const {
  ssize_t sync_pos = pos_at_last_sync_ == -1 ? 0 : pos_at_last_sync_;
  return Truncate(env, filename_, sync_pos);
}

rocksdb_rs::status::Status FileState::DropRandomUnsyncedData(Env* env, Random* rand) const {
  ssize_t sync_pos = pos_at_last_sync_ == -1 ? 0 : pos_at_last_sync_;
  assert(pos_ >= sync_pos);
  int range = static_cast<int>(pos_ - sync_pos);
  uint64_t truncated_size =
      static_cast<uint64_t>(sync_pos) + rand->Uniform(range);
  return Truncate(env, filename_, truncated_size);
}

rocksdb_rs::status::Status TestDirectory::Fsync() {
  if (!env_->IsFilesystemActive()) {
    return env_->GetError();
  }
  env_->SyncDir(dirname_);
  return dir_->Fsync();
}

rocksdb_rs::status::Status TestDirectory::Close() {
  if (!env_->IsFilesystemActive()) {
    return env_->GetError();
  }
  return dir_->Close();
}

TestRandomAccessFile::TestRandomAccessFile(
    std::unique_ptr<RandomAccessFile>&& target, FaultInjectionTestEnv* env)
    : target_(std::move(target)), env_(env) {
  assert(target_);
  assert(env_);
}

rocksdb_rs::status::Status TestRandomAccessFile::Read(uint64_t offset, size_t n, Slice* result,
                                  char* scratch) const {
  assert(env_);
  if (!env_->IsFilesystemActive()) {
    return env_->GetError();
  }

  assert(target_);
  return target_->Read(offset, n, result, scratch);
}

rocksdb_rs::status::Status TestRandomAccessFile::Prefetch(uint64_t offset, size_t n) {
  assert(env_);
  if (!env_->IsFilesystemActive()) {
    return env_->GetError();
  }

  assert(target_);
  return target_->Prefetch(offset, n);
}

rocksdb_rs::status::Status TestRandomAccessFile::MultiRead(ReadRequest* reqs, size_t num_reqs) {
  assert(env_);
  if (!env_->IsFilesystemActive()) {
    const rocksdb_rs::status::Status s = env_->GetError();

    assert(reqs);
    for (size_t i = 0; i < num_reqs; ++i) {
      reqs[i].status.copy_from(s);
    }

    return s.Clone();
  }

  assert(target_);
  return target_->MultiRead(reqs, num_reqs);
}

TestWritableFile::TestWritableFile(const std::string& fname,
                                   std::unique_ptr<WritableFile>&& f,
                                   FaultInjectionTestEnv* env)
    : state_(fname),
      target_(std::move(f)),
      writable_file_opened_(true),
      env_(env) {
  assert(target_ != nullptr);
  state_.pos_ = 0;
}

TestWritableFile::~TestWritableFile() {
  if (writable_file_opened_) {
    Close();
  }
}

rocksdb_rs::status::Status TestWritableFile::Append(const Slice& data) {
  if (!env_->IsFilesystemActive()) {
    return env_->GetError();
  }
  rocksdb_rs::status::Status s = target_->Append(data);
  if (s.ok()) {
    state_.pos_ += data.size();
    env_->WritableFileAppended(state_);
  }
  return s;
}

rocksdb_rs::status::Status TestWritableFile::Close() {
  writable_file_opened_ = false;
  rocksdb_rs::status::Status s = target_->Close();
  if (s.ok()) {
    env_->WritableFileClosed(state_);
  }
  return s;
}

rocksdb_rs::status::Status TestWritableFile::Flush() {
  rocksdb_rs::status::Status s = target_->Flush();
  if (s.ok() && env_->IsFilesystemActive()) {
    state_.pos_at_last_flush_ = state_.pos_;
  }
  return s;
}

rocksdb_rs::status::Status TestWritableFile::Sync() {
  if (!env_->IsFilesystemActive()) {
    return rocksdb_rs::status::Status_IOError("FaultInjectionTestEnv: not active");
  }
  // No need to actual sync.
  state_.pos_at_last_sync_ = state_.pos_;
  env_->WritableFileSynced(state_);
  return rocksdb_rs::status::Status_OK();
}

TestRandomRWFile::TestRandomRWFile(const std::string& /*fname*/,
                                   std::unique_ptr<RandomRWFile>&& f,
                                   FaultInjectionTestEnv* env)
    : target_(std::move(f)), file_opened_(true), env_(env) {
  assert(target_ != nullptr);
}

TestRandomRWFile::~TestRandomRWFile() {
  if (file_opened_) {
    Close();
  }
}

rocksdb_rs::status::Status TestRandomRWFile::Write(uint64_t offset, const Slice& data) {
  if (!env_->IsFilesystemActive()) {
    return env_->GetError();
  }
  return target_->Write(offset, data);
}

rocksdb_rs::status::Status TestRandomRWFile::Read(uint64_t offset, size_t n, Slice* result,
                              char* scratch) const {
  if (!env_->IsFilesystemActive()) {
    return env_->GetError();
  }
  return target_->Read(offset, n, result, scratch);
}

rocksdb_rs::status::Status TestRandomRWFile::Close() {
  file_opened_ = false;
  return target_->Close();
}

rocksdb_rs::status::Status TestRandomRWFile::Flush() {
  if (!env_->IsFilesystemActive()) {
    return env_->GetError();
  }
  return target_->Flush();
}

rocksdb_rs::status::Status TestRandomRWFile::Sync() {
  if (!env_->IsFilesystemActive()) {
    return env_->GetError();
  }
  return target_->Sync();
}

rocksdb_rs::status::Status FaultInjectionTestEnv::NewDirectory(const std::string& name,
                                           std::unique_ptr<Directory>* result) {
  std::unique_ptr<Directory> r;
  rocksdb_rs::status::Status s = target()->NewDirectory(name, &r);
  assert(s.ok());
  if (!s.ok()) {
    return s;
  }
  result->reset(new TestDirectory(this, TrimDirname(name), r.release()));
  return rocksdb_rs::status::Status_OK();
}

rocksdb_rs::status::Status FaultInjectionTestEnv::NewWritableFile(
    const std::string& fname, std::unique_ptr<WritableFile>* result,
    const EnvOptions& soptions) {
  if (!IsFilesystemActive()) {
    return GetError();
  }
  // Not allow overwriting files
  rocksdb_rs::status::Status s = target()->FileExists(fname);
  if (s.ok()) {
    return rocksdb_rs::status::Status_Corruption("File already exists.");
  } else if (!s.IsNotFound()) {
    assert(s.IsIOError());
    return s;
  }
  s = target()->NewWritableFile(fname, result, soptions);
  if (s.ok()) {
    result->reset(new TestWritableFile(fname, std::move(*result), this));
    // WritableFileWriter* file is opened
    // again then it will be truncated - so forget our saved state.
    UntrackFile(fname);
    MutexLock l(&mutex_);
    open_managed_files_.insert(fname);
    auto dir_and_name = GetDirAndName(fname);
    auto& list = dir_to_new_files_since_last_sync_[dir_and_name.first];
    list.insert(dir_and_name.second);
  }
  return s;
}

rocksdb_rs::status::Status FaultInjectionTestEnv::ReopenWritableFile(
    const std::string& fname, std::unique_ptr<WritableFile>* result,
    const EnvOptions& soptions) {
  if (!IsFilesystemActive()) {
    return GetError();
  }

  bool exists;
  rocksdb_rs::status::Status s = rocksdb_rs::status::Status_new();
  rocksdb_rs::status::Status exists_s = target()->FileExists(fname);
  if (exists_s.IsNotFound()) {
    exists = false;
  } else if (exists_s.ok()) {
    exists = true;
  } else {
    s.copy_from(exists_s);
    exists = false;
  }

  if (s.ok()) {
    s = target()->ReopenWritableFile(fname, result, soptions);
  }

  // Only track files we created. Files created outside of this
  // `FaultInjectionTestEnv` are not eligible for tracking/data dropping
  // (for example, they may contain data a previous db_stress run expects to
  // be recovered). This could be extended to track/drop data appended once
  // the file is under `FaultInjectionTestEnv`'s control.
  if (s.ok()) {
    bool should_track;
    {
      MutexLock l(&mutex_);
      if (db_file_state_.find(fname) != db_file_state_.end()) {
        // It was written by this `Env` earlier.
        assert(exists);
        should_track = true;
      } else if (!exists) {
        // It was created by this `Env` just now.
        should_track = true;
        open_managed_files_.insert(fname);
        auto dir_and_name = GetDirAndName(fname);
        auto& list = dir_to_new_files_since_last_sync_[dir_and_name.first];
        list.insert(dir_and_name.second);
      } else {
        should_track = false;
      }
    }
    if (should_track) {
      result->reset(new TestWritableFile(fname, std::move(*result), this));
    }
  }
  return s;
}

rocksdb_rs::status::Status FaultInjectionTestEnv::NewRandomRWFile(
    const std::string& fname, std::unique_ptr<RandomRWFile>* result,
    const EnvOptions& soptions) {
  if (!IsFilesystemActive()) {
    return GetError();
  }
  rocksdb_rs::status::Status s = target()->NewRandomRWFile(fname, result, soptions);
  if (s.ok()) {
    result->reset(new TestRandomRWFile(fname, std::move(*result), this));
    // WritableFileWriter* file is opened
    // again then it will be truncated - so forget our saved state.
    UntrackFile(fname);
    MutexLock l(&mutex_);
    open_managed_files_.insert(fname);
    auto dir_and_name = GetDirAndName(fname);
    auto& list = dir_to_new_files_since_last_sync_[dir_and_name.first];
    list.insert(dir_and_name.second);
  }
  return s;
}

rocksdb_rs::status::Status FaultInjectionTestEnv::NewRandomAccessFile(
    const std::string& fname, std::unique_ptr<RandomAccessFile>* result,
    const EnvOptions& soptions) {
  if (!IsFilesystemActive()) {
    return GetError();
  }

  assert(target());
  const rocksdb_rs::status::Status s = target()->NewRandomAccessFile(fname, result, soptions);
  if (!s.ok()) {
    return s.Clone();
  }

  assert(result);
  result->reset(new TestRandomAccessFile(std::move(*result), this));

  return rocksdb_rs::status::Status_OK();
}

rocksdb_rs::status::Status FaultInjectionTestEnv::DeleteFile(const std::string& f) {
  if (!IsFilesystemActive()) {
    return GetError();
  }
  rocksdb_rs::status::Status s = EnvWrapper::DeleteFile(f);
  if (s.ok()) {
    UntrackFile(f);
  }
  return s;
}

rocksdb_rs::status::Status FaultInjectionTestEnv::RenameFile(const std::string& s,
                                         const std::string& t) {
  if (!IsFilesystemActive()) {
    return GetError();
  }
  rocksdb_rs::status::Status ret = EnvWrapper::RenameFile(s, t);

  if (ret.ok()) {
    MutexLock l(&mutex_);
    if (db_file_state_.find(s) != db_file_state_.end()) {
      db_file_state_[t] = db_file_state_[s];
      db_file_state_.erase(s);
    }

    auto sdn = GetDirAndName(s);
    auto tdn = GetDirAndName(t);
    if (dir_to_new_files_since_last_sync_[sdn.first].erase(sdn.second) != 0) {
      auto& tlist = dir_to_new_files_since_last_sync_[tdn.first];
      assert(tlist.find(tdn.second) == tlist.end());
      tlist.insert(tdn.second);
    }
  }

  return ret;
}

rocksdb_rs::status::Status FaultInjectionTestEnv::LinkFile(const std::string& s,
                                       const std::string& t) {
  if (!IsFilesystemActive()) {
    return GetError();
  }
  rocksdb_rs::status::Status ret = EnvWrapper::LinkFile(s, t);

  if (ret.ok()) {
    MutexLock l(&mutex_);
    if (db_file_state_.find(s) != db_file_state_.end()) {
      db_file_state_[t] = db_file_state_[s];
    }

    auto sdn = GetDirAndName(s);
    auto tdn = GetDirAndName(t);
    if (dir_to_new_files_since_last_sync_[sdn.first].find(sdn.second) !=
        dir_to_new_files_since_last_sync_[sdn.first].end()) {
      auto& tlist = dir_to_new_files_since_last_sync_[tdn.first];
      assert(tlist.find(tdn.second) == tlist.end());
      tlist.insert(tdn.second);
    }
  }

  return ret;
}

void FaultInjectionTestEnv::WritableFileClosed(const FileState& state) {
  MutexLock l(&mutex_);
  if (open_managed_files_.find(state.filename_) != open_managed_files_.end()) {
    db_file_state_[state.filename_] = state;
    open_managed_files_.erase(state.filename_);
  }
}

void FaultInjectionTestEnv::WritableFileSynced(const FileState& state) {
  MutexLock l(&mutex_);
  if (open_managed_files_.find(state.filename_) != open_managed_files_.end()) {
    if (db_file_state_.find(state.filename_) == db_file_state_.end()) {
      db_file_state_.insert(std::make_pair(state.filename_, state));
    } else {
      db_file_state_[state.filename_] = state;
    }
  }
}

void FaultInjectionTestEnv::WritableFileAppended(const FileState& state) {
  MutexLock l(&mutex_);
  if (open_managed_files_.find(state.filename_) != open_managed_files_.end()) {
    if (db_file_state_.find(state.filename_) == db_file_state_.end()) {
      db_file_state_.insert(std::make_pair(state.filename_, state));
    } else {
      db_file_state_[state.filename_] = state;
    }
  }
}

// For every file that is not fully synced, make a call to `func` with
// FileState of the file as the parameter.
rocksdb_rs::status::Status FaultInjectionTestEnv::DropFileData(
    std::function<rocksdb_rs::status::Status(Env*, FileState)> func) {
  rocksdb_rs::status::Status s = rocksdb_rs::status::Status_new();
  MutexLock l(&mutex_);
  for (std::map<std::string, FileState>::const_iterator it =
           db_file_state_.begin();
       s.ok() && it != db_file_state_.end(); ++it) {
    const FileState& state = it->second;
    if (!state.IsFullySynced()) {
      s = func(target(), state);
    }
  }
  return s;
}

rocksdb_rs::status::Status FaultInjectionTestEnv::DropUnsyncedFileData() {
  return DropFileData([&](Env* env, const FileState& state) {
    return state.DropUnsyncedData(env);
  });
}

rocksdb_rs::status::Status FaultInjectionTestEnv::DropRandomUnsyncedFileData(Random* rnd) {
  return DropFileData([&](Env* env, const FileState& state) {
    return state.DropRandomUnsyncedData(env, rnd);
  });
}

rocksdb_rs::status::Status FaultInjectionTestEnv::DeleteFilesCreatedAfterLastDirSync() {
  // Because DeleteFile access this container make a copy to avoid deadlock
  std::map<std::string, std::set<std::string>> map_copy;
  {
    MutexLock l(&mutex_);
    map_copy.insert(dir_to_new_files_since_last_sync_.begin(),
                    dir_to_new_files_since_last_sync_.end());
  }

  for (auto& pair : map_copy) {
    for (std::string name : pair.second) {
      rocksdb_rs::status::Status s = DeleteFile(pair.first + "/" + name);
      if (!s.ok()) {
        return s;
      }
    }
  }
  return rocksdb_rs::status::Status_OK();
}
void FaultInjectionTestEnv::ResetState() {
  MutexLock l(&mutex_);
  db_file_state_.clear();
  dir_to_new_files_since_last_sync_.clear();
  SetFilesystemActiveNoLock(true);
}

void FaultInjectionTestEnv::UntrackFile(const std::string& f) {
  MutexLock l(&mutex_);
  auto dir_and_name = GetDirAndName(f);
  dir_to_new_files_since_last_sync_[dir_and_name.first].erase(
      dir_and_name.second);
  db_file_state_.erase(f);
  open_managed_files_.erase(f);
}
}  // namespace rocksdb
