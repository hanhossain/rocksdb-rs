// autocxx hasn't migrated to edition 2024
#![allow(unsafe_op_in_unsafe_fn)]

autocxx::include_cpp! {
    #include "rocksdb/options.h"
    #include "rocksdb/slice.h"

    safety!(unsafe)

    generate!("rocksdb::DbPath")
    generate!("rocksdb::Slice")
}
pub use ffi::*;
