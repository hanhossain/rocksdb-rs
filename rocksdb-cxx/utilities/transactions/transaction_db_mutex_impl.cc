//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).


#include "utilities/transactions/transaction_db_mutex_impl.h"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>

#include "rocksdb/utilities/transaction_db_mutex.h"

#include "rocksdb-rs/src/status.rs.h"

namespace rocksdb {

class TransactionDBMutexImpl : public TransactionDBMutex {
 public:
  TransactionDBMutexImpl() {}
  ~TransactionDBMutexImpl() override {}

  rocksdb_rs::status::Status Lock() override;

  rocksdb_rs::status::Status TryLockFor(int64_t timeout_time) override;

  void UnLock() override { mutex_.unlock(); }

  friend class TransactionDBCondVarImpl;

 private:
  std::mutex mutex_;
};

class TransactionDBCondVarImpl : public TransactionDBCondVar {
 public:
  TransactionDBCondVarImpl() {}
  ~TransactionDBCondVarImpl() override {}

  rocksdb_rs::status::Status Wait(std::shared_ptr<TransactionDBMutex> mutex) override;

  rocksdb_rs::status::Status WaitFor(std::shared_ptr<TransactionDBMutex> mutex,
                 int64_t timeout_time) override;

  void Notify() override { cv_.notify_one(); }

  void NotifyAll() override { cv_.notify_all(); }

 private:
  std::condition_variable cv_;
};

std::shared_ptr<TransactionDBMutex>
TransactionDBMutexFactoryImpl::AllocateMutex() {
  return std::shared_ptr<TransactionDBMutex>(new TransactionDBMutexImpl());
}

std::shared_ptr<TransactionDBCondVar>
TransactionDBMutexFactoryImpl::AllocateCondVar() {
  return std::shared_ptr<TransactionDBCondVar>(new TransactionDBCondVarImpl());
}

rocksdb_rs::status::Status TransactionDBMutexImpl::Lock() {
  mutex_.lock();
  return rocksdb_rs::status::Status_OK();
}

rocksdb_rs::status::Status TransactionDBMutexImpl::TryLockFor(int64_t timeout_time) {
  bool locked = true;

  if (timeout_time == 0) {
    locked = mutex_.try_lock();
  } else {
    // Previously, this code used a std::timed_mutex.  However, this was changed
    // due to known bugs in gcc versions < 4.9.
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=54562
    //
    // Since this mutex isn't held for long and only a single mutex is ever
    // held at a time, it is reasonable to ignore the lock timeout_time here
    // and only check it when waiting on the condition_variable.
    mutex_.lock();
  }

  if (!locked) {
    // timeout acquiring mutex
    return rocksdb_rs::status::Status_TimedOut(rocksdb_rs::status::SubCode::kMutexTimeout);
  }

  return rocksdb_rs::status::Status_OK();
}

rocksdb_rs::status::Status TransactionDBCondVarImpl::Wait(
    std::shared_ptr<TransactionDBMutex> mutex) {
  auto mutex_impl = reinterpret_cast<TransactionDBMutexImpl*>(mutex.get());

  std::unique_lock<std::mutex> lock(mutex_impl->mutex_, std::adopt_lock);
  cv_.wait(lock);

  // Make sure unique_lock doesn't unlock mutex when it destructs
  lock.release();

  return rocksdb_rs::status::Status_OK();
}

rocksdb_rs::status::Status TransactionDBCondVarImpl::WaitFor(
    std::shared_ptr<TransactionDBMutex> mutex, int64_t timeout_time) {
  rocksdb_rs::status::Status s = rocksdb_rs::status::Status_new();

  auto mutex_impl = reinterpret_cast<TransactionDBMutexImpl*>(mutex.get());
  std::unique_lock<std::mutex> lock(mutex_impl->mutex_, std::adopt_lock);

  if (timeout_time < 0) {
    // If timeout is negative, do not use a timeout
    cv_.wait(lock);
  } else {
    auto duration = std::chrono::microseconds(timeout_time);
    auto cv_status = cv_.wait_for(lock, duration);

    // Check if the wait stopped due to timing out.
    if (cv_status == std::cv_status::timeout) {
      s = rocksdb_rs::status::Status_TimedOut(rocksdb_rs::status::SubCode::kMutexTimeout);
    }
  }

  // Make sure unique_lock doesn't unlock mutex when it destructs
  lock.release();

  // CV was signaled, or we spuriously woke up (but didn't time out)
  return s;
}

}  // namespace rocksdb

