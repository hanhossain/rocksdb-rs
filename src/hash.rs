use xxhash_rust::xxh3::{xxh3_128, xxh3_128_with_seed};

/// Hashes a key using the xxh3 algorithm and returns the 128-bit hash as `(upper-64, lower-64)`.
pub(crate) fn hash2x64(key: &[u8]) -> (u64, u64) {
    let h = xxh3_128(key);
    let upper = h >> 64;
    let lower = h & 0xffff_ffff_ffff_ffff;
    (upper as u64, lower as u64)
}

/// Hashes a key using the xxh3 algorithm with seed and returns the 128-bit hash as `(upper-64,
/// lower-64)`.
pub(crate) fn hash2x64_with_seed(key: &[u8], seed: u64) -> (u64, u64) {
    let h = xxh3_128_with_seed(key, seed);
    let upper = h >> 64;
    let lower = h & 0xffff_ffff_ffff_ffff;
    (upper as u64, lower as u64)
}
