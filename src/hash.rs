use xxhash_rust::xxh3::{xxh3_128, xxh3_128_with_seed};

#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
    extern "Rust" {
        #[cxx_name = "hash2x64"]
        fn hash2x64_ext(key: &[u8], upper: &mut u64, lower: &mut u64);

        #[cxx_name = "hash2x64_with_seed"]
        fn hash2x64_with_seed_ext(key: &[u8], seed: u64, upper: &mut u64, lower: &mut u64);

        fn xxh3_avalanche(mut h: u64) -> u64;
        fn xxh3_unavalanche(mut h: u64) -> u64;

        #[cxx_name = "bijective_hash2x64_with_seed"]
        fn bijective_hash2x64_with_seed_ext(
            high: u64,
            low: u64,
            seed: u64,
            high_res: &mut u64,
            low_res: &mut u64,
        );

        #[cxx_name = "bijective_hash2x64"]
        fn bijective_hash2x64_ext(high: u64, low: u64, high_res: &mut u64, low_res: &mut u64);
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

fn xxh3_avalanche(mut h: u64) -> u64 {
    h = h ^ (h >> 37);
    h = h.wrapping_mul(0x165667919E3779F9);
    h = h ^ (h >> 32);
    h
}

fn xxh3_unavalanche(mut h: u64) -> u64 {
    h = h ^ (h >> 32);
    h = h.wrapping_mul(0x8da8ee41d6df849); // inverse of 0x165667919E3779F9
    h = h ^ (h >> 37);
    h
}

fn bijective_hash2x64_with_seed(mut high: u64, low: u64, seed: u64) -> (u64, u64) {
    let bitflipl = 0x59973f0033362349u64.wrapping_sub(seed);
    let bitfliph = 0xc202797692d63d58u64.wrapping_add(seed);
    let mut tmp128 = multiply_64_to_128(low ^ high ^ bitflipl, 0x9E3779B185EBCA87);
    let mut lo = lower_64_of_128(tmp128);
    let mut hi = upper_64_of_128(tmp128);
    lo = lo.wrapping_add(0x3c0000000000000);
    high = high ^ bitfliph;
    hi = hi
        .wrapping_add(high)
        .wrapping_add((lower_32_of_64(high) as u64).wrapping_mul(0x85EBCA76));
    lo = lo ^ hi.swap_bytes();
    tmp128 = multiply_64_to_128(lo, 0xC2B2AE3D27D4EB4F);
    lo = lower_64_of_128(tmp128);
    hi = upper_64_of_128(tmp128).wrapping_add(hi.wrapping_mul(0xC2B2AE3D27D4EB4F));
    let low_res = xxh3_avalanche(lo);
    let high_res = xxh3_avalanche(hi);
    (high_res, low_res)
}

fn bijective_hash2x64_with_seed_ext(
    high: u64,
    low: u64,
    seed: u64,
    high_res: &mut u64,
    low_res: &mut u64,
) {
    (*high_res, *low_res) = bijective_hash2x64_with_seed(high, low, seed);
}

fn bijective_hash2x64(high: u64, low: u64) -> (u64, u64) {
    bijective_hash2x64_with_seed(high, low, 0)
}

fn bijective_hash2x64_ext(high: u64, low: u64, high_res: &mut u64, low_res: &mut u64) {
    (*high_res, *low_res) = bijective_hash2x64(high, low);
}

fn lower_64_of_128(v: u128) -> u64 {
    v as u64
}

fn upper_64_of_128(v: u128) -> u64 {
    (v >> 64) as u64
}

fn lower_32_of_64(v: u64) -> u32 {
    v as u32
}

fn multiply_64_to_128(a: u64, b: u64) -> u128 {
    (a as u128) * (b as u128)
}
