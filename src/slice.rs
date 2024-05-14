#[cxx::bridge(namespace = "rocksdb")]
pub mod ffix {
    unsafe extern "C++" {
        include!("rocksdb/slice.h");

        type Slice;

        #[cxx_name = "ToUniquePtrString"]
        fn to_unique_ptr_string(self: &Slice) -> UniquePtr<CxxString>;

        fn size(self: &Slice) -> usize;

        fn data(self: &Slice) -> *const c_char;

        fn remove_prefix(self: Pin<&mut Slice>, n: usize);
    }
}
