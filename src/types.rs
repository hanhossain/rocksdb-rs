#[cxx::bridge(namespace = "rocksdb_rs::types")]
pub mod ffix {
    /// The types of files RocksDB uses in a DB directory. (Available for advanced options.)
    enum FileType {
        kWalFile,
        kDBLockFile,
        kTableFile,
        kDescriptorFile,
        kCurrentFile,
        kTempFile,
        /// Either the current one, or an old one
        kInfoLogFile,
        kMetaDatabase,
        kIdentityFile,
        kOptionsFile,
        kBlobFile,
    }

    enum TableFileCreationReason {
        kFlush,
        kCompaction,
        kRecovery,
        kMisc,
    }

    enum BlobFileCreationReason {
        kFlush,
        kCompaction,
        kRecovery,
    }

    /// User-oriented representation of internal key types.
    /// Ordering of this enum entries should not change.
    enum EntryType {
        kEntryPut,
        kEntryDelete,
        kEntrySingleDelete,
        kEntryMerge,
        kEntryRangeDeletion,
        kEntryBlobIndex,
        kEntryDeleteWithTimestamp,
        kEntryWideColumnEntity,
        kEntryOther,
    }

    enum WriteStallCause {
        // Beginning of CF-scope write stall causes
        //
        // Always keep `kMemtableLimit` as the first stat in this section
        kMemtableLimit,
        kL0FileCountLimit,
        kPendingCompactionBytes,
        kCFScopeWriteStallCauseEnumMax,
        // End of CF-scope write stall causes

        // Beginning of DB-scope write stall causes
        //
        // Always keep `kWriteBufferManagerLimit` as the first stat in this section
        kWriteBufferManagerLimit,
        kDBScopeWriteStallCauseEnumMax,
        // End of DB-scope write stall causes

        // Always add new WriteStallCause before `kNone`
        kNone,
    }

    enum WriteStallCondition {
        kDelayed,
        kStopped,
        // Always add new WriteStallCondition before `kNormal`
        kNormal,
    }
}
