#[cxx::bridge(namespace = "rocksdb_rs::options")]
pub mod ffix {
    #[namespace = "rocksdb"]
    unsafe extern "C++" {
        include!("rocksdb/options.h");

        type DbPath;

        #[cxx_name = "Path"]
        fn path(self: &DbPath) -> &CxxString;
    }

    impl CxxVector<DbPath> {}
}
