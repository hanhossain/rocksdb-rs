use crate::cache::ffi::CacheEntryRole;

#[cxx::bridge(namespace = "rocksdb")]
pub mod ffi {
    /// Classifications of block cache entries.
    ///
    /// Developer notes: Adding a new enum to this class requires corresponding
    /// updates to `kCacheEntryRoleToCamelString` and
    /// `kCacheEntryRoleToHyphenString`. Do not add to this enum after `kMisc` since
    /// `kNumCacheEntryRoles` assumes `kMisc` comes last.
    enum CacheEntryRole {
        /// Block-based table data block
        kDataBlock,
        /// Block-based table filter block (full or partitioned)
        kFilterBlock,
        /// Block-based table metadata block for partitioned filter
        kFilterMetaBlock,
        /// OBSOLETE / DEPRECATED: old/removed block-based filter
        kDeprecatedFilterBlock,
        /// Block-based table index block
        kIndexBlock,
        /// Other kinds of block-based table block
        kOtherBlock,
        /// WriteBufferManager's charge to account for its memtable usage
        kWriteBuffer,
        /// Compression dictionary building buffer's charge to account for
        /// its memory usage
        kCompressionDictionaryBuildingBuffer,
        /// Filter's charge to account for
        /// (new) bloom and ribbon filter construction's memory usage
        kFilterConstruction,
        /// BlockBasedTableReader's charge to account for its memory usage
        kBlockBasedTableReader,
        /// FileMetadata's charge to account for its memory usage
        kFileMetadata,
        /// Blob value (when using the same cache as block cache and blob cache)
        kBlobValue,
        /// Blob cache's charge to account for its memory usage (when using a
        /// separate block cache and blob cache)
        kBlobCache,
        /// Default bucket, for miscellaneous cache entries. Do not use for
        /// entries that could potentially add up to large usage.
        kMisc,
    }

    extern "Rust" {
        #[cxx_name = "BlockCacheEntryStatsMapKeys_CacheId"]
        fn block_cache_entry_stats_map_keys_cache_id() -> &'static str;

        #[cxx_name = "BlockCacheEntryStatsMapKeys_CacheCapacityBytes"]
        fn block_cache_entry_stats_map_keys_cache_capacity_bytes() -> &'static str;

        #[cxx_name = "BlockCacheEntryStatsMapKeys_LastCollectionDurationSeconds"]
        fn block_cache_entry_stats_map_keys_last_collection_duration_seconds() -> &'static str;

        #[cxx_name = "BlockCacheEntryStatsMapKeys_LastCollectionAgeSeconds"]
        fn block_cache_entry_stats_map_keys_last_collection_age_seconds() -> &'static str;

        #[cxx_name = "BlockCacheEntryStatsMapKeys_EntryCount"]
        fn block_cache_entry_stats_map_keys_entry_count(role: CacheEntryRole) -> String;

        #[cxx_name = "BlockCacheEntryStatsMapKeys_UsedBytes"]
        fn block_cache_entry_stats_map_keys_used_bytes(role: CacheEntryRole) -> String;

        #[cxx_name = "BlockCacheEntryStatsMapKeys_UsedPercent"]
        fn block_cache_entry_stats_map_keys_used_percent(role: CacheEntryRole) -> String;

        #[cxx_name = "CacheEntryRole_ToCamelString"]
        fn cache_entry_role_to_camel_str(role: CacheEntryRole) -> &'static str;
    }
}

