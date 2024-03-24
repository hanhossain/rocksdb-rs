//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <atomic>
#include <fstream>

#include "monitoring/instrumented_mutex.h"
#include "port/lang.h"
#include "rocksdb/file_system.h"
#include "rocksdb/options.h"
#include "rocksdb/trace_record.h"
#include "trace_replay/trace_replay.h"

namespace rocksdb {
class SystemClock;
class TraceReader;
class TraceWriter;

/* In order to log new data in trace record for specified operations, do
   following:
   1. Add new data in IOTraceOP (say kIONewData= 3)
   2. Log it in IOTraceWriter::WriteIOOp, and read that in
   IOTraceReader::ReadIOOp and
   IOTraceRecordParser::PrintHumanReadableIOTraceRecord in the switch case.
   3. In the FileSystemTracer APIs where this data will be logged with, update
   io_op_data |= (1 << IOTraceOp::kIONewData).
*/
enum IOTraceOp : char {
  // The value of each enum represents the bitwise position for
  // IOTraceRecord.io_op_data.
  kIOFileSize = 0,
  kIOLen = 1,
  kIOOffset = 2,
};

struct IOTraceRecord {
  // Required fields for all accesses.
  uint64_t access_timestamp = 0;
  TraceType trace_type = TraceType::kTraceMax;
  // Each bit in io_op_data stores which corresponding info from IOTraceOp will
  // be added in the trace. Foreg, if bit at position 1 is set then
  // IOTraceOp::kIOLen (length) will be logged in the record.
  uint64_t io_op_data = 0;
  std::string file_operation;
  uint64_t latency = 0;
  std::string io_status;
  // Stores file name instead of full path.
  std::string file_name;

  // Fields added to record based on IO operation.
  uint64_t len = 0;
  uint64_t offset = 0;
  uint64_t file_size = 0;

  // Additional information passed in IODebugContext.
  uint64_t trace_data = 0;
  std::string request_id;

  IOTraceRecord() {}

  IOTraceRecord(const uint64_t& _access_timestamp, const TraceType& _trace_type,
                const uint64_t& _io_op_data, const std::string& _file_operation,
                const uint64_t& _latency, const std::string& _io_status,
                const std::string& _file_name, const uint64_t& _file_size = 0)
      : access_timestamp(_access_timestamp),
        trace_type(_trace_type),
        io_op_data(_io_op_data),
        file_operation(_file_operation),
        latency(_latency),
        io_status(_io_status),
        file_name(_file_name),
        file_size(_file_size) {}

  IOTraceRecord(const uint64_t& _access_timestamp, const TraceType& _trace_type,
                const uint64_t& _io_op_data, const std::string& _file_operation,
                const uint64_t& _latency, const std::unique_ptr<std::string>& _io_status,
                const std::string& _file_name, const uint64_t& _file_size = 0)
                  : IOTraceRecord(_access_timestamp, _trace_type, _io_op_data, _file_operation,
                                  _latency, *_io_status, _file_name, _file_size) {}

  IOTraceRecord(const uint64_t& _access_timestamp, const TraceType& _trace_type,
                const uint64_t& _io_op_data, const std::string& _file_operation,
                const uint64_t& _latency, const std::string& _io_status,
                const std::string& _file_name, const uint64_t& _len,
                const uint64_t& _offset)
      : access_timestamp(_access_timestamp),
        trace_type(_trace_type),
        io_op_data(_io_op_data),
        file_operation(_file_operation),
        latency(_latency),
        io_status(_io_status),
        file_name(_file_name),
        len(_len),
        offset(_offset) {}

