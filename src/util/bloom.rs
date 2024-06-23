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

        /// False positive rate of querying a new item against `num_keys` items, all
        /// hashed to `fingerprint_bits` bits. (This assumes the fingerprint hashes
        /// themselves are stored losslessly. See Section 4 of
        /// http://www.ccs.neu.edu/home/pete/pub/bloom-filters-verification.pdf)
        #[cxx_name = "FingerprintFpRate"]
        fn fingerprint_fp_rate(num_keys: usize, fingerprint_bits: i32) -> f64;

        /// Returns the probably of either of two independent(-ish) events
        /// happening, given their probabilities. (This is useful for combining
        /// results from StandardFpRate or CacheLocalFpRate with FingerprintFpRate
        /// for a hash-efficient Bloom filter's FP rate. See Section 4 of
        /// http://www.ccs.neu.edu/home/pete/pub/bloom-filters-verification.pdf)
        #[cxx_name = "IndependentProbabilitySum"]
        fn independent_probability_sum(rate1: f64, rate2: f64) -> f64;
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

/// False positive rate of querying a new item against `num_keys` items, all
/// hashed to `fingerprint_bits` bits. (This assumes the fingerprint hashes
/// themselves are stored losslessly. See Section 4 of
/// http://www.ccs.neu.edu/home/pete/pub/bloom-filters-verification.pdf)
fn fingerprint_fp_rate(num_keys: usize, fingerprint_bits: i32) -> f64 {
    let inv_fingerprint_space = 0.5f64.powi(fingerprint_bits);
    // Base estimate assumes each key maps to a unique fingerprint.
    // Could be > 1 in extreme cases.
    let base_estimate = num_keys as f64 * inv_fingerprint_space;
    // To account for potential overlap, we choose between two formulas
    if base_estimate > 0.0001 {
        // A very good formula assuming we don't construct a floating point
        // number extremely close to 1. Always produces a probability < 1.
        1.0 - (-base_estimate).exp()
    } else {
        // A very good formula when base_estimate is far below 1. (Subtract
        // away the integral-approximated sum that some key has same hash as
        // one coming before it in a list.)
        base_estimate - (base_estimate * base_estimate * 0.5)
    }
}

/// Returns the probably of either of two independent(-ish) events
/// happening, given their probabilities. (This is useful for combining
/// results from StandardFpRate or CacheLocalFpRate with FingerprintFpRate
/// for a hash-efficient Bloom filter's FP rate. See Section 4 of
/// http://www.ccs.neu.edu/home/pete/pub/bloom-filters-verification.pdf)
fn independent_probability_sum(rate1: f64, rate2: f64) -> f64 {
    // Use formula that avoids floating point extremely close to 1 if
    // rates are extremely small.
    rate1 + rate2 - rate1 * rate2
}
