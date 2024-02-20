const ROCKSDB_BLOB_FILE_EXT: &str = "blob";
const ROCKSDB_TFILE_EXT: &str = "sst";
const ARCHIVAL_DIR_NAME: &str = "archive";

#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
    extern "Rust" {
        // TODO: remove export
        #[namespace = "rocksdb::rs"]
        fn make_file_name(number: u64, suffix: &str) -> String;
        // TODO: remove export
        #[namespace = "rocksdb::rs"]
        fn make_file_name_full_path(name: &str, number: u64, suffix: &str) -> String;
        #[cxx_name = "LogFileName"]
        fn log_file_name(number: u64) -> String;
        #[cxx_name = "LogFileName"]
        fn log_file_name_full_path(dbname: &str, number: u64) -> String;
        #[cxx_name = "BlobFileName"]
        fn blob_file_name(number: u64) -> String;
        #[cxx_name = "BlobFileName"]
        fn blob_file_name_dir_name(blob_dir_name: &str, number: u64) -> String;
        #[cxx_name = "BlobFileName"]
        fn blob_file_name_full_path(dbname: &str, blob_dir: &str, number: u64) -> String;
        #[cxx_name = "ArchivalDirectory"]
        fn archival_directory(dir: &str) -> String;
        ///  Return the name of the archived log file with the specified number
        ///  in the db named by `dbname`. The result will be prefixed with `dbname`.
        #[cxx_name = "ArchivedLogFileName"]
        fn archived_log_file_name(name: &str, number: u64) -> String;
        #[cxx_name = "MakeTableFileName"]
        fn make_table_file_name(number: u64) -> String;
        #[cxx_name = "MakeTableFileName"]
        fn make_table_file_name_full_path(path: &str, number: u64) -> String;
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

/// Return the name of the log file with the specified number in the db named by "dbname". The result will be prefixed
/// with "dbname".
fn log_file_name_full_path(dbname: &str, number: u64) -> String {
    assert!(number > 0);
    make_file_name_full_path(dbname, number, "log")
}

fn blob_file_name(number: u64) -> String {
    assert!(number > 0);
    make_file_name(number, ROCKSDB_BLOB_FILE_EXT)
}

fn blob_file_name_dir_name(blob_dir_name: &str, number: u64) -> String {
    assert!(number > 0);
    make_file_name_full_path(blob_dir_name, number, ROCKSDB_BLOB_FILE_EXT)
}

fn blob_file_name_full_path(dbname: &str, blob_dir: &str, number: u64) -> String {
    assert!(number > 0);
    make_file_name_full_path(
        &format!("{}/{}", dbname, blob_dir),
        number,
        ROCKSDB_BLOB_FILE_EXT,
    )
}

fn archival_directory(dir: &str) -> String {
    format!("{}/{}", dir, ARCHIVAL_DIR_NAME)
}

///  Return the name of the archived log file with the specified number
///  in the db named by `dbname`. The result will be prefixed with `dbname`.
fn archived_log_file_name(dbname: &str, number: u64) -> String {
    assert!(number > 0);
    make_file_name_full_path(&format!("{}/{}", dbname, ARCHIVAL_DIR_NAME), number, "log")
}

fn make_table_file_name(number: u64) -> String {
    make_file_name(number, ROCKSDB_TFILE_EXT)
}

fn make_table_file_name_full_path(path: &str, number: u64) -> String {
    make_file_name_full_path(path, number, ROCKSDB_TFILE_EXT)
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
