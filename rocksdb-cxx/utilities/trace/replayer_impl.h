//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "rocksdb-rs/src/status.rs.h"
#include "rocksdb/db.h"
#include "rocksdb/env.h"
#include "rocksdb/trace_reader_writer.h"
#include "rocksdb/trace_record.h"
#include "rocksdb/trace_record_result.h"
#include "rocksdb/utilities/replayer.h"
#include "trace_replay/trace_replay.h"

namespace rocksdb {

class ReplayerImpl : public Replayer {
 public:
  ReplayerImpl(DB* db, const std::vector<ColumnFamilyHandle*>& handles,
               std::unique_ptr<TraceReader>&& reader);
  ~ReplayerImpl() override;

  using Replayer::Prepare;
  rocksdb_rs::status::Status Prepare() override;

  using Replayer::Next;
  rocksdb_rs::status::Status Next(
      std::unique_ptr<TraceRecord>* record) override;

  using Replayer::Execute;
  rocksdb_rs::status::Status Execute(
      const std::unique_ptr<TraceRecord>& record,
      std::unique_ptr<TraceRecordResult>* result) override;

  using Replayer::Replay;
  rocksdb_rs::status::Status Replay(
      const ReplayOptions& options,
      const std::function<void(rocksdb_rs::status::Status,
                               std::unique_ptr<TraceRecordResult>&&)>&
          result_callback) override;

  using Replayer::GetHeaderTimestamp;
  uint64_t GetHeaderTimestamp() const override;

 private:
  rocksdb_rs::status::Status ReadHeader(Trace* header);
  rocksdb_rs::status::Status ReadTrace(Trace* trace);

  // Generic function to execute a Trace in a thread pool.
  static void BackgroundWork(void* arg);

  std::unique_ptr<TraceReader> trace_reader_;
  std::mutex mutex_;
  std::atomic<bool> prepared_;
  std::atomic<bool> trace_end_;
  uint64_t header_ts_;
  std::unique_ptr<TraceRecord::Handler> exec_handler_;
  Env* env_;
  // When reading the trace header, the trace file version can be parsed.
  // Replayer will use different decode method to get the trace content based
  // on different trace file version.
  int trace_file_version_;
};

// Arguments passed to BackgroundWork() for replaying in a thread pool.
struct ReplayerWorkerArg {
  Trace trace_entry;
  int trace_file_version;
  // Handler to execute TraceRecord.
  TraceRecord::Handler* handler;
  // Callback function to report the error status and the timestamp of the
  // TraceRecord (not the start/end timestamp of executing the TraceRecord).
  std::function<void(rocksdb_rs::status::Status, uint64_t)> error_cb;
  // Callback function to report the trace execution status and operation
  // execution status/result(s).
  std::function<void(rocksdb_rs::status::Status,
                     std::unique_ptr<TraceRecordResult>&&)>
      result_cb;
};

}  // namespace rocksdb
