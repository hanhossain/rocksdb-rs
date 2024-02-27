#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
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
}
