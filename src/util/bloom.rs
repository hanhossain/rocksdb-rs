#[cxx::bridge(namespace = "rocksdb_rs::util::bloom")]
mod ffix {
    extern "Rust" {
        /// False positive rate of a standard Bloom filter, for given ratio of
        /// filter memory bits to added keys, and number of probes per operation.
        /// (The false positive rate is effectively independent of scale, assuming
        /// the implementation scales OK.)
        #[cxx_name = "StandardFpRate"]
        fn standard_fp_rate(bits_per_key: f64, num_probes: i32) -> f64;
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
