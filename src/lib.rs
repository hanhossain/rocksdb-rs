use crate::ffi::rocksdb::Rusty;
use autocxx::prelude::*;

include_cpp! {
    #include "rocksdb/env.h"
    safety!(unsafe)

    generate!("rocksdb::Rusty")
}

pub fn hello_world() -> &'static str {
    "hello, world!"
}

pub fn hello_world_cxx() -> String {
    let user = Rusty::new().within_unique_ptr();
    let value = user.as_ref().unwrap().HelloWorld();
    value.to_string()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_hello_world() {
        assert_eq!(hello_world(), "hello, world!");
    }

    #[test]
    fn test_hello_world_from_cpp() {
        assert_eq!(hello_world_cxx(), "Hello World from C++!");
    }
}
