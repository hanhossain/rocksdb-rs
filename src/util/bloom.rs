#[cxx::bridge(namespace = "rocksdb_rs::util::bloom")]
mod ffix {
    extern "Rust" {
        /// False positive rate of a standard Bloom filter, for given ratio of
        /// filter memory bits to added keys, and number of probes per operation.
        /// (The false positive rate is effectively independent of scale, assuming
        /// the implementation scales OK.)
        #[cxx_name = "BloomMath_StandardFpRate"]
        fn bloom_math_standard_fp_rate(bits_per_key: f64, num_probes: i32) -> f64;

        /// False positive rate of a "blocked"/"shareded"/"cache-local" Bloom filter,
        /// for given ratio of filter memory bits to added keys, number of probes per
        /// operation (all within the given block or cache line size), and block or
        /// cache line size.
        #[cxx_name = "BloomMath_CacheLocalFpRate"]
        fn bloom_math_cache_local_fp_rate(
            bits_per_key: f64,
            num_probes: i32,
            cache_line_bits: i32,
        ) -> f64;

        /// False positive rate of querying a new item against `num_keys` items, all
        /// hashed to `fingerprint_bits` bits. (This assumes the fingerprint hashes
        /// themselves are stored losslessly. See Section 4 of
        /// http://www.ccs.neu.edu/home/pete/pub/bloom-filters-verification.pdf)
        #[cxx_name = "BloomMath_FingerprintFpRate"]
        fn bloom_math_fingerprint_fp_rate(num_keys: usize, fingerprint_bits: i32) -> f64;

        /// Returns the probably of either of two independent(-ish) events
        /// happening, given their probabilities. (This is useful for combining
        /// results from StandardFpRate or CacheLocalFpRate with FingerprintFpRate
        /// for a hash-efficient Bloom filter's FP rate. See Section 4 of
        /// http://www.ccs.neu.edu/home/pete/pub/bloom-filters-verification.pdf)
        #[cxx_name = "BloomMath_IndependentProbabilitySum"]
        fn bloom_math_independent_probability_sum(rate1: f64, rate2: f64) -> f64;

        /// NOTE: this has only been validated to enough accuracy for producing
        /// reasonable warnings / user feedback, not for making functional decisions.
        #[cxx_name = "FastLocalBloomImpl_EstimatedFpRate"]
        fn fast_local_bloom_impl_estimated_fp_rate(
            keys: usize,
            bytes: usize,
            num_probes: i32,
            hash_bits: i32,
        ) -> f64;

        #[cxx_name = "FastLocalBloomImpl_ChooseNumProbes"]
        fn fast_local_bloom_impl_choose_num_probes(millibits_per_key: i32) -> i32;
    }
}

struct BloomMath;

