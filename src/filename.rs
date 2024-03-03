use crate::filename::ffi::{FileType, InfoLogPrefix, WalFileType};
use cxx::CxxVector;
use lazy_static::lazy_static;
use regex::Regex;

const ROCKSDB_BLOB_FILE_EXT: &str = "blob";
const ROCKSDB_TFILE_EXT: &str = "sst";
const ARCHIVAL_DIR_NAME: &str = "archive";
const LEVELDB_TFILE_EXT: &str = "ldb";
const CURRENT_FILE_NAME: &str = "CURRENT";
const TEMP_FILE_NAME_SUFFIX: &str = "dbtmp";
const OPTIONS_FILE_NAME_PREFIX: &str = "OPTIONS-";

#[cxx::bridge(namespace = "rocksdb_rs::filename")]
mod ffi {
    /// A helper structure for prefix of info log names.
    struct InfoLogPrefix {
        prefix: String,
    }

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
        #[cxx_name = "NormalizePath"]
        fn normalize_path(path: &str) -> String;
        /// Prefix with DB absolute path encoded
        #[cxx_name = "InfoLogPrefix_new"]
        fn info_log_prefix_new(has_log_dir: bool, db_absolute_path: &str) -> InfoLogPrefix;
        /// Return the name of the info log file for `dbname`.
        #[cxx_name = "InfoLogFileName"]
        fn info_log_file_name(dbname: &str, db_path: &str, log_dir: &str) -> String;
        /// Return the name of the old info log file for `dbname`.
        #[cxx_name = "OldInfoLogFileName"]
        fn old_info_log_file_name(dbname: &str, ts: u64, db_path: &str, log_dir: &str) -> String;

        /// If filename is a rocksdb file, store the type of the file in *type.
        /// The number encoded in the filename is stored in *number. If the
        /// filename was successfully parsed, returns true. Else return false.
        /// info_log_name_prefix is the path of info logs.
        #[cxx_name = "ParseFileName"]
        unsafe fn parse_file_name_with_info_log_prefix_and_log_type(
            file_name: &str,
            number: *mut u64,
            info_log_name_prefix: &str,
            file_type: *mut FileType,
            log_type: *mut WalFileType,
        ) -> bool;

        /// If filename is a rocksdb file, store the type of the file in *type.
        /// The number encoded in the filename is stored in *number. If the
        /// filename was successfully parsed, returns true. Else return false.
        /// info_log_name_prefix is the path of info logs.
        #[cxx_name = "ParseFileName"]
        unsafe fn parse_file_name_with_info_log_prefix(
            file_name: &str,
            number: *mut u64,
            info_log_name_prefix: &str,
            file_type: *mut FileType,
        ) -> bool;

        /// If filename is a rocksdb file, store the type of the file in *type.
        /// The number encoded in the filename is stored in *number. If the
        /// filename was successfully parsed, returns true. Else return false.
        /// Skips info log files.
        #[cxx_name = "ParseFileName"]
        unsafe fn parse_file_name_with_log_type(
            file_name: &str,
            number: *mut u64,
            file_type: *mut FileType,
            log_type: *mut WalFileType,
        ) -> bool;

