use cxx::CxxVector;

const ROCKSDB_BLOB_FILE_EXT: &str = "blob";
const ROCKSDB_TFILE_EXT: &str = "sst";
const ARCHIVAL_DIR_NAME: &str = "archive";
const LEVELDB_TFILE_EXT: &str = "ldb";
const CURRENT_FILE_NAME: &str = "CURRENT";
const TEMP_FILE_NAME_SUFFIX: &str = "dbtmp";
const OPTIONS_FILE_NAME_PREFIX: &str = "OPTIONS-";

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
        #[cxx_name = "FormatFileNumber"]
        fn format_file_number(number: u64, path_id: u32) -> String;
        /// Return the name of the current file. This file contains the name of the current manifest
        /// file. The result will be prefixed with `dbname`.
        #[cxx_name = "CurrentFileName"]
        fn current_file_name(dbname: &str) -> String;
        /// Return the name of the lock file for the db named by `dbname`. The result will be prefixed with
        /// `dbname`.
        #[cxx_name = "LockFileName"]
        fn lock_file_name(dbname: &str) -> String;
        /// Return the name of a temporary file owned by the db named `dbname`. The result will be prefixed
        /// with `dbname`.
        #[cxx_name = "TempFileName"]
        fn temp_file_name(dbname: &str, number: u64) -> String;
        #[cxx_name = "OptionsFileName"]
        fn options_file_name(file_num: u64) -> String;
        /// Return a options file name given the `dbname` and file number. Format: OPTIONS-{number}.dbtmp
        #[cxx_name = "OptionsFileName"]
        fn options_file_name_full_path(dbname: &str, file_num: u64) -> String;
        #[cxx_name = "TempOptionsFileName"]
        /// Return a temp options file name given the "dbname" and file number.
        /// Format: OPTIONS-{number}
        fn temp_options_file_name(dbname: &str, file_num: u64) -> String;
        /// Return the name to use for a metadatabase. The result will be prefixed with `dbname`.
        #[cxx_name = "MetaDatabaseName"]
        fn meta_database_name(dbname: &str, number: u64) -> String;
        /// Return the name of the Identity file which stores a unique number for the db that will get
        /// regenerated if the db loses all its data and is recreated fresh either from a backup-image or
        /// empty
        #[cxx_name = "IdentityFileName"]
        fn identity_file_name(dbname: &str) -> String;
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
    let re = regex::Regex::new(r"(\d+)\.[^.]*$").unwrap();
    re.captures(name)
        .map(|cap| cap[1].parse().unwrap())
        .unwrap_or(0)
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

fn format_file_number(number: u64, path_id: u32) -> String {
    if path_id == 0 {
        number.to_string()
    } else {
        format!("{}(path {})", number, path_id)
    }
}

/// Return the name of the current file. This file contains the name of the current manifest file.
/// The result will be prefixed with `dbname`.
fn current_file_name(dbname: &str) -> String {
    format!("{}/{}", dbname, CURRENT_FILE_NAME)
}

/// Return the name of the lock file for the db named by `dbname`. The result will be prefixed with
/// `dbname`.
fn lock_file_name(dbname: &str) -> String {
    format!("{}/LOCK", dbname)
}

/// Return the name of a temporary file owned by the db named `dbname`. The result will be prefixed
/// with `dbname`.
fn temp_file_name(dbname: &str, number: u64) -> String {
    make_file_name_full_path(dbname, number, TEMP_FILE_NAME_SUFFIX)
}

fn options_file_name(file_num: u64) -> String {
    format!("{}{:06}", OPTIONS_FILE_NAME_PREFIX, file_num)
}

/// Return a options file name given the `dbname` and file number. Format: OPTIONS-{number}.dbtmp
fn options_file_name_full_path(dbname: &str, file_num: u64) -> String {
    format!("{}/{}", dbname, options_file_name(file_num))
}

/// Return a temp options file name given the "dbname" and file number. Format: OPTIONS-{number}
fn temp_options_file_name(dbname: &str, file_num: u64) -> String {
    format!(
        "{}/{}{:06}.{}",
        dbname, OPTIONS_FILE_NAME_PREFIX, file_num, TEMP_FILE_NAME_SUFFIX
    )
}

/// Return the name to use for a metadatabase. The result will be prefixed with `dbname`.
fn meta_database_name(dbname: &str, number: u64) -> String {
    format!("{}/METADB-{}", dbname, number)
}

/// Return the name of the Identity file which stores a unique number for the db that will get
/// regenerated if the db loses all its data and is recreated fresh either from a backup-image or
/// empty
fn identity_file_name(dbname: &str) -> String {
    format!("{}/IDENTITY", dbname)
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

    #[test]
    fn test_table_file_name_to_number() {
        assert_eq!(table_file_name_to_number("123.sst"), 123);
        assert_eq!(table_file_name_to_number("1.sst"), 1);
        assert_eq!(table_file_name_to_number(".sst"), 0);
        assert_eq!(table_file_name_to_number("sst"), 0);
    }
}
