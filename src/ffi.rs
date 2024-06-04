#![allow(unused_imports)]

autocxx::include_cpp! {
    #include "rocksdb/options.h"
    #include "rocksdb/slice.h"

    safety!(unsafe)

    generate!("rocksdb::DbPath")
    generate!("rocksdb::Slice")
}
pub use ffi::*;
