pub mod cache;
mod coding_lean;
pub mod compression_type;
pub mod env;
mod filename;
mod hash;
mod options;
pub mod port_defs;
pub mod slice;
pub mod status;
mod string_util;
pub mod unique_id;

#[cxx::bridge(namespace = "rocksdb")]
pub mod ffi {
    unsafe extern "C++" {
        include!("rocksdb/common_ffi.h");

        fn make_string() -> UniquePtr<CxxString>;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn create_string() {
        let mut cxx_str = ffi::make_string();
        cxx_str.pin_mut().push_str("hello world");
        assert_eq!(cxx_str.to_str().unwrap(), "hello world");
    }
}
