use crate::coding_lean::{
    decode_fixed_16_ptr, decode_fixed_32_ptr, decode_fixed_64_ptr, encode_fixed_16,
    encode_fixed_32, encode_fixed_64,
};
use cxx::CxxString;
use std::ffi::c_char;
use std::mem::size_of;
use std::pin::Pin;

#[cxx::bridge(namespace = "rocksdb_rs::coding")]
mod ffix {
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

        #[cxx_name = "GetSliceUntil"]
        fn get_slice_until(mut slice: Pin<&mut Slice>, delimeter: c_char) -> Pin<&mut Slice>;

        /// Returns the length of the varint32 or varint64 encoding of "v"
        #[cxx_name = "VarintLength"]
        fn varint_length(mut v: u64) -> i32;

        #[cxx_name = "i64ToZigzag"]
        fn i64_to_zigzag(v: i64) -> u64;

        #[cxx_name = "zigzagToI64"]
        fn zigzag_to_i64(n: u64) -> i64;

        #[cxx_name = "GetVarint64Ptr"]
        unsafe fn get_varint64_ptr(
            p: *const c_char,
            limit: *const c_char,
            value: *mut u64,
        ) -> *const c_char;

        /// Internal routine for use by fallback path of GetVarint32Ptr
        #[cxx_name = "GetVarint32PtrFallback"]
        unsafe fn get_varint32_ptr_fallback(
            p: *const c_char,
            limit: *const c_char,
            value: *mut u32,
        ) -> *const c_char;

        #[cxx_name = "GetVarint32Ptr"]
        unsafe fn get_varint32_ptr(
            p: *const c_char,
            limit: *const c_char,
            value: *mut u32,
        ) -> *const c_char;

