// autocxx hasn't migrated to edition 2024
#![allow(unsafe_op_in_unsafe_fn)]

autocxx::include_cpp! {
    safety!(unsafe)
}
pub use ffi::*;
