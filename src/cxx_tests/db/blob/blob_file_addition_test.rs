#[allow(dead_code)]
#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
    unsafe extern "C++" {
        include!("db/blob/blob_file_addition_test.h");

        fn BlobFileAdditionTest_Empty_Test();
        fn BlobFileAdditionTest_NonEmpty_Test();
        fn BlobFileAdditionTest_DecodeErrors_Test();
        fn BlobFileAdditionTest_ForwardCompatibleCustomField_Test();
        fn BlobFileAdditionTest_ForwardIncompatibleCustomField_Test();
    }
}

#[allow(non_snake_case)]
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn BlobFileAdditionTest_Empty_Test() {
        ffi::BlobFileAdditionTest_Empty_Test();
    }

    #[test]
    fn BlobFileAdditionTest_NonEmpty_Test() {
        ffi::BlobFileAdditionTest_NonEmpty_Test();
    }

    #[test]
    fn BlobFileAdditionTest_DecodeErrors_Test() {
        ffi::BlobFileAdditionTest_DecodeErrors_Test();
    }

    #[test]
    fn BlobFileAdditionTest_ForwardCompatibleCustomField_Test() {
        ffi::BlobFileAdditionTest_ForwardCompatibleCustomField_Test();
    }

    #[test]
    fn BlobFileAdditionTest_ForwardIncompatibleCustomField_Test() {
        ffi::BlobFileAdditionTest_ForwardIncompatibleCustomField_Test();
    }
}