  IOTraceRecord(const uint64_t& _access_timestamp, const TraceType& _trace_type,
                const uint64_t& _io_op_data, const std::string& _file_operation,
                const uint64_t& _latency, const std::unique_ptr<std::string>& _io_status,
                const std::string& _file_name, const uint64_t& _len,
                const uint64_t& _offset)
                  : IOTraceRecord(_access_timestamp, _trace_type, _io_op_data, _file_operation,
                                  _latency, *_io_status, _file_name, _len, _offset) {}
};

struct IOTraceHeader {
  uint64_t start_time;
  uint32_t rocksdb_major_version;
  uint32_t rocksdb_minor_version;
};

// IOTraceWriter writes IO operation as a single trace. Each trace will have a
// timestamp and type, followed by the trace payload.
class IOTraceWriter {
 public:
  IOTraceWriter(SystemClock* clock, const TraceOptions& trace_options,
                std::unique_ptr<TraceWriter>&& trace_writer);
  ~IOTraceWriter() = default;
  // No copy and move.
  IOTraceWriter(const IOTraceWriter&) = delete;
  IOTraceWriter& operator=(const IOTraceWriter&) = delete;
  IOTraceWriter(IOTraceWriter&&) = delete;
  IOTraceWriter& operator=(IOTraceWriter&&) = delete;

  rocksdb_rs::status::Status WriteIOOp(const IOTraceRecord& record, IODebugContext* dbg);

  // Write a trace header at the beginning, typically on initiating a trace,
  // with some metadata like a magic number and RocksDB version.
  rocksdb_rs::status::Status WriteHeader();

 private:
  SystemClock* clock_;
  TraceOptions trace_options_;
  std::unique_ptr<TraceWriter> trace_writer_;
};

// IOTraceReader helps read the trace file generated by IOTraceWriter.
class IOTraceReader {
 public:
  explicit IOTraceReader(std::unique_ptr<TraceReader>&& reader);
  ~IOTraceReader() = default;
  // No copy and move.
  IOTraceReader(const IOTraceReader&) = delete;
  IOTraceReader& operator=(const IOTraceReader&) = delete;
  IOTraceReader(IOTraceReader&&) = delete;
  IOTraceReader& operator=(IOTraceReader&&) = delete;

  rocksdb_rs::status::Status ReadHeader(IOTraceHeader* header);

  rocksdb_rs::status::Status ReadIOOp(IOTraceRecord* record);

 private:
  std::unique_ptr<TraceReader> trace_reader_;
};

// An IO tracer. It uses IOTraceWriter to write the access record to the
// trace file.
class IOTracer {
 public:
  IOTracer();
  ~IOTracer();
  // No copy and move.
  IOTracer(const IOTracer&) = delete;
  IOTracer& operator=(const IOTracer&) = delete;
  IOTracer(IOTracer&&) = delete;
  IOTracer& operator=(IOTracer&&) = delete;

  // no_sanitize is added for tracing_enabled. writer_ is protected under mutex
  // so even if user call Start/EndIOTrace and tracing_enabled is not updated in
  // the meanwhile, WriteIOOp will anyways check the writer_ protected under
  // mutex and ignore the operation if writer_is null. So its ok if
  // tracing_enabled shows non updated value.

  // Start writing IO operations to the trace_writer.
  TSAN_SUPPRESSION rocksdb_rs::status::Status
  StartIOTrace(SystemClock* clock, const TraceOptions& trace_options,
               std::unique_ptr<TraceWriter>&& trace_writer);

  // Stop writing IO operations to the trace_writer.
  TSAN_SUPPRESSION void EndIOTrace();

  TSAN_SUPPRESSION bool is_tracing_enabled() const { return tracing_enabled; }

  void WriteIOOp(const IOTraceRecord& record, IODebugContext* dbg);

 private:
  TraceOptions trace_options_;
  // A mutex protects the writer_.
  InstrumentedMutex trace_writer_mutex_;
  std::atomic<IOTraceWriter*> writer_;
  // bool tracing_enabled is added to avoid costly operation of checking atomic
  // variable 'writer_' is nullptr or not in is_tracing_enabled().
  // is_tracing_enabled() is invoked multiple times by FileSystem classes.
  bool tracing_enabled;
};

}  // namespace rocksdb
