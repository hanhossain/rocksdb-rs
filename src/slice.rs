#[cxx::bridge(namespace = "rocksdb")]
pub mod ffi {
    unsafe extern "C++" {
        include!("rocksdb/slice.h");

        type Slice;

        #[cxx_name = "ToUniquePtrString"]
        fn to_unique_ptr_string(self: &Slice) -> UniquePtr<CxxString>;
    }
}
