#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
    unsafe extern "C++" {
        include!("rocksdb/common_ffi.h");

        // Create a unique ptr of an empty string
        fn make_empty_string() -> UniquePtr<CxxString>;

        // Create a unique ptr of a string
        fn make_string(s: &str) -> UniquePtr<CxxString>;
    }
}

pub use ffi::*;
