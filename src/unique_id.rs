use crate::status::{NotSupported, Status};
use crate::string_util::{parse_base_chars, put_base_chars};
use crate::unique_id::ffi::{UniqueId64x2, UniqueId64x3, UniqueIdPtr};
use cxx::CxxString;

#[cxx::bridge(namespace = "rocksdb")]
pub mod ffi {
    /// Standard size unique ID, good enough for almost all practical purposes
    #[derive(Debug, Copy, Clone, Eq, PartialEq, Default)]
    struct UniqueId64x2 {
        data: [u64; 2],
    }

    /// Extended size unique ID, for extra certainty of uniqueness among SST files
    /// spanning many hosts over a long time (rarely if ever needed)
    #[derive(Debug, Copy, Clone, Eq, PartialEq, Default)]
    struct UniqueId64x3 {
        data: [u64; 3],
    }

    // TODO: Should find a way to let this have a lifetime annotation.
    // TODO: This should not be copy/clone but the original C++ depends on copy semantics.
    /// Dynamic pointer wrapper for UniqueId64x2 and UniqueId64x3
    #[derive(Debug, Copy, Clone, Eq, PartialEq)]
    struct UniqueIdPtr {
        ptr: *mut u64,
        extended: bool,
    }

    extern "C++" {
        // TODO: build from rust
        // include!("rocksdb-rs/src/status.rs.h");

        // build from c++
        include!("rocksdb-rs-cxx/status.h");

        type Status = crate::status::ffi::Status;
    }

    extern "Rust" {
        fn as_unique_id_ptr(self: &mut UniqueId64x2) -> UniqueIdPtr;
        fn as_unique_id_ptr(self: &mut UniqueId64x3) -> UniqueIdPtr;

        #[cxx_name = "UniqueId64x2_null"]
        fn unique_id_64_x2_null() -> UniqueId64x2;
        #[cxx_name = "UniqueId64x3_null"]
        fn unique_id_64_x3_null() -> UniqueId64x3;

        fn to_internal_human_string(self: &UniqueId64x2) -> String;
        fn to_internal_human_string(self: &UniqueId64x3) -> String;

        #[cxx_name = "UniqueIdToHumanString"]
        fn unique_id_to_human_string(id: &CxxString) -> String;

        #[cxx_name = "EncodeSessionId"]
        fn encode_session_id(upper: u64, lower: u64) -> String;

        fn decode_session_id(db_session_id: &str, upper: &mut u64, lower: &mut u64) -> Status;
    }
}

impl UniqueId64x2 {
    fn as_unique_id_ptr(&mut self) -> UniqueIdPtr {
        UniqueIdPtr {
            ptr: self.data.as_mut_ptr(),
            extended: false,
        }
    }

    /// For presenting internal IDs for debugging purposes. Visually distinct from
    /// UniqueIdToHumanString for external IDs.
    fn to_internal_human_string(&self) -> String {
        format!("{{{},{}}}", self.data[0], self.data[1])
    }
}

impl UniqueId64x3 {
    fn as_unique_id_ptr(&mut self) -> UniqueIdPtr {
        UniqueIdPtr {
            ptr: self.data.as_mut_ptr(),
            extended: true,
        }
    }

    /// For presenting internal IDs for debugging purposes. Visually distinct from
    /// UniqueIdToHumanString for external IDs.
    fn to_internal_human_string(&self) -> String {
        format!("{{{},{},{}}}", self.data[0], self.data[1], self.data[2])
    }
}

// Value never used as an actual unique ID so can be used for "null"
fn unique_id_64_x2_null() -> UniqueId64x2 {
    UniqueId64x2::default()
}

// Value never used as an actual unique ID so can be used for "null"
fn unique_id_64_x3_null() -> UniqueId64x3 {
    UniqueId64x3::default()
}

/// Converts a binary string (unique id) to hexadecimal, with each 64 bits
/// separated by '-', e.g. 6474DF650323BDF0-B48E64F3039308CA-17284B32E7F7444B
/// Also works on unique id prefix.
fn unique_id_to_human_string(id: &CxxString) -> String {
    // convert to hex
    let mut hex = String::new();
    let mut counter = 0;
    for c in id.as_bytes() {
        if counter > 0 && counter % 8 == 0 {
            hex.push('-');
        }
        hex.push_str(&format!("{:02X}", c));
        counter += 1;
    }
    hex
}

/// Reformat a random value down to our "DB session id" format,
/// which is intended to be compact and friendly for use in file names.
/// `lower` is fully preserved and data is lost from `upper`.
///
/// Detail: Encoded into 20 chars in base-36 ([0-9A-Z]), which is ~103 bits of
/// entropy, which is enough to expect no collisions across a billion servers
/// each opening DBs a million times (~2^50). Benefits vs. RFC-4122 unique id:
/// * Save ~ dozen bytes per SST file
/// * Shorter shared backup file names (some platforms have low limits)
/// * Visually distinct from DB id format (usually RFC-4122)
fn encode_session_id(upper: u64, lower: u64) -> String {
    let mut db_session_id = String::with_capacity(20);
    // Preserving `lower` is slightly tricky. 36^12 is slightly more than
    // 62 bits, so we use 12 chars plus the bottom two bits of one more.
    // (A tiny fraction of 20 digit strings go unused.)
    let a = (upper << 2) | (lower >> 62);
    let b = lower & (u64::MAX >> 2);
    put_base_chars(&mut db_session_id, 8, a, true);
    put_base_chars(&mut db_session_id, 12, b, true);
    db_session_id
}

fn decode_session_id(
    db_session_id: &str,
    upper: &mut u64,
    lower: &mut u64,
) -> crate::status::ffi::Status {
    let len = db_session_id.len();
    if len == 0 {
        return Status::NotSupported(NotSupported {
            msg: "Missing db_session_id".to_owned(),
        })
        .into();
    }

    // Anything from 13 to 24 chars is reasonable. We don't have to limit to exactly 20.
    if len < 13 {
        return Status::NotSupported(NotSupported {
            msg: "Too short db_session_id".to_owned(),
        })
        .into();
    }

    if len > 24 {
        return Status::NotSupported(NotSupported {
            msg: "Too long db_session_id".to_owned(),
        })
        .into();
    }

    let mut a = 0;
    let mut b = 0;

    if !parse_base_chars(&db_session_id[..len - 12], 36, &mut a) {
        return Status::NotSupported(NotSupported {
            msg: "Bad digit in db_session_id".to_owned(),
        })
        .into();
    }

    if !parse_base_chars(&db_session_id[len - 12..], 36, &mut b) {
        return Status::NotSupported(NotSupported {
            msg: "Bad digit in db_session_id".to_owned(),
        })
        .into();
    }

    *upper = a >> 2;
    *lower = (b & (u64::MAX >> 2)) | (a << 62);

    Status::Ok.into()
}
