#[cxx::bridge(namespace = "rocksdb_rs::types")]
pub mod ffi {
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
}
