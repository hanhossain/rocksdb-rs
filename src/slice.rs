#[cxx::bridge(namespace = "rocksdb_rs::slice")]
pub mod ffix {

    #[namespace = "rocksdb"]
    unsafe extern "C++" {
        include!("rocksdb/slice.h");

        type Slice;

        /// Return the length (in bytes) of the referenced data
        fn size(self: &Slice) -> usize;
        /// Return a pointer to the beginning of the referenced data
        fn data(self: &Slice) -> *const c_char;
        fn remove_prefix(self: Pin<&mut Slice>, n: usize);

        #[cxx_name = "ToUniquePtrString"]
        fn to_unique_ptr_string(self: &Slice) -> UniquePtr<CxxString>;
    }
}