        /// If filename is a rocksdb file, store the type of the file in *type.
        /// The number encoded in the filename is stored in *number. If the
        /// filename was successfully parsed, returns true. Else return false.
        /// Skips info log files.
        #[cxx_name = "ParseFileName"]
        unsafe fn parse_file_name(
            file_name: &str,
            number: *mut u64,
            file_type: *mut FileType,
        ) -> bool;
    }

    #[namespace = "rocksdb"]
    unsafe extern "C++" {
        include!("rocksdb/options.h");
        include!("rocksdb-rs/src/transaction_log.rs.h");

        type DbPath = crate::options::ffi::DbPath;

        #[namespace = "rocksdb_rs::types"]
        type FileType = crate::types::ffi::FileType;

        #[namespace = "rocksdb_rs::transaction_log"]
        type WalFileType = crate::transaction_log::ffi::WalFileType;
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
    lazy_static! {
        static ref RE: Regex = Regex::new(r"(\d+)\.[^.]*$").unwrap();
    }
    RE.captures(name)
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

fn normalize_path(path: &str) -> String {
    lazy_static! {
        static ref RE: Regex = Regex::new("/+").unwrap();
    }

    let mut normalized = String::new();

    let path = if path.len() > 2 && &path[..1] == "/" {
        normalized.push('/');
        &path[1..]
    } else {
        path
    };

    let replaced = RE.replace_all(path, "/");
    normalized.push_str(&replaced);

    normalized
}

/// Given a path, flatten the path name by replacing all chars not in {[0-9,a-z,A-Z,-,_,.]} with _.
fn get_info_log_prefix(path: &str) -> String {
    lazy_static! {
        static ref RE: Regex = Regex::new("[^a-zA-Z0-9-._]").unwrap();
    }

    if path.is_empty() {
        return "".to_string();
    }

    let replaced = if RE.is_match(&path[..1]) && path.len() > 1 {
        RE.replace_all(&path[1..], "_")
    } else {
        RE.replace_all(path, "_")
    };
    format!("{replaced}_LOG")
}

impl InfoLogPrefix {
    /// Prefix with DB absolute path encoded
    fn new(has_log_dir: bool, db_absolute_path: &str) -> Self {
        let prefix = if has_log_dir {
            get_info_log_prefix(&normalize_path(db_absolute_path))
        } else {
            "LOG".to_string()
        };
        InfoLogPrefix { prefix }
    }
}

/// Prefix with DB absolute path encoded
fn info_log_prefix_new(has_log_dir: bool, db_absolute_path: &str) -> InfoLogPrefix {
    InfoLogPrefix::new(has_log_dir, db_absolute_path)
}

/// Return the name of the info log file for `dbname`.
fn info_log_file_name(dbname: &str, db_path: &str, log_dir: &str) -> String {
    if log_dir.is_empty() {
        return format!("{}/LOG", dbname);
    }

    let info_log_prefix = InfoLogPrefix::new(true, db_path);
    format!("{}/{}", log_dir, info_log_prefix.prefix)
}

/// Return the name of the old info log file for `dbname`.
fn old_info_log_file_name(dbname: &str, ts: u64, db_path: &str, log_dir: &str) -> String {
    if log_dir.is_empty() {
        return format!("{}/LOG.old.{}", dbname, ts);
    }

    let info_log_prefix = InfoLogPrefix::new(true, db_path);
    format!("{}/{}.old.{}", log_dir, info_log_prefix.prefix, ts)
}

/// If filename is a rocksdb file, store the type of the file in *type.
/// The number encoded in the filename is stored in *number. If the
/// filename was successfully parsed, returns true. Else return false.
/// info_log_name_prefix is the path of info logs.
unsafe fn parse_file_name_with_info_log_prefix_and_log_type(
    file_name: &str,
    number: *mut u64,
    info_log_name_prefix: &str,
    file_type: *mut FileType,
    log_type: *mut WalFileType,
) -> bool {
    if let Some(result) = parse_with_info_log_prefix(file_name, info_log_name_prefix) {
        *number = result.number;
        *file_type = result.file_type;
        if let Some(log) = result.log_type {
            *log_type = log;
        }
        true
    } else {
        false
    }
}

/// If filename is a rocksdb file, store the type of the file in *type.
/// The number encoded in the filename is stored in *number. If the
/// filename was successfully parsed, returns true. Else return false.
/// info_log_name_prefix is the path of info logs.
unsafe fn parse_file_name_with_info_log_prefix(
    file_name: &str,
    number: *mut u64,
    info_log_name_prefix: &str,
    file_type: *mut FileType,
) -> bool {
    if let Some(result) = parse_with_info_log_prefix(file_name, info_log_name_prefix) {
        *number = result.number;
        *file_type = result.file_type;
        true
    } else {
        false
    }
}

/// If filename is a rocksdb file, store the type of the file in *type.
/// The number encoded in the filename is stored in *number. If the
/// filename was successfully parsed, returns true. Else return false.
/// Skips info log files.
unsafe fn parse_file_name_with_log_type(
    file_name: &str,
    number: *mut u64,
    file_type: *mut FileType,
    log_type: *mut WalFileType,
) -> bool {
    if let Some(result) = parse(file_name) {
        *number = result.number;
        *file_type = result.file_type;
        if let Some(log) = result.log_type {
            *log_type = log;
        }
        true
    } else {
        false
    }
}

/// If filename is a rocksdb file, store the type of the file in *type.
/// The number encoded in the filename is stored in *number. If the
/// filename was successfully parsed, returns true. Else return false.
/// Skips info log files.
unsafe fn parse_file_name(file_name: &str, number: *mut u64, file_type: *mut FileType) -> bool {
    if let Some(result) = parse(file_name) {
        *number = result.number;
        *file_type = result.file_type;
        true
    } else {
        false
    }
}

struct ParseResult {
    number: u64,
    file_type: FileType,
    log_type: Option<WalFileType>,
}

/// If filename is a rocksdb file, store the type of the file in *type.
/// The number encoded in the filename is stored in *number. If the
/// filename was successfully parsed, returns true. Else return false.
/// Skips info log files.
///
/// Owned filenames have the form:
///    dbname/IDENTITY
///    dbname/CURRENT
///    dbname/LOCK
///    dbname/<info_log_name_prefix>
///    dbname/<info_log_name_prefix>.old.[0-9]+
///    dbname/MANIFEST-[0-9]+
///    dbname/[0-9]+.(log|sst|blob)
///    dbname/METADB-[0-9]+
///    dbname/OPTIONS-[0-9]+
///    dbname/OPTIONS-[0-9]+.dbtmp
///    Disregards / at the beginning
fn parse(file_name: &str) -> Option<ParseResult> {
    parse_with_info_log_prefix(file_name, "")
}

/// If filename is a rocksdb file, store the type of the file in *type.
/// The number encoded in the filename is stored in *number. If the
/// filename was successfully parsed, returns true. Else return false.
/// info_log_name_prefix is the path of info logs.
///
/// Owned filenames have the form:
///    dbname/IDENTITY
///    dbname/CURRENT
///    dbname/LOCK
///    dbname/<info_log_name_prefix>
///    dbname/<info_log_name_prefix>.old.[0-9]+
///    dbname/MANIFEST-[0-9]+
///    dbname/[0-9]+.(log|sst|blob)
///    dbname/METADB-[0-9]+
///    dbname/OPTIONS-[0-9]+
///    dbname/OPTIONS-[0-9]+.dbtmp
///    Disregards / at the beginning
fn parse_with_info_log_prefix(file_name: &str, info_log_name_prefix: &str) -> Option<ParseResult> {
    lazy_static! {
        static ref OLD_LOG_FILE_REGEX: Regex = Regex::new(r"^(\.old)?$").unwrap();
        static ref OLD_LOG_FILE_WITH_NUM_REGEX: Regex = Regex::new(r"^\.old\.(\d+)$").unwrap();
        static ref MANIFEST_OR_METADB_FILE_REGEX: Regex =
            Regex::new(r"^(MANIFEST|METADB)-(\d+)$").unwrap();
        static ref OPTIONS_FILE_REGEX: Regex = Regex::new(r"^OPTIONS-(\d+)(\.dbtmp)?$").unwrap();
        static ref GENERAL_FILE_REGEX: Regex =
            Regex::new(r"^(archive/)?(\d+)\.(log|sst|ldb|blob|dbtmp)$").unwrap();
    }

    let mut rest = file_name;
    if file_name.len() > 1 && file_name.starts_with('/') {
        rest = &file_name[1..];
    }

    match rest {
        "IDENTITY" => {
            return Some(ParseResult {
                number: 0,
                file_type: FileType::kIdentityFile,
                log_type: None,
            });
        }
        "CURRENT" => {
            return Some(ParseResult {
                number: 0,
                file_type: FileType::kCurrentFile,
                log_type: None,
            });
        }
        "LOCK" => {
            return Some(ParseResult {
                number: 0,
                file_type: FileType::kDBLockFile,
                log_type: None,
            });
        }
        _ => (),
    };

    if !info_log_name_prefix.is_empty() && rest.starts_with(info_log_name_prefix) {
        let rest_log_name = &rest[info_log_name_prefix.len()..];

        if OLD_LOG_FILE_REGEX.is_match(rest_log_name) {
            return Some(ParseResult {
                number: 0,
                file_type: FileType::kInfoLogFile,
                log_type: None,
            });
        }

        let captures = OLD_LOG_FILE_WITH_NUM_REGEX.captures(rest_log_name)?;
        let number = captures.get(1).unwrap().as_str().parse().ok()?;
        return Some(ParseResult {
            number,
            file_type: FileType::kInfoLogFile,
            log_type: None,
        });
    }

    if let Some(captures) = MANIFEST_OR_METADB_FILE_REGEX.captures(&rest) {
        let number = captures.get(2).unwrap().as_str().parse().ok()?;
        let prefix = captures.get(1).unwrap().as_str();
        let file_type = match prefix {
            "MANIFEST" => FileType::kDescriptorFile,
            "METADB" => FileType::kMetaDatabase,
            _ => unreachable!(),
        };
        return Some(ParseResult {
            number,
            file_type,
            log_type: None,
        });
    }

    if let Some(captures) = OPTIONS_FILE_REGEX.captures(&rest) {
        let number = captures.get(1).unwrap().as_str().parse().ok()?;
        let file_type = match captures.get(2) {
            Some(_) => FileType::kTempFile,
            _ => FileType::kOptionsFile,
        };
        return Some(ParseResult {
            number,
            file_type,
            log_type: None,
        });
    }

    let captures = GENERAL_FILE_REGEX.captures(&rest)?;
    let number = captures.get(2).unwrap().as_str().parse().ok()?;
    let archive_dir_found = captures.get(1).is_some();
    let suffix = captures.get(3).unwrap().as_str();
    let (file_type, log_type) = match (suffix, archive_dir_found) {
        ("log", true) => (FileType::kWalFile, Some(WalFileType::kArchivedLogFile)),
        ("log", false) => (FileType::kWalFile, Some(WalFileType::kAliveLogFile)),
        ("sst" | "ldb", _) => (FileType::kTableFile, None),
        ("blob", _) => (FileType::kBlobFile, None),
        ("dbtmp", _) => (FileType::kTempFile, None),
        _ => unreachable!(),
    };

    Some(ParseResult {
        number,
        file_type,
        log_type,
    })
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

    #[test]
    fn test_normalize_path() {
        assert_eq!(normalize_path("a/b/c"), "a/b/c");
        assert_eq!(normalize_path("a//b/c"), "a/b/c");
        assert_eq!(normalize_path("//a/b/c"), "//a/b/c");
        assert_eq!(normalize_path("//a//b/c"), "//a/b/c");
        assert_eq!(normalize_path("///////a/////b/c"), "//a/b/c");
        assert_eq!(normalize_path("//"), "/");
    }
}
