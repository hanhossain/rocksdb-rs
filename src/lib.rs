pub mod cache;
mod coding;
mod coding_lean;
pub mod compression_type;
pub mod env;
mod filename;
mod hash;
mod io_status;
mod options;
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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn create_string() {
        let cxx_str = ffi::make_string("hello world");
        assert_eq!(cxx_str.to_str().unwrap(), "hello world");
    }
}
