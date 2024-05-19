#![allow(unused_imports)]

pub mod cache;
mod coding;
mod coding_lean;
pub mod compression_type;
mod filename;
mod hash;
mod io_status;
pub mod port_defs;
pub mod status;
mod string_util;
mod transaction_log;
mod types;
pub mod unique_id;

autocxx::include_cpp! {
    #include "rocksdb/slice.h"
    safety!(unsafe)

    generate!("rocksdb::Slice")
}

autocxx::include_cpp! {
    #include "rocksdb/options.h"
    safety!(unsafe)
    name!(ffi_options)
    generate!("rocksdb::DbPath")
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn create_string() {
        let cxx_str = ffi::make_string("hello world");
        assert_eq!(cxx_str.to_str().unwrap(), "hello world");
    }
}
