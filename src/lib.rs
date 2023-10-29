#[cxx::bridge(namespace = "rocksdb")]
pub mod ffi {

    unsafe extern "C++" {
        include!("rocksdb/env.h");

        type Rusty;

        #[cxx_name = "NewRusty"]
        fn rusty_new() -> UniquePtr<Rusty>;

        //     #[cxx_name = "HelloWorld"]
        //     fn hello_world(&self) -> UniquePtr<CxxString>;
    }
}

pub fn add(left: usize, right: usize) -> usize {
    left + right
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }

    #[test]
    fn hello_rusty() {
        let _rusty = ffi::rusty_new();
        // let rusty = ffi::Rusty_new();
        // let value = rusty.HelloWorld();
        // let value = value.to_string();
        // assert_eq!(value, "Hello World from C++!");
    }
}