        /// Lower-level versions of Put... that write directly into a character buffer
        /// and return a pointer just past the last byte written.
        /// REQUIRES: dst has enough space for the value being written
        #[cxx_name = "EncodeVarint32"]
        unsafe fn encode_varint32(dst: *mut c_char, v: u32) -> *mut c_char;
    }

    #[namespace = "rocksdb"]
    unsafe extern "C++" {
        include!("rocksdb/slice.h");

        type Slice = crate::ffi::rocksdb::Slice;
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

fn get_fixed_16(input: Pin<&mut ffix::Slice>, value: &mut u16) -> bool {
    if input.size() < size_of::<u16>() {
        return false;
    }

    *value = unsafe { decode_fixed_16_ptr(input.data()) };
    input.remove_prefix(size_of::<u16>());
    true
}

fn get_fixed_32(input: Pin<&mut ffix::Slice>, value: &mut u32) -> bool {
    if input.size() < size_of::<u32>() {
        return false;
    }

    *value = unsafe { decode_fixed_32_ptr(input.data()) };
    input.remove_prefix(size_of::<u32>());
    true
}

fn get_fixed_64(input: Pin<&mut ffix::Slice>, value: &mut u64) -> bool {
    if input.size() < size_of::<u64>() {
        return false;
    }

    *value = unsafe { decode_fixed_64_ptr(input.data()) };
    input.remove_prefix(size_of::<u64>());
    true
}

fn get_slice_until(mut slice: Pin<&mut ffix::Slice>, delimeter: c_char) -> Pin<&mut ffix::Slice> {
    let mut p = slice.data();
    let mut n = slice.size();
    while n > 0 {
        if unsafe { *p } == delimeter {
            break;
        }
        p = unsafe { p.add(1) };
        n -= 1;
    }

    let size = slice.size() - n;
    slice.as_mut().remove_prefix(size);
    slice
}

/// Returns the length of the varint32 or varint64 encoding of "v"
fn varint_length(mut v: u64) -> i32 {
    let mut len = 1;
    while v >= 128 {
        v >>= 7;
        len += 1;
    }
    len
}

// Borrowed from
// https://github.com/facebook/fbthrift/blob/449a5f77f9f9bae72c9eb5e78093247eef185c04/thrift/lib/cpp/util/VarintUtils-inl.h#L202-L208
fn i64_to_zigzag(v: i64) -> u64 {
    ((v as u64) << 1) ^ ((v >> 63) as u64)
}

fn zigzag_to_i64(n: u64) -> i64 {
    ((n >> 1) as i64) ^ (-((n & 1) as i64))
}

unsafe fn get_varint64_ptr(
    mut p: *const c_char,
    limit: *const c_char,
    value: *mut u64,
) -> *const c_char {
    let mut result = 0;
    let mut shift = 0;

    while shift <= 63 && p < limit {
        let byte = *p as u64;
        p = p.add(1);

        if byte & 0x80 == 0 {
            *value = result | (byte << shift);
            return p;
        }

        result |= (byte & 0x7f) << shift;
        shift += 7;
    }

    std::ptr::null()
}

/// Internal routine for use by fallback path of GetVarint32Ptr
unsafe fn get_varint32_ptr_fallback(
    mut p: *const c_char,
    limit: *const c_char,
    value: *mut u32,
) -> *const c_char {
    let mut result = 0;
    let mut shift = 0;

    while shift <= 28 && p < limit {
        let byte = *p as u32;
        p = p.add(1);

        if byte & 0x80 == 0 {
            *value = result | (byte << shift);
            return p;
        }

        result |= (byte & 0x7f) << shift;
        shift += 7;
    }

    std::ptr::null()
}

/// Pointer-based variants of GetVarint...  These either store a value
/// in *v and return a pointer just past the parsed value, or return
/// nullptr on error.  These routines only look at bytes in the range
/// [p..limit-1]
unsafe fn get_varint32_ptr(
    p: *const c_char,
    limit: *const c_char,
    value: *mut u32,
) -> *const c_char {
    if p < limit {
        let result = *p as u32;
        if result & 0x80 == 0 {
            *value = result;
            return p.add(1);
        }
    }
    get_varint32_ptr_fallback(p, limit, value)
}

unsafe fn encode_varint32(dst: *mut c_char, v: u32) -> *mut c_char {
    let mut ptr = dst as *mut u8;
    let msb = 0x80;

    if v < (1 << 7) {
        *ptr = v as u8;
        ptr = ptr.add(1);
    } else if v < (1 << 14) {
        *ptr = (v | msb) as u8;
        ptr = ptr.add(1);
        *ptr = (v >> 7) as u8;
        ptr = ptr.add(1);
    } else if v < (1 << 21) {
        *ptr = (v | msb) as u8;
        ptr = ptr.add(1);
        *ptr = ((v >> 7) | msb) as u8;
        ptr = ptr.add(1);
        *ptr = (v >> 14) as u8;
        ptr = ptr.add(1);
    } else if v < (1 << 28) {
        *ptr = (v | msb) as u8;
        ptr = ptr.add(1);
        *ptr = ((v >> 7) | msb) as u8;
        ptr = ptr.add(1);
        *ptr = ((v >> 14) | msb) as u8;
        ptr = ptr.add(1);
        *ptr = (v >> 21) as u8;
        ptr = ptr.add(1);
    } else {
        *ptr = (v | msb) as u8;
        ptr = ptr.add(1);
        *ptr = ((v >> 7) | msb) as u8;
        ptr = ptr.add(1);
        *ptr = ((v >> 14) | msb) as u8;
        ptr = ptr.add(1);
        *ptr = ((v >> 21) | msb) as u8;
        ptr = ptr.add(1);
        *ptr = (v >> 28) as u8;
        ptr = ptr.add(1);
    }
    ptr as *mut _
}

#[cfg(test)]
mod tests {
    use super::*;
    use cxx::let_cxx_string;

    #[test]
    fn fixed16() {
        let_cxx_string!(s = "");
        for v in 0..0xFFFF {
            put_fixed_16(s.as_mut(), v);
        }

        let mut p = s.as_ptr();
        for v in 0..0xFFFF {
            let actual = unsafe { decode_fixed_16_ptr(p as *const _) };
            assert_eq!(v, actual);
            p = unsafe { p.add(size_of::<u16>()) };
        }
    }

    #[test]
    fn fixed32() {
        let_cxx_string!(s = "");
        for v in 0..100_000 {
            put_fixed_32(s.as_mut(), v);
        }

        let mut p = s.as_ptr();
        for v in 0..100_000 {
            let actual = unsafe { decode_fixed_32_ptr(p as *const _) };
            assert_eq!(v, actual);
            p = unsafe { p.add(size_of::<u32>()) };
        }
    }

    #[test]
    fn fixed64() {
        let_cxx_string!(s = "");
        for power in 0..=63 {
            let v = 1u64 << power;
            put_fixed_64(s.as_mut(), v - 1);
            put_fixed_64(s.as_mut(), v);
            put_fixed_64(s.as_mut(), v + 1);
        }

        let mut p = s.as_ptr();
        for power in 0..63 {
            let v = 1u64 << power;
            unsafe {
                let actual = decode_fixed_64_ptr(p as *const _);
                assert_eq!(v - 1, actual);
                p = p.add(size_of::<u64>());

                let actual = decode_fixed_64_ptr(p as *const _);
                assert_eq!(v, actual);
                p = p.add(size_of::<u64>());

                let actual = decode_fixed_64_ptr(p as *const _);
                assert_eq!(v + 1, actual);
                p = p.add(size_of::<u64>());
            }
        }
    }
}
