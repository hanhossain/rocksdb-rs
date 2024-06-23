#[cxx::bridge(namespace = "rocksdb_rs::util::bloom")]
mod ffix {
    extern "Rust" {
        /// False positive rate of a standard Bloom filter, for given ratio of
        /// filter memory bits to added keys, and number of probes per operation.
        /// (The false positive rate is effectively independent of scale, assuming
        /// the implementation scales OK.)
        #[cxx_name = "StandardFpRate"]
        fn standard_fp_rate(bits_per_key: f64, num_probes: i32) -> f64;

        /// False positive rate of a "blocked"/"shareded"/"cache-local" Bloom filter,
        /// for given ratio of filter memory bits to added keys, number of probes per
        /// operation (all within the given block or cache line size), and block or
        /// cache line size.
        #[cxx_name = "CacheLocalFpRate"]
        fn cache_local_fp_rate(bits_per_key: f64, num_probes: i32, cache_line_bits: i32) -> f64;
    }
}

/// False positive rate of a standard Bloom filter, for given ratio of
/// filter memory bits to added keys, and number of probes per operation.
/// (The false positive rate is effectively independent of scale, assuming
/// the implementation scales OK.)
fn standard_fp_rate(bits_per_key: f64, num_probes: i32) -> f64 {
    // Standard very-good-estimate formula. See
    // https://en.wikipedia.org/wiki/Bloom_filter#Probability_of_false_positives
    (1.0 - (-num_probes as f64 / bits_per_key).exp()).powi(num_probes)
}

/// False positive rate of a "blocked"/"shareded"/"cache-local" Bloom filter,
/// for given ratio of filter memory bits to added keys, number of probes per
/// operation (all within the given block or cache line size), and block or
/// cache line size.
fn cache_local_fp_rate(bits_per_key: f64, num_probes: i32, cache_line_bits: i32) -> f64 {
    if bits_per_key <= 0.0 {
        // Fix a discontinuity
        return 1.0;
    }

    let keys_per_cache_line = cache_line_bits as f64 / bits_per_key;
    // A reasonable estimate is the average of the FP rates for one standard
    // deviation above and below the mean bucket occupancy. See
    // https://github.com/facebook/rocksdb/wiki/RocksDB-Bloom-Filter#the-math
    let keys_stddev = keys_per_cache_line.sqrt();
    let crowded_fp = standard_fp_rate(
        cache_line_bits as f64 / (keys_per_cache_line + keys_stddev),
        num_probes,
    );
    let uncrowded_fp = standard_fp_rate(
        cache_line_bits as f64 / (keys_per_cache_line - keys_stddev),
        num_probes,
    );
    (crowded_fp + uncrowded_fp) / 2.0
}
