#[cxx::bridge(namespace = "rocksdb_rs::transaction_log")]
pub mod ffix {
    enum WalFileType {
        /// Indicates that WAL file is in archive directory. WAL files are moved from
        /// the main db directory to archive directory once they are not live and stay
        /// there until cleaned up. Files are cleaned depending on archive size
        /// (Options::WAL_size_limit_MB) and time since last cleaning
        /// (Options::WAL_ttl_seconds).
        kArchivedLogFile = 0,

        /// Indicates that WAL file is live and resides in the main db directory
        kAliveLogFile = 1,
    }
}
