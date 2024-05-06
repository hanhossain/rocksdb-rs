#[allow(dead_code)]
#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
    unsafe extern "C++" {
        include!("db/blob/blob_counting_iterator_test.h");

        fn BlobCountingIteratorTest_CountBlobs_Test();
        fn BlobCountingIteratorTest_CorruptBlobIndex_Test();
    }
}

#[allow(non_snake_case)]
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn BlobCountingIteratorTest_CountBlobs_Test() {
        ffi::BlobCountingIteratorTest_CountBlobs_Test();
    }

    #[test]
    fn BlobCountingIteratorTest_CorruptBlobIndex_Test() {
        ffi::BlobCountingIteratorTest_CorruptBlobIndex_Test();
    }
}