impl BloomMath {
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
        let crowded_fp = BloomMath::standard_fp_rate(
            cache_line_bits as f64 / (keys_per_cache_line + keys_stddev),
            num_probes,
        );
        let uncrowded_fp = BloomMath::standard_fp_rate(
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
}

fn bloom_math_standard_fp_rate(bits_per_key: f64, num_probes: i32) -> f64 {
    BloomMath::standard_fp_rate(bits_per_key, num_probes)
}

fn bloom_math_cache_local_fp_rate(bits_per_key: f64, num_probes: i32, cache_line_bits: i32) -> f64 {
    BloomMath::cache_local_fp_rate(bits_per_key, num_probes, cache_line_bits)
}

fn bloom_math_fingerprint_fp_rate(num_keys: usize, fingerprint_bits: i32) -> f64 {
    BloomMath::fingerprint_fp_rate(num_keys, fingerprint_bits)
}

fn bloom_math_independent_probability_sum(rate1: f64, rate2: f64) -> f64 {
    BloomMath::independent_probability_sum(rate1, rate2)
}

/// A fast, flexible, and accurate cache-local Bloom implementation with
/// SIMD-optimized query performance (currently using AVX2 on Intel). Write
/// performance and non-SIMD read are very good, benefiting from FastRange32
/// used in place of % and single-cycle multiplication on recent processors.
///
/// Most other SIMD Bloom implementations sacrifice flexibility and/or
/// accuracy by requiring num_probes to be a power of two and restricting
/// where each probe can occur in a cache line. This implementation sacrifices
/// SIMD-optimization for add (might still be possible, especially with AVX512)
/// in favor of allowing any num_probes, not crossing cache line boundary,
/// and accuracy close to theoretical best accuracy for a cache-local Bloom.
/// E.g. theoretical best for 10 bits/key, num_probes=6, and 512-bit bucket
/// (Intel cache line size) is 0.9535% FP rate. This implementation yields
/// about 0.957%. (Compare to LegacyLocalityBloomImpl<false> at 1.138%, or
/// about 0.951% for 1024-bit buckets, cache line size for some ARM CPUs.)
///
/// This implementation can use a 32-bit hash (let h2 be h1 * 0x9e3779b9) or
/// a 64-bit hash (split into two uint32s). With many millions of keys, the
/// false positive rate associated with using a 32-bit hash can dominate the
/// false positive rate of the underlying filter. At 10 bits/key setting, the
/// inflection point is about 40 million keys, so 32-bit hash is a bad idea
/// with 10s of millions of keys or more.
///
/// Despite accepting a 64-bit hash, this implementation uses 32-bit fastrange
/// to pick a cache line, which can be faster than 64-bit in some cases.
/// This only hurts accuracy as you get into 10s of GB for a single filter,
/// and accuracy abruptly breaks down at 256GB (2^32 cache lines). Switch to
/// 64-bit fastrange if you need filters so big. ;)
///
/// Using only a 32-bit input hash within each cache line has negligible
/// impact for any reasonable cache line / bucket size, for arbitrary filter
/// size, and potentially saves intermediate data size in some cases vs.
/// tracking full 64 bits. (Even in an implementation using 64-bit arithmetic
/// to generate indices, I might do the same, as a single multiplication
/// suffices to generate a sufficiently mixed 64 bits from 32 bits.)
///
/// This implementation is currently tied to Intel cache line size, 64 bytes ==
/// 512 bits. If there's sufficient demand for other cache line sizes, this is
/// a pretty good implementation to extend, but slight performance enhancements
/// are possible with an alternate implementation (probably not very compatible
/// with SIMD):
/// (1) Use rotation in addition to multiplication for remixing
/// (like murmur hash). (Using multiplication alone *slightly* hurts accuracy
/// because lower bits never depend on original upper bits.)
/// (2) Extract more than one bit index from each re-mix. (Only if rotation
/// or similar is part of remix, because otherwise you're making the
/// multiplication-only problem worse.)
/// (3) Re-mix full 64 bit hash, to get maximum number of bit indices per
/// re-mix.
struct FastLocalBloomImpl;

impl FastLocalBloomImpl {
    /// NOTE: this has only been validated to enough accuracy for producing
    /// reasonable warnings / user feedback, not for making functional decisions.
    fn estimated_fp_rate(keys: usize, bytes: usize, num_probes: i32, hash_bits: i32) -> f64 {
        BloomMath::independent_probability_sum(
            BloomMath::cache_local_fp_rate(8.0 * bytes as f64 / keys as f64, num_probes, 512),
            BloomMath::fingerprint_fp_rate(keys, hash_bits),
        )
    }

    fn choose_num_probes(millibits_per_key: i32) -> i32 {
        // Since this implementation can (with AVX2) make up to 8 probes
        // for the same cost, we pick the most accurate num_probes, based
        // on actual tests of the implementation. Note that for higher
        // bits/key, the best choice for cache-local Bloom can be notably
        // smaller than standard bloom, e.g. 9 instead of 11 @ 16 b/k.
        return if millibits_per_key <= 2080 {
            1
        } else if millibits_per_key <= 3580 {
            2
        } else if millibits_per_key <= 5100 {
            3
        } else if millibits_per_key <= 6640 {
            4
        } else if millibits_per_key <= 8300 {
            5
        } else if millibits_per_key <= 10070 {
            6
        } else if millibits_per_key <= 11720 {
            7
        } else if millibits_per_key <= 14001 {
            // Would be something like <= 13800 but sacrificing *slightly* for
            // more settings using <= 8 probes.
            8
        } else if millibits_per_key <= 16050 {
            9
        } else if millibits_per_key <= 18300 {
            10
        } else if millibits_per_key <= 22001 {
            11
        } else if millibits_per_key <= 25501 {
            12
        } else if millibits_per_key > 50000 {
            // Top out at 24 probes (three sets of 8)
            24
        } else {
            // Roughly optimal choices for remaining range
            // e.g.
            // 28000 -> 12, 28001 -> 13
            // 50000 -> 23, 50001 -> 24
            (millibits_per_key - 1) / 2000 - 1
        };
    }
}

fn fast_local_bloom_impl_estimated_fp_rate(
    keys: usize,
    bytes: usize,
    num_probes: i32,
    hash_bits: i32,
) -> f64 {
    FastLocalBloomImpl::estimated_fp_rate(keys, bytes, num_probes, hash_bits)
}

fn fast_local_bloom_impl_choose_num_probes(millibits_per_key: i32) -> i32 {
    FastLocalBloomImpl::choose_num_probes(millibits_per_key)
}
