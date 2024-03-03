//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <list>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <vector>

#include "rocksdb/env.h"
#include "rocksdb/trace_reader_writer.h"
#include "rocksdb/trace_record.h"
#include "rocksdb/write_batch.h"
#include "trace_replay/trace_replay.h"

namespace rocksdb {

// Value sizes may be used as denominators. Replacing 0 value sizes with this
// positive integer avoids division error.
extern const size_t kShadowValueSize /* = 10*/;

enum TraceOperationType : int {
  kGet = 0,
  kPut = 1,
  kDelete = 2,
  kSingleDelete = 3,
  kRangeDelete = 4,
  kMerge = 5,
  kIteratorSeek = 6,
  kIteratorSeekForPrev = 7,
  kMultiGet = 8,
  kPutEntity = 9,
  kTaTypeNum = 10
};

struct TraceUnit {
  uint64_t ts;
  uint32_t type;
  uint32_t cf_id;
  size_t value_size;
  std::string key;
};

struct TypeCorrelation {
  uint64_t count;
  uint64_t total_ts;
};

struct StatsUnit {
  uint64_t key_id;
  uint64_t access_count;
  uint64_t latest_ts;
  uint64_t succ_count;  // current only used to count Get if key found
  uint32_t cf_id;
  size_t value_size;
  std::vector<TypeCorrelation> v_correlation;
};

class AnalyzerOptions {
 public:
  std::vector<std::vector<int>> correlation_map;
  std::vector<std::pair<int, int>> correlation_list;

  AnalyzerOptions();

  ~AnalyzerOptions();

  void SparseCorrelationInput(const std::string& in_str);
};

// Note that, for the variable names  in the trace_analyzer,
// Starting with 'a_' means the variable is used for 'accessed_keys'.
// Starting with 'w_' means it is used for 'the whole key space'.
// Ending with '_f' means a file write or reader pointer.
// For example, 'a_count' means 'accessed_keys_count',
// 'w_key_f' means 'whole_key_space_file'.

struct TraceStats {
  uint32_t cf_id;
  std::string cf_name;
  uint64_t a_count;
  uint64_t a_succ_count;
  uint64_t a_key_id;
  uint64_t a_key_size_sqsum;
  uint64_t a_key_size_sum;
  uint64_t a_key_mid;
  uint64_t a_value_size_sqsum;
  uint64_t a_value_size_sum;
  uint64_t a_value_mid;
  uint32_t a_peak_qps;
  double a_ave_qps;
  std::map<std::string, StatsUnit> a_key_stats;
  std::map<uint64_t, uint64_t> a_count_stats;
  std::map<uint64_t, uint64_t> a_key_size_stats;
  std::map<uint64_t, uint64_t> a_value_size_stats;
  std::map<uint32_t, uint32_t> a_qps_stats;
  std::map<uint32_t, std::map<std::string, uint32_t>> a_qps_prefix_stats;
  std::priority_queue<std::pair<uint64_t, std::string>,
                      std::vector<std::pair<uint64_t, std::string>>,
                      std::greater<std::pair<uint64_t, std::string>>>
      top_k_queue;
  std::priority_queue<std::pair<uint64_t, std::string>,
                      std::vector<std::pair<uint64_t, std::string>>,
                      std::greater<std::pair<uint64_t, std::string>>>
      top_k_prefix_access;
  std::priority_queue<std::pair<double, std::string>,
                      std::vector<std::pair<double, std::string>>,
                      std::greater<std::pair<double, std::string>>>
      top_k_prefix_ave;
  std::priority_queue<std::pair<uint32_t, uint32_t>,
                      std::vector<std::pair<uint32_t, uint32_t>>,
                      std::greater<std::pair<uint32_t, uint32_t>>>
      top_k_qps_sec;
  std::list<TraceUnit> time_series;
  std::vector<std::pair<uint64_t, uint64_t>> correlation_output;
  std::map<uint32_t, uint64_t> uni_key_num;

  std::unique_ptr<rocksdb::WritableFile> time_series_f;
  std::unique_ptr<rocksdb::WritableFile> a_key_f;
  std::unique_ptr<rocksdb::WritableFile> a_count_dist_f;
  std::unique_ptr<rocksdb::WritableFile> a_prefix_cut_f;
  std::unique_ptr<rocksdb::WritableFile> a_value_size_f;
  std::unique_ptr<rocksdb::WritableFile> a_key_size_f;
  std::unique_ptr<rocksdb::WritableFile> a_key_num_f;
  std::unique_ptr<rocksdb::WritableFile> a_qps_f;
  std::unique_ptr<rocksdb::WritableFile> a_top_qps_prefix_f;
  std::unique_ptr<rocksdb::WritableFile> w_key_f;
  std::unique_ptr<rocksdb::WritableFile> w_prefix_cut_f;

