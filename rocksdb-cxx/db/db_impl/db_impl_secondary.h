//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once


#include <string>
#include <vector>

#include "db/db_impl/db_impl.h"
#include "logging/logging.h"

namespace rocksdb {

// A wrapper class to hold log reader, log reporter, log status.
class LogReaderContainer {
 public:
  LogReaderContainer()
      : reader_(nullptr), reporter_(nullptr), status_(nullptr) {}
  LogReaderContainer(Env* env, std::shared_ptr<Logger> info_log,
                     std::string fname,
                     std::unique_ptr<SequentialFileReader>&& file_reader,
                     uint64_t log_number) {
    status_ = std::make_unique<rocksdb_rs::status::Status>(rocksdb_rs::status::Status_new());
    LogReporter* reporter = new LogReporter(status_);
    reporter->env = env;
    reporter->info_log = info_log.get();
    reporter->fname = std::move(fname);
    reporter_ = reporter;
    // We intentially make log::Reader do checksumming even if
    // paranoid_checks==false so that corruptions cause entire commits
    // to be skipped instead of propagating bad information (like overly
    // large sequence numbers).
    reader_ = new log::FragmentBufferedReader(info_log, std::move(file_reader),
                                              reporter, true /*checksum*/,
                                              log_number);
  }
  log::FragmentBufferedReader* reader_;
  log::Reader::Reporter* reporter_;
  std::unique_ptr<rocksdb_rs::status::Status> status_;
  ~LogReaderContainer() {
    delete reader_;
    delete reporter_;
  }

 private:
  struct LogReporter : public log::Reader::Reporter {
    Env* env;
    Logger* info_log;
    std::string fname;
    std::unique_ptr<rocksdb_rs::status::Status> status;  // nullptr if immutable_db_options_.paranoid_checks==false

    LogReporter() : env(nullptr), info_log(nullptr), status(nullptr) {}
    LogReporter(const std::unique_ptr<rocksdb_rs::status::Status>& _status) : status(std::make_unique<rocksdb_rs::status::Status>(_status->Clone())) {}

    void Corruption(size_t bytes, const rocksdb_rs::status::Status& s) override {
      ROCKS_LOG_WARN(info_log, "%s%s: dropping %d bytes; %s",
                     (this->status == nullptr ? "(ignoring error) " : ""),
                     fname.c_str(), static_cast<int>(bytes),
                     s.ToString()->c_str());
      if (this->status != nullptr && this->status->ok()) {
        this->status->copy_from(s);
      }
    }
  };
};

// The secondary instance shares access to the storage as the primary.
// The secondary is able to read and replay changes described in both the
// MANIFEST and the WAL files without coordination with the primary.
// The secondary instance can be opened using `DB::OpenAsSecondary`. After
// that, it can call `DBImplSecondary::TryCatchUpWithPrimary` to make best
// effort attempts to catch up with the primary.
// TODO: Share common structure with CompactedDBImpl and DBImplReadOnly
class DBImplSecondary : public DBImpl {
 public:
  DBImplSecondary(const DBOptions& options, const std::string& dbname,
                  std::string secondary_path);
  ~DBImplSecondary() override;

  // Recover by replaying MANIFEST and WAL. Also initialize manifest_reader_
  // and log_readers_ to facilitate future operations.
  rocksdb_rs::status::Status Recover(const std::vector<ColumnFamilyDescriptor>& column_families,
                 bool read_only, bool error_if_wal_file_exists,
                 bool error_if_data_exists_in_wals, uint64_t* = nullptr,
                 RecoveryContext* recovery_ctx = nullptr) override;

  // Implementations of the DB interface.
  using DB::Get;
  // Can return IOError due to files being deleted by the primary. To avoid
  // IOError in this case, application can coordinate between primary and
  // secondaries so that primary will not delete files that are currently being
  // used by the secondaries. The application can also provide a custom FS/Env
  // implementation so that files will remain present until all primary and
  // secondaries indicate that they can be deleted. As a partial hacky
  // workaround, the secondaries can be opened with `max_open_files=-1` so that
  // it eagerly keeps all talbe files open and is able to access the contents of
  // deleted files via prior open fd.
  rocksdb_rs::status::Status Get(const ReadOptions& options, ColumnFamilyHandle* column_family,
             const Slice& key, PinnableSlice* value) override;

