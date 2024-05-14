use std::ffi::c_char;
use std::io::Write;

#[cxx::bridge(namespace = "rocksdb_rs::coding_lean")]
mod ffix {

    extern "Rust" {
        #[cxx_name = "EncodeFixed16"]
        unsafe fn encode_fixed_16_ptr(buf: *mut c_char, value: u16);

        #[cxx_name = "EncodeFixed32"]
        unsafe fn encode_fixed_32_ptr(buf: *mut c_char, value: u32);

        #[cxx_name = "EncodeFixed64"]
        unsafe fn encode_fixed_64_ptr(buf: *mut c_char, value: u64);

        #[cxx_name = "DecodeFixed16"]
        unsafe fn decode_fixed_16_ptr(bytes: *const c_char) -> u16;

        #[cxx_name = "DecodeFixed32"]
        unsafe fn decode_fixed_32_ptr(bytes: *const c_char) -> u32;

        #[cxx_name = "DecodeFixed64"]
        unsafe fn decode_fixed_64_ptr(bytes: *const c_char) -> u64;
    }
}

pub(crate) fn encode_fixed_16(value: u16) -> [u8; 2] {
    value.to_le_bytes()
}

pub(crate) fn encode_fixed_32(value: u32) -> [u8; 4] {
    value.to_le_bytes()
}

pub(crate) fn encode_fixed_64(value: u64) -> [u8; 8] {
    value.to_le_bytes()
}

// Lower-level versions of Put... that write directly into a character buffer
// REQUIRES: dst has enough space for the value being written
// -- Implementation of the functions declared above
unsafe fn encode_fixed_16_ptr(buf: *mut c_char, value: u16) {
    let mut buf = std::slice::from_raw_parts_mut(buf as *mut u8, 2);
    buf.write_all(&encode_fixed_16(value)).unwrap();
}

unsafe fn encode_fixed_32_ptr(buf: *mut c_char, value: u32) {
    let mut buf = std::slice::from_raw_parts_mut(buf as *mut u8, 4);
    buf.write_all(&encode_fixed_32(value)).unwrap();
}

unsafe fn encode_fixed_64_ptr(buf: *mut c_char, value: u64) {
    let mut buf = std::slice::from_raw_parts_mut(buf as *mut u8, 8);
    buf.write_all(&encode_fixed_64(value)).unwrap();
}

pub(crate) fn decode_fixed_16(bytes: &[u8]) -> u16 {
    u16::from_le_bytes(bytes.try_into().unwrap())
}

pub(crate) fn decode_fixed_32(bytes: &[u8]) -> u32 {
    u32::from_le_bytes(bytes.try_into().unwrap())
}

pub(crate) fn decode_fixed_64(bytes: &[u8]) -> u64 {
    u64::from_le_bytes(bytes.try_into().unwrap())
}

// Lower-level versions of Get... that read directly from a character buffer
// without any bounds checking.
pub(crate) unsafe fn decode_fixed_16_ptr(bytes: *const c_char) -> u16 {
    decode_fixed_16(std::slice::from_raw_parts(bytes as *const _, 2))
}

pub(crate) unsafe fn decode_fixed_32_ptr(bytes: *const c_char) -> u32 {
    decode_fixed_32(std::slice::from_raw_parts(bytes as *const _, 4))
}

pub(crate) unsafe fn decode_fixed_64_ptr(bytes: *const c_char) -> u64 {
    decode_fixed_64(std::slice::from_raw_parts(bytes as *const _, 8))
}
