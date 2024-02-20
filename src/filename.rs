use cxx::CxxVector;

const ROCKSDB_BLOB_FILE_EXT: &str = "blob";
const ROCKSDB_TFILE_EXT: &str = "sst";
const ARCHIVAL_DIR_NAME: &str = "archive";
const LEVELDB_TFILE_EXT: &str = "ldb";

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
        /// Return the name of sstable with LevelDB suffix created from RocksDB sstable suffixed name
        #[cxx_name = "Rocks2LevelTableFileName"]
        fn rocks_to_level_table_file_name(fullname: &str) -> String;
        /// The reverse function of MakeTableFileName
        #[cxx_name = "TableFileNameToNumber"]
        fn table_file_name_to_number(name: &str) -> u64;
        #[cxx_name = "DescriptorFileName"]
        fn descriptor_file_name(number: u64) -> String;
        /// Return the name of the descriptor file for the db named by `dbname` and the specified
        /// incarnation number. The result will be prefixed with `dbname`.
        #[cxx_name = "DescriptorFileName"]
        fn descriptor_file_name_full_path(dbname: &str, number: u64) -> String;
        /// Return the name of the sstable with the specified number in the db named by `dbname`.
        /// The result will be prefixed with `dbname`.
        #[cxx_name = "TableFileName"]
        fn table_file_name(db_paths: &CxxVector<DbPath>, number: u64, path_id: u32) -> String;
    }

    unsafe extern "C++" {
        include!("rocksdb/options.h");

        type DbPath = crate::options::ffi::DbPath;
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

/// Return the name of sstable with LevelDB suffix created from RocksDB sstable suffixed name
fn rocks_to_level_table_file_name(fullname: &str) -> String {
    assert!(fullname.len() > ROCKSDB_TFILE_EXT.len() + 1);
    if fullname.len() <= ROCKSDB_TFILE_EXT.len() + 1 {
        return "".to_string();
    }
    let mut res = fullname[0..fullname.len() - ROCKSDB_TFILE_EXT.len()].to_string();
    res.push_str(LEVELDB_TFILE_EXT);
    res
}

/// The reverse function of MakeTableFileName
// TODO(yhchiang): could merge this function with ParseFileName()
fn table_file_name_to_number(name: &str) -> u64 {
    let mut number = 0;
    let mut base = 1;
    let mut pos = name.rfind('.').unwrap();
    while pos > 0 && name.chars().nth(pos - 1).unwrap().is_ascii_digit() {
        number += (name.chars().nth(pos - 1).unwrap() as u64 - '0' as u64) * base;
        base *= 10;
        pos -= 1;
    }
    number
}

fn descriptor_file_name(number: u64) -> String {
    assert!(number > 0);
    format!("MANIFEST-{:06}", number)
}

/// Return the name of the descriptor file for the db named by `dbname` and the specified
/// incarnation number. The result will be prefixed with `dbname`.
fn descriptor_file_name_full_path(dbname: &str, number: u64) -> String {
    format!("{}/{}", dbname, descriptor_file_name(number))
}

/// Return the name of the sstable with the specified number in the db named by `dbname`.
/// The result will be prefixed with `dbname`.
fn table_file_name(db_paths: &CxxVector<ffi::DbPath>, number: u64, path_id: u32) -> String {
    assert!(number > 0);
    let mut idx = path_id as usize;
    if idx >= db_paths.len() {
        idx = db_paths.len() - 1;
    }
    let path = db_paths.get(idx).unwrap().path();
    make_table_file_name_full_path(path.to_str().unwrap(), number)
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

    #[test]
    fn make_table_file_name_simple() {
        let table_file_name = make_table_file_name(987654);
        assert_eq!(table_file_name, "987654.sst");
        assert_eq!(table_file_name_to_number(&table_file_name), 987654);
    }
}