  rocksdb_rs::status::Status Get(const ReadOptions& options, ColumnFamilyHandle* column_family,
             const Slice& key, PinnableSlice* value,
             std::string* timestamp) override;

  rocksdb_rs::status::Status GetImpl(const ReadOptions& options, ColumnFamilyHandle* column_family,
                 const Slice& key, PinnableSlice* value,
                 std::string* timestamp);

  using DBImpl::NewIterator;
  // Operations on the created iterators can return IOError due to files being
  // deleted by the primary. To avoid IOError in this case, application can
  // coordinate between primary and secondaries so that primary will not delete
  // files that are currently being used by the secondaries. The application can
  // also provide a custom FS/Env implementation so that files will remain
  // present until all primary and secondaries indicate that they can be
  // deleted. As a partial hacky workaround, the secondaries can be opened with
  // `max_open_files=-1` so that it eagerly keeps all talbe files open and is
  // able to access the contents of deleted files via prior open fd.
  Iterator* NewIterator(const ReadOptions&,
                        ColumnFamilyHandle* column_family) override;

  ArenaWrappedDBIter* NewIteratorImpl(const ReadOptions& read_options,
                                      ColumnFamilyData* cfd,
                                      SequenceNumber snapshot,
                                      ReadCallback* read_callback,
                                      bool expose_blob_index = false,
                                      bool allow_refresh = true);

  rocksdb_rs::status::Status NewIterators(const ReadOptions& options,
                      const std::vector<ColumnFamilyHandle*>& column_families,
                      std::vector<Iterator*>* iterators) override;

