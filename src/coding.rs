use crate::coding_lean::{
    decode_fixed_16_ptr, decode_fixed_32_ptr, decode_fixed_64_ptr, encode_fixed_16,
    encode_fixed_32, encode_fixed_64,
};
use cxx::CxxString;
use std::mem::size_of;
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

        #[cxx_name = "GetFixed16"]
        fn get_fixed_16(input: Pin<&mut Slice>, value: &mut u16) -> bool;

        #[cxx_name = "GetFixed32"]
        fn get_fixed_32(input: Pin<&mut Slice>, value: &mut u32) -> bool;

        #[cxx_name = "GetFixed64"]
        fn get_fixed_64(input: Pin<&mut Slice>, value: &mut u64) -> bool;
    }

    #[namespace = "rocksdb"]
    unsafe extern "C++" {
        include!("rocksdb/slice.h");

        type Slice = crate::slice::ffi::Slice;
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

fn get_fixed_16(input: Pin<&mut ffi::Slice>, value: &mut u16) -> bool {
    if input.size() < size_of::<u16>() {
        return false;
    }

    *value = unsafe { decode_fixed_16_ptr(input.data()) };
    input.remove_prefix(size_of::<u16>());
    true
}

fn get_fixed_32(input: Pin<&mut ffi::Slice>, value: &mut u32) -> bool {
    if input.size() < size_of::<u32>() {
        return false;
    }

    *value = unsafe { decode_fixed_32_ptr(input.data()) };
    input.remove_prefix(size_of::<u32>());
    true
}

fn get_fixed_64(input: Pin<&mut ffi::Slice>, value: &mut u64) -> bool {
    if input.size() < size_of::<u64>() {
        return false;
    }

    *value = unsafe { decode_fixed_64_ptr(input.data()) };
    input.remove_prefix(size_of::<u64>());
    true
}
