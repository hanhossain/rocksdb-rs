#![allow(unused_imports)]

autocxx::include_cpp! {
    safety!(unsafe)
}
pub use ffi::*;

pub mod slice {
    autocxx::include_cpp! {
        #include "rocksdb/slice.h"
        safety!(unsafe)
        name!(ffi_slice)
        exclude_utilities!()
        generate!("rocksdb::Slice")

    }
    pub use ffi_slice::rocksdb::*;
}

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

pub mod table_properties {
    autocxx::include_cpp! {
        #include "rocksdb/table_properties.h"
        safety!(unsafe)
        name!(ffi_table_properties)
        exclude_utilities!()
        generate!("rocksdb::TableProperties")
    }
    pub use ffi_table_properties::rocksdb::*;
}