impl CacheEntryRole {
    /// Obtain a hyphen-separated, lowercase name of a `CacheEntryRole`.
    fn to_hyphen_str(self) -> &'static str {
        match self {
            CacheEntryRole::kDataBlock => "data-block",
            CacheEntryRole::kFilterBlock => "filter-block",
            CacheEntryRole::kFilterMetaBlock => "filter-meta-block",
            CacheEntryRole::kDeprecatedFilterBlock => "deprecated-filter-block",
            CacheEntryRole::kIndexBlock => "index-block",
            CacheEntryRole::kOtherBlock => "other-block",
            CacheEntryRole::kWriteBuffer => "write-buffer",
            CacheEntryRole::kCompressionDictionaryBuildingBuffer => {
                "compression-dictionary-building-buffer"
            }
            CacheEntryRole::kFilterConstruction => "filter-construction",
            CacheEntryRole::kBlockBasedTableReader => "block-based-table-reader",
            CacheEntryRole::kFileMetadata => "file-metadata",
            CacheEntryRole::kBlobValue => "blob-value",
            CacheEntryRole::kBlobCache => "blob-cache",
            CacheEntryRole::kMisc => "misc",
            _ => unreachable!(),
        }
    }

    fn to_camel_str(self) -> &'static str {
        match self {
            CacheEntryRole::kDataBlock => "DataBlock",
            CacheEntryRole::kFilterBlock => "FilterBlock",
            CacheEntryRole::kFilterMetaBlock => "FilterMetaBlock",
            CacheEntryRole::kDeprecatedFilterBlock => "DeprecatedFilterBlock",
            CacheEntryRole::kIndexBlock => "IndexBlock",
            CacheEntryRole::kOtherBlock => "OtherBlock",
            CacheEntryRole::kWriteBuffer => "WriteBuffer",
            CacheEntryRole::kCompressionDictionaryBuildingBuffer => {
                "CompressionDictionaryBuildingBuffer"
            }
            CacheEntryRole::kFilterConstruction => "FilterConstruction",
            CacheEntryRole::kBlockBasedTableReader => "BlockBasedTableReader",
            CacheEntryRole::kFileMetadata => "FileMetadata",
            CacheEntryRole::kBlobValue => "BlobValue",
            CacheEntryRole::kBlobCache => "BlobCache",
            CacheEntryRole::kMisc => "Misc",
            _ => unreachable!(),
        }
    }
}

fn cache_entry_role_to_camel_str(role: CacheEntryRole) -> &'static str {
    role.to_camel_str()
}

/// For use with `GetMapProperty()` for property
/// `DB::Properties::kBlockCacheEntryStats`. On success, the map will
/// be populated with all keys that can be obtained from these functions.
struct BlockCacheEntryStatsMapKeys;

impl BlockCacheEntryStatsMapKeys {
    fn cache_id() -> &'static str {
        "id"
    }

    fn cache_capacity_bytes() -> &'static str {
        "capacity"
    }

    fn last_collection_duration_seconds() -> &'static str {
        "secs_for_last_collection"
    }

    fn last_collection_age_seconds() -> &'static str {
        "secs_since_last_collection"
    }

    fn entry_count(role: CacheEntryRole) -> String {
        format!("count.{}", role.to_hyphen_str())
    }

    fn used_bytes(role: CacheEntryRole) -> String {
        format!("bytes.{}", role.to_hyphen_str())
    }

    fn used_percent(role: CacheEntryRole) -> String {
        format!("percent.{}", role.to_hyphen_str())
    }
}

fn block_cache_entry_stats_map_keys_cache_id() -> &'static str {
    BlockCacheEntryStatsMapKeys::cache_id()
}

fn block_cache_entry_stats_map_keys_cache_capacity_bytes() -> &'static str {
    BlockCacheEntryStatsMapKeys::cache_capacity_bytes()
}

fn block_cache_entry_stats_map_keys_last_collection_duration_seconds() -> &'static str {
    BlockCacheEntryStatsMapKeys::last_collection_duration_seconds()
}

fn block_cache_entry_stats_map_keys_last_collection_age_seconds() -> &'static str {
    BlockCacheEntryStatsMapKeys::last_collection_age_seconds()
}

fn block_cache_entry_stats_map_keys_entry_count(role: CacheEntryRole) -> String {
    BlockCacheEntryStatsMapKeys::entry_count(role)
}

fn block_cache_entry_stats_map_keys_used_bytes(role: CacheEntryRole) -> String {
    BlockCacheEntryStatsMapKeys::used_bytes(role)
}

fn block_cache_entry_stats_map_keys_used_percent(role: CacheEntryRole) -> String {
    BlockCacheEntryStatsMapKeys::used_percent(role)
}
