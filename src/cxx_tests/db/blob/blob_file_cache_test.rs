#[allow(dead_code)]
#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
    unsafe extern "C++" {
        include!("db/blob/blob_file_cache_test.h");

        fn BlobFileCacheTest_GetBlobFileReader_Test();
        fn BlobFileCacheTest_GetBlobFileReader_Race_Test();
        fn BlobFileCacheTest_GetBlobFileReader_IOError_Test();
        fn BlobFileCacheTest_GetBlobFileReader_CacheFull_Test();
    }
}

#[allow(non_snake_case)]
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn BlobFileCacheTest_GetBlobFileReader_Test() {
        ffi::BlobFileCacheTest_GetBlobFileReader_Test();
    }

    #[test]
    fn BlobFileCacheTest_GetBlobFileReader_Race_Test() {
        ffi::BlobFileCacheTest_GetBlobFileReader_Race_Test();
    }

    #[test]
    fn BlobFileCacheTest_GetBlobFileReader_IOError_Test() {
        ffi::BlobFileCacheTest_GetBlobFileReader_IOError_Test();
    }

    #[test]
    fn BlobFileCacheTest_GetBlobFileReader_CacheFull_Test() {
        ffi::BlobFileCacheTest_GetBlobFileReader_CacheFull_Test();
    }
}
