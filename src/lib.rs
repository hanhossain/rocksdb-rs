use autocxx::prelude::*;

include_cpp! {
    #include "rocksdb/env.h"
    safety!(unsafe)

    generate!("rocksdb::Rusty")
}

pub fn hello_world(caller: &str) {
    println!("Hello world from rust! I was called from {caller}.");
}

pub fn add(left: usize, right: usize) -> usize {
    left + right
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ffi::rocksdb::Rusty;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }

    #[test]
    fn hello_rusty() {
        let rusty = Rusty::new().within_unique_ptr();
        let value = rusty.HelloWorld();
        let value = value.to_string();
        assert_eq!(value, "Hello World from C++!");
    }
}
