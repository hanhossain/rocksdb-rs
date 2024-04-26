use crate::coding_lean::{encode_fixed_16, encode_fixed_32, encode_fixed_64};
use cxx::CxxString;
use std::pin::Pin;

#[cxx::bridge(namespace = "rocksdb_rs::coding")]
mod ffi {
    extern "Rust" {
        /// Pull the last 8 bits and cast it to a character
        #[cxx_name = "PutFixed16"]
        fn put_fixed_16(dst: Pin<&mut CxxString>, value: u16);

        #[cxx_name = "PutFixed32"]
        fn put_fixed_32(dst: Pin<&mut CxxString>, value: u32);

        #[cxx_name = "PutFixed64"]
        fn put_fixed_64(dst: Pin<&mut CxxString>, value: u64);
    }
}

/// Pull the last 8 bits and cast it to a character
fn put_fixed_16(dst: Pin<&mut CxxString>, value: u16) {
    dst.push_bytes(&encode_fixed_16(value));
}

fn put_fixed_32(dst: Pin<&mut CxxString>, value: u32) {
    dst.push_bytes(&encode_fixed_32(value));
}

fn put_fixed_64(dst: Pin<&mut CxxString>, value: u64) {
    dst.push_bytes(&encode_fixed_64(value));
}
