use crate::ffi::CommonRustData;

#[cxx::bridge(namespace = "rocksdb")]
pub mod ffi {
    unsafe extern "C++" {
        include!("rocksdb/env.h");

        type Rusty;

        #[cxx_name = "Rusty_new"]
        fn rusty_new() -> UniquePtr<Rusty>;

        #[cxx_name = "HelloWorld"]
        fn hello_world(&self) -> UniquePtr<CxxString>;
    }

    extern "Rust" {
        fn hello_common(data: &CommonRustData) -> String;
    }

    struct CommonRustData {
        value: String,
    }

    enum Severity {
        kNoError = 0,
        kSoftError = 1,
        kHardError = 2,
        kFatalError = 3,
        kUnrecoverableError = 4,
        kMaxSeverity,
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
}
