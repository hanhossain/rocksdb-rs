use crate::env::ffi::CommonRustData;

#[cxx::bridge(namespace = "rocksdb")]
pub mod ffi {
    unsafe extern "C++" {
        include!("rocksdb/env.h");

        type Rusty;

        #[cxx_name = "Rusty_new"]
        fn rusty_new() -> UniquePtr<Rusty>;

        #[cxx_name = "HelloWorld"]
        fn hello_world(&self) -> UniquePtr<CxxString>;

        #[cxx_name = "HelloCommonFromCpp"]
        fn hello_common_from_cpp() -> String;
    }

    extern "Rust" {
        fn hello_common(data: &CommonRustData) -> String;
    }

    struct CommonRustData {
        value: String,
    }
}

pub fn hello_common(data: &CommonRustData) -> String {
    format!("Hello {} from rust!", data.value)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn create_rusty() {
        let _rusty = ffi::rusty_new();
    }

    #[test]
    fn hello_rusty() {
        let rusty = ffi::rusty_new();
        let value = rusty.hello_world();
        let value = value.to_string();
        assert_eq!(value, "Hello World from C++!");
    }

    #[test]
    fn hello_from_cpp() {
        let value = ffi::hello_common_from_cpp();
        assert_eq!(value, "Hello c++ from rust!");
    }
}
