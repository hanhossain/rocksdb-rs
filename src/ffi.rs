#![allow(unused_imports)]

autocxx::include_cpp! {
    #include "rocksdb/slice.h"
    safety!(unsafe)
    generate!("rocksdb::Slice")
}
pub use ffi::*;

pub mod options {
    autocxx::include_cpp! {
        #include "rocksdb/options.h"
        safety!(unsafe)
        name!(ffi_options)
        exclude_utilities!()
        generate!("rocksdb::DbPath")
    }
    pub use ffi_options::rocksdb::*;
}
