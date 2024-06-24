#[cxx::bridge(namespace = "rocksdb_rs::util::fastrange")]
mod ffix {
    extern "Rust" {
        #[cxx_name = "FastRange32"]
        fn fastrange_32(hash: u32, range: u32) -> u32;
    }
}

fn fastrange_32(hash: u32, range: u32) -> u32 {
    let product = range as u64 * hash as u64;
    (product >> 32) as u32
}
