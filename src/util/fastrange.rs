//! fastrange/FastRange: A faster alternative to % for mapping a hash value
//! to an arbitrary range. See https://github.com/lemire/fastrange
//!
//! Generally recommended are FastRange32 for mapping results of 32-bit
//! hash functions and FastRange64 for mapping results of 64-bit hash
//! functions. FastRange is less forgiving than % if the input hashes are
//! not well distributed over the full range of the type (32 or 64 bits).

#[cxx::bridge(namespace = "rocksdb_rs::util::fastrange")]
mod ffix {
    extern "Rust" {
        /// Map a quality 64-bit hash value down to an arbitrary `usize` range.
        /// (`usize` is standard for mapping to things in memory.)
        #[cxx_name = "FastRange64"]
        fn fastrange_64(hash: u64, range: usize) -> u64;

        /// Map a quality 32-bit hash value down to an arbitrary `u32` range.
        #[cxx_name = "FastRange32"]
        fn fastrange_32(hash: u32, range: u32) -> u32;
    }
}

/// Map a quality 64-bit hash value down to an arbitrary `usize` range.
/// (`usize` is standard for mapping to things in memory.)
fn fastrange_64(hash: u64, range: usize) -> u64 {
    let product = range as u128 * hash as u128;
    (product >> 64) as u64
}

/// Map a quality 32-bit hash value down to an arbitrary `u32` range.
fn fastrange_32(hash: u32, range: u32) -> u32 {
    let product = range as u64 * hash as u64;
    (product >> 32) as u32
}
