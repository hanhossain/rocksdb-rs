#![allow(dead_code)]
#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
    unsafe extern "C++" {
        include!("util/coding_test.h");

        fn Coding_EncodingOutput();
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn coding_encoding_output() {
        ffi::Coding_EncodingOutput();
    }
}