  TraceStats();
  ~TraceStats();
  TraceStats(const TraceStats&) = delete;
  TraceStats& operator=(const TraceStats&) = delete;
  TraceStats(TraceStats&&) = default;
  TraceStats& operator=(TraceStats&&) = default;
};

struct TypeUnit {
  std::string type_name;
  bool enabled;
  uint64_t total_keys;
  uint64_t total_access;
  uint64_t total_succ_access;
  uint32_t sample_count;
  std::map<uint32_t, TraceStats> stats;
  TypeUnit() = default;
  ~TypeUnit() = default;
  TypeUnit(const TypeUnit&) = delete;
  TypeUnit& operator=(const TypeUnit&) = delete;
  TypeUnit(TypeUnit&&) = default;
  TypeUnit& operator=(TypeUnit&&) = default;
};

struct CfUnit {
  uint32_t cf_id;
  uint64_t w_count;  // total keys in this cf if we use the whole key space
  uint64_t a_count;  // the total keys in this cf that are accessed
  std::map<uint64_t, uint64_t> w_key_size_stats;  // whole key space key size
                                                  // statistic this cf
  std::map<uint32_t, uint32_t> cf_qps;
};

class TraceAnalyzer : private TraceRecord::Handler,
                      private WriteBatch::Handler {
 public:
  TraceAnalyzer(std::string& trace_path, std::string& output_path,
                AnalyzerOptions _analyzer_opts);
  ~TraceAnalyzer();

  rocksdb_rs::status::Status PrepareProcessing();

  rocksdb_rs::status::Status StartProcessing();

  rocksdb_rs::status::Status MakeStatistics();

  rocksdb_rs::status::Status ReProcessing();

  rocksdb_rs::status::Status EndProcessing();

  rocksdb_rs::status::Status WriteTraceUnit(TraceUnit& unit);

  std::vector<TypeUnit>& GetTaVector() { return ta_; }

 private:
  using TraceRecord::Handler::Handle;
  rocksdb_rs::status::Status Handle(const WriteQueryTraceRecord& record,
                std::unique_ptr<TraceRecordResult>* result) override;
  rocksdb_rs::status::Status Handle(const GetQueryTraceRecord& record,
                std::unique_ptr<TraceRecordResult>* result) override;
  rocksdb_rs::status::Status Handle(const IteratorSeekQueryTraceRecord& record,
                std::unique_ptr<TraceRecordResult>* result) override;
  rocksdb_rs::status::Status Handle(const MultiGetQueryTraceRecord& record,
                std::unique_ptr<TraceRecordResult>* result) override;

  using WriteBatch::Handler::PutCF;
  rocksdb_rs::status::Status PutCF(uint32_t column_family_id, const Slice& key,
               const Slice& value) override;

  using WriteBatch::Handler::PutEntityCF;
  rocksdb_rs::status::Status PutEntityCF(uint32_t column_family_id, const Slice& key,
                     const Slice& value) override;

  using WriteBatch::Handler::DeleteCF;
  rocksdb_rs::status::Status DeleteCF(uint32_t column_family_id, const Slice& key) override;

  using WriteBatch::Handler::SingleDeleteCF;
  rocksdb_rs::status::Status SingleDeleteCF(uint32_t column_family_id, const Slice& key) override;

  using WriteBatch::Handler::DeleteRangeCF;
  rocksdb_rs::status::Status DeleteRangeCF(uint32_t column_family_id, const Slice& begin_key,
                       const Slice& end_key) override;

  using WriteBatch::Handler::MergeCF;
  rocksdb_rs::status::Status MergeCF(uint32_t column_family_id, const Slice& key,
                 const Slice& value) override;

  // The following hanlders are not implemented, return Status_OK() to avoid
  // the running time assertion and other irrelevant falures.
  using WriteBatch::Handler::PutBlobIndexCF;
  rocksdb_rs::status::Status PutBlobIndexCF(uint32_t /*column_family_id*/, const Slice& /*key*/,
                        const Slice& /*value*/) override {
    return rocksdb_rs::status::Status_OK();
  }

  // The default implementation of LogData does nothing.
  using WriteBatch::Handler::LogData;
  void LogData(const Slice& /*blob*/) override {}

  using WriteBatch::Handler::MarkBeginPrepare;
  rocksdb_rs::status::Status MarkBeginPrepare(bool = false) override { return rocksdb_rs::status::Status_OK(); }

  using WriteBatch::Handler::MarkEndPrepare;
  rocksdb_rs::status::Status MarkEndPrepare(const Slice& /*xid*/) override { return rocksdb_rs::status::Status_OK(); }

  using WriteBatch::Handler::MarkNoop;
  rocksdb_rs::status::Status MarkNoop(bool /*empty_batch*/) override { return rocksdb_rs::status::Status_OK(); }

  using WriteBatch::Handler::MarkRollback;
  rocksdb_rs::status::Status MarkRollback(const Slice& /*xid*/) override { return rocksdb_rs::status::Status_OK(); }

  using WriteBatch::Handler::MarkCommit;
  rocksdb_rs::status::Status MarkCommit(const Slice& /*xid*/) override { return rocksdb_rs::status::Status_OK(); }

  using WriteBatch::Handler::MarkCommitWithTimestamp;
  rocksdb_rs::status::Status MarkCommitWithTimestamp(const Slice& /*xid*/,
                                 const Slice& /*commit_ts*/) override {
    return rocksdb_rs::status::Status_OK();
  }

  // Process each trace operation and output the analysis result to
  // stdout/files.
  rocksdb_rs::status::Status OutputAnalysisResult(TraceOperationType op_type, uint64_t timestamp,
                              std::vector<uint32_t> cf_ids,
                              std::vector<Slice> keys,
                              std::vector<size_t> value_sizes);

  rocksdb_rs::status::Status OutputAnalysisResult(TraceOperationType op_type, uint64_t timestamp,
                              uint32_t cf_id, const Slice& key,
                              size_t value_size);

  rocksdb::Env* env_;
  EnvOptions env_options_;
  std::unique_ptr<TraceReader> trace_reader_;
  size_t offset_;
  char buffer_[1024];
  // Timestamp of a WriteBatch, used in its iteration.
  uint64_t write_batch_ts_;
  std::string trace_name_;
  std::string output_path_;
  AnalyzerOptions analyzer_opts_;
  uint64_t total_requests_;
  uint64_t total_access_keys_;
  uint64_t total_gets_;
  uint64_t total_writes_;
  uint64_t total_seeks_;
  uint64_t total_seek_prevs_;
  uint64_t total_multigets_;
  uint64_t trace_create_time_;
  uint64_t begin_time_;
  uint64_t end_time_;
  uint64_t time_series_start_;
  uint32_t sample_max_;
  uint32_t cur_time_sec_;
  std::unique_ptr<rocksdb::WritableFile>
      trace_sequence_f_;                                    // readable trace
  std::unique_ptr<rocksdb::WritableFile> qps_f_;  // overall qps
  std::unique_ptr<rocksdb::WritableFile>
      cf_qps_f_;              // The qps of each CF>
  std::vector<TypeUnit> ta_;  // The main statistic collecting data structure
  std::map<uint32_t, CfUnit> cfs_;  // All the cf_id appears in this trace;
  std::vector<uint32_t> qps_peak_;
  std::vector<double> qps_ave_;

  rocksdb_rs::status::Status ReadTraceHeader(Trace* header);
  rocksdb_rs::status::Status ReadTraceFooter(Trace* footer);
  rocksdb_rs::status::Status ReadTraceRecord(Trace* trace);
  rocksdb_rs::status::Status KeyStatsInsertion(const uint32_t& type, const uint32_t& cf_id,
                           const std::string& key, const size_t value_size,
                           const uint64_t ts);
  rocksdb_rs::status::Status StatsUnitCorrelationUpdate(StatsUnit& unit, const uint32_t& type,
                                    const uint64_t& ts, const std::string& key);
  rocksdb_rs::status::Status OpenStatsOutputFiles(const std::string& type, TraceStats& new_stats);
  rocksdb_rs::status::Status CreateOutputFile(
      const std::string& type, const std::string& cf_name,
      const std::string& ending,
      std::unique_ptr<rocksdb::WritableFile>* f_ptr);
  rocksdb_rs::status::Status CloseOutputFiles();

  void PrintStatistics();
  rocksdb_rs::status::Status TraceUnitWriter(
      std::unique_ptr<rocksdb::WritableFile>& f_ptr, TraceUnit& unit);
  rocksdb_rs::status::Status WriteTraceSequence(const uint32_t& type, const uint32_t& cf_id,
                            const Slice& key, const size_t value_size,
                            const uint64_t ts);
  rocksdb_rs::status::Status MakeStatisticKeyStatsOrPrefix(TraceStats& stats);
  rocksdb_rs::status::Status MakeStatisticCorrelation(TraceStats& stats, StatsUnit& unit);
  rocksdb_rs::status::Status MakeStatisticQPS();
  int db_version_;
};

int trace_analyzer_tool(int argc, char** argv);

}  // namespace rocksdb

