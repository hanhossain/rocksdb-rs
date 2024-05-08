#[allow(dead_code)]
#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
    unsafe extern "C++" {
        include!("db/blob/blob_garbage_meter_test.h");

        fn BlobGarbageMeterTest_MeasureGarbage_Test();
        fn BlobGarbageMeterTest_PlainValue_Test();
        fn BlobGarbageMeterTest_CorruptInternalKey_Test();
        fn BlobGarbageMeterTest_CorruptBlobIndex_Test();
        fn BlobGarbageMeterTest_InlinedTTLBlobIndex_Test();
    }
}

#[allow(non_snake_case)]
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn BlobGarbageMeterTest_MeasureGarbage_Test() {
        ffi::BlobGarbageMeterTest_MeasureGarbage_Test();
    }

    #[test]
    fn BlobGarbageMeterTest_PlainValue_Test() {
        ffi::BlobGarbageMeterTest_PlainValue_Test();
    }

    #[test]
    fn BlobGarbageMeterTest_CorruptInternalKey_Test() {
        ffi::BlobGarbageMeterTest_CorruptInternalKey_Test();
    }

    #[test]
    fn BlobGarbageMeterTest_CorruptBlobIndex_Test() {
        ffi::BlobGarbageMeterTest_CorruptBlobIndex_Test();
    }

    #[test]
    fn BlobGarbageMeterTest_InlinedTTLBlobIndex_Test() {
        ffi::BlobGarbageMeterTest_InlinedTTLBlobIndex_Test();
    }
}
