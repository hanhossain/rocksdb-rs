use xxhash_rust::xxh3::{xxh3_128, xxh3_128_with_seed};

#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
    extern "Rust" {
        #[cxx_name = "hash2x64"]
        fn hash2x64_ext(key: &[u8], upper: &mut u64, lower: &mut u64);

        #[cxx_name = "hash2x64_with_seed"]
        fn hash2x64_with_seed_ext(key: &[u8], seed: u64, upper: &mut u64, lower: &mut u64);
    }
}

/// Hashes a key using the xxh3 algorithm and returns the 128-bit hash as `(upper-64, lower-64)`.
pub(crate) fn hash2x64(key: &[u8]) -> (u64, u64) {
    let h = xxh3_128(key);
    let upper = h >> 64;
    let lower = h & 0xffff_ffff_ffff_ffff;
    (upper as u64, lower as u64)
}

fn hash2x64_ext(key: &[u8], upper: &mut u64, lower: &mut u64) {
    (*upper, *lower) = hash2x64(key);
}

/// Hashes a key using the xxh3 algorithm with seed and returns the 128-bit hash as `(upper-64,
/// lower-64)`.
pub(crate) fn hash2x64_with_seed(key: &[u8], seed: u64) -> (u64, u64) {
    let h = xxh3_128_with_seed(key, seed);
    let upper = h >> 64;
    let lower = h & 0xffff_ffff_ffff_ffff;
    (upper as u64, lower as u64)
}

fn hash2x64_with_seed_ext(key: &[u8], seed: u64, upper: &mut u64, lower: &mut u64) {
    (*upper, *lower) = hash2x64_with_seed(key, seed);
}
