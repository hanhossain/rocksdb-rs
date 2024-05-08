#[allow(dead_code)]
#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
    unsafe extern "C++" {
        include!("util/coding_test.h");

        fn Coding_EncodingOutput_Test();
        fn Coding_Varint32_Test();
        fn Coding_Varint64_Test();
        fn Coding_Varint32Overflow_Test();
        fn Coding_Varint32Truncation_Test();
        fn Coding_Varint64Overflow_Test();
        fn Coding_Varint64Truncation_Test();
        fn Coding_Strings_Test();
    }
}

#[allow(non_snake_case)]
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn Coding_EncodingOutput_Test() {
        ffi::Coding_EncodingOutput_Test();
    }

    #[test]
    fn Coding_Varint32_Test() {
        ffi::Coding_Varint32_Test();
    }

    #[test]
    fn Coding_Varint64_Test() {
        ffi::Coding_Varint64_Test();
    }

    #[test]
    fn Coding_Varint32Overflow_Test() {
        ffi::Coding_Varint32Overflow_Test();
    }

    #[test]
    fn Coding_Varint32Truncation_Test() {
        ffi::Coding_Varint32Truncation_Test();
    }

    #[test]
    fn Coding_Varint64Overflow_Test() {
        ffi::Coding_Varint64Overflow_Test();
    }

    #[test]
    fn Coding_Varint64Truncation_Test() {
        ffi::Coding_Varint64Truncation_Test();
    }

    #[test]
    fn Coding_Strings_Test() {
        ffi::Coding_Strings_Test();
    }
}
