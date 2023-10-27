#[cxx::bridge]
mod ffi {
    extern "Rust" {
        fn hello_world(caller: &str);
    }
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

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}
