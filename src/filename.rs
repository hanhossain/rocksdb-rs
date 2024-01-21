#[cxx::bridge(namespace = "rocksdb::rs")]
mod ffi {
    extern "Rust" {
        // TODO: remove export
        fn make_file_name(number: u64, suffix: &str) -> String;
        // TODO: remove export
        fn make_file_name_full_path(name: &str, number: u64, suffix: &str) -> String;
        #[cxx_name = "LogFileName"]
        fn log_file_name(number: u64) -> String;
        #[cxx_name = "LogFileName"]
        fn log_file_name_full_path(name: &str, number: u64) -> String;
    }
}

fn make_file_name(number: u64, suffix: &str) -> String {
    format!("{:06}.{}", number, suffix)
}

fn make_file_name_full_path(name: &str, number: u64, suffix: &str) -> String {
    format!("{}/{}", name, make_file_name(number, suffix))
}

fn log_file_name(number: u64) -> String {
    assert!(number > 0);
    make_file_name(number, "log")
}

fn log_file_name_full_path(name: &str, number: u64) -> String {
    assert!(number > 0);
    make_file_name_full_path(name, number, "log")
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn make_file_name_simple() {
        assert_eq!(make_file_name(1, "simple"), "000001.simple");
    }

    #[test]
    fn make_file_name_full_path_simple() {
        assert_eq!(
            make_file_name_full_path("test", 1, "simple"),
            "test/000001.simple"
        );
    }
}