  using DBImpl::Put;
  rocksdb_rs::status::Status Put(const WriteOptions& /*options*/,
             ColumnFamilyHandle* /*column_family*/, const Slice& /*key*/,
             const Slice& /*value*/) override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  using DBImpl::PutEntity;
  rocksdb_rs::status::Status PutEntity(const WriteOptions& /* options */,
                   ColumnFamilyHandle* /* column_family */,
                   const Slice& /* key */,
                   const WideColumns& /* columns */) override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  using DBImpl::Merge;
  rocksdb_rs::status::Status Merge(const WriteOptions& /*options*/,
               ColumnFamilyHandle* /*column_family*/, const Slice& /*key*/,
               const Slice& /*value*/) override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  using DBImpl::Delete;
  rocksdb_rs::status::Status Delete(const WriteOptions& /*options*/,
                ColumnFamilyHandle* /*column_family*/,
                const Slice& /*key*/) override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  using DBImpl::SingleDelete;
  rocksdb_rs::status::Status SingleDelete(const WriteOptions& /*options*/,
                      ColumnFamilyHandle* /*column_family*/,
                      const Slice& /*key*/) override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  rocksdb_rs::status::Status Write(const WriteOptions& /*options*/,
               WriteBatch* /*updates*/) override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  using DBImpl::CompactRange;
  rocksdb_rs::status::Status CompactRange(const CompactRangeOptions& /*options*/,
                      ColumnFamilyHandle* /*column_family*/,
                      const Slice* /*begin*/, const Slice* /*end*/) override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  using DBImpl::CompactFiles;
  rocksdb_rs::status::Status CompactFiles(
      const CompactionOptions& /*compact_options*/,
      ColumnFamilyHandle* /*column_family*/,
      const std::vector<std::string>& /*input_file_names*/,
      const int /*output_level*/, const int /*output_path_id*/ = -1,
      std::vector<std::string>* const /*output_file_names*/ = nullptr,
      CompactionJobInfo* /*compaction_job_info*/ = nullptr) override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  rocksdb_rs::status::Status DisableFileDeletions() override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  rocksdb_rs::status::Status EnableFileDeletions(bool /*force*/) override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  rocksdb_rs::status::Status GetLiveFiles(std::vector<std::string>&,
                      uint64_t* /*manifest_file_size*/,
                      bool /*flush_memtable*/ = true) override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  using DBImpl::Flush;
  rocksdb_rs::status::Status Flush(const FlushOptions& /*options*/,
               ColumnFamilyHandle* /*column_family*/) override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  using DBImpl::SetDBOptions;
  rocksdb_rs::status::Status SetDBOptions(const std::unordered_map<std::string, std::string>&
                      /*options_map*/) override {
    // Currently not supported because changing certain options may cause
    // flush/compaction.
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  using DBImpl::SetOptions;
  rocksdb_rs::status::Status SetOptions(
      ColumnFamilyHandle* /*cfd*/,
      const std::unordered_map<std::string, std::string>& /*options_map*/)
      override {
    // Currently not supported because changing certain options may cause
    // flush/compaction and/or write to MANIFEST.
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  using DBImpl::SyncWAL;
  rocksdb_rs::status::Status SyncWAL() override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  using DB::IngestExternalFile;
  rocksdb_rs::status::Status IngestExternalFile(
      ColumnFamilyHandle* /*column_family*/,
      const std::vector<std::string>& /*external_files*/,
      const IngestExternalFileOptions& /*ingestion_options*/) override {
    return Status_NotSupported("Not supported operation in secondary mode.");
  }

  // Try to catch up with the primary by reading as much as possible from the
  // log files until there is nothing more to read or encounters an error. If
  // the amount of information in the log files to process is huge, this
  // method can take long time due to all the I/O and CPU costs.
  rocksdb_rs::status::Status TryCatchUpWithPrimary() override;

  // Try to find log reader using log_number from log_readers_ map, initialize
  // if it doesn't exist
  rocksdb_rs::status::Status MaybeInitLogReader(uint64_t log_number,
                            log::FragmentBufferedReader** log_reader);

  // Check if all live files exist on file system and that their file sizes
  // matche to the in-memory records. It is possible that some live files may
  // have been deleted by the primary. In this case, CheckConsistency() does
  // not flag the missing file as inconsistency.
  rocksdb_rs::status::Status CheckConsistency() override;

#ifndef NDEBUG
  rocksdb_rs::status::Status TEST_CompactWithoutInstallation(const OpenAndCompactOptions& options,
                                         ColumnFamilyHandle* cfh,
                                         const CompactionServiceInput& input,
                                         CompactionServiceResult* result) {
    return CompactWithoutInstallation(options, cfh, input, result);
  }
#endif  // NDEBUG

 protected:
  rocksdb_rs::status::Status FlushForGetLiveFiles() override {
    // No-op for read-only DB
    return rocksdb_rs::status::Status_OK();
  }

  bool OwnTablesAndLogs() const override {
    // Currently, the secondary instance does not own the database files. It
    // simply opens the files of the primary instance and tracks their file
    // descriptors until they become obsolete. In the future, the secondary may
    // create links to database files. OwnTablesAndLogs will return true then.
    return false;
  }

 private:
  friend class DB;

  // No copying allowed
  DBImplSecondary(const DBImplSecondary&);
  void operator=(const DBImplSecondary&);

  using DBImpl::Recover;

  rocksdb_rs::status::Status FindAndRecoverLogFiles(
      std::unordered_set<ColumnFamilyData*>* cfds_changed,
      JobContext* job_context);
  rocksdb_rs::status::Status FindNewLogNumbers(std::vector<uint64_t>* logs);
  // After manifest recovery, replay WALs and refresh log_readers_ if necessary
  // REQUIRES: log_numbers are sorted in ascending order
  rocksdb_rs::status::Status RecoverLogFiles(const std::vector<uint64_t>& log_numbers,
                         SequenceNumber* next_sequence,
                         std::unordered_set<ColumnFamilyData*>* cfds_changed,
                         JobContext* job_context);

  // Run compaction without installation, the output files will be placed in the
  // secondary DB path. The LSM tree won't be changed, the secondary DB is still
  // in read-only mode.
  rocksdb_rs::status::Status CompactWithoutInstallation(const OpenAndCompactOptions& options,
                                    ColumnFamilyHandle* cfh,
                                    const CompactionServiceInput& input,
                                    CompactionServiceResult* result);

  std::unique_ptr<log::FragmentBufferedReader> manifest_reader_;
  std::unique_ptr<log::Reader::Reporter> manifest_reporter_;
  std::unique_ptr<rocksdb_rs::status::Status> manifest_reader_status_;

  // Cache log readers for each log number, used for continue WAL replay
  // after recovery
  std::map<uint64_t, std::unique_ptr<LogReaderContainer>> log_readers_;

  // Current WAL number replayed for each column family.
  std::unordered_map<ColumnFamilyData*, uint64_t> cfd_to_current_log_;

  const std::string secondary_path_;
};

}  // namespace rocksdb

