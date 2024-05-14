#[cxx::bridge(namespace = "rocksdb")]
pub mod ffix {
    unsafe extern "C++" {
        include!("rocksdb/options.h");

        type DbPath;
        #[cxx_name = "Path"]
        fn path(self: &DbPath) -> &CxxString;
    }

    impl CxxVector<DbPath> {}
}
