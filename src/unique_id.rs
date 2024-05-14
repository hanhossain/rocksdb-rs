use crate::coding_lean::{decode_fixed_64, encode_fixed_64};
use crate::hash::{hash2x64, hash2x64_with_seed};
use crate::status::{NotSupported, Status};
use crate::string_util::{parse_base_chars, put_base_chars};
use crate::unique_id::ffix::{UniqueId64x2, UniqueId64x3, UniqueIdPtr};
use cxx::{CxxString, UniquePtr};

#[cxx::bridge(namespace = "rocksdb_rs::unique_id")]
pub mod ffix {
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
        include!("rocksdb-rs/src/status.rs.h");

        #[namespace = "rocksdb_rs::status"]
        type Status = crate::status::ffix::Status;
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

        #[cxx_name = "DecodeSessionId"]
        fn decode_session_id(db_session_id: &str, upper: &mut u64, lower: &mut u64) -> Status;

        fn encode_bytes(self: &UniqueId64x2) -> UniquePtr<CxxString>;
        fn encode_bytes(self: &UniqueId64x3) -> UniquePtr<CxxString>;

        fn decode_bytes(self: &mut UniqueId64x2, unique_id: &CxxString) -> Status;
        fn decode_bytes(self: &mut UniqueId64x3, unique_id: &CxxString) -> Status;

        fn get_sst_internal_unique_id(
            self: &mut UniqueId64x2,
            db_id: &CxxString,
            db_session_id: &str,
            file_number: u64,
            force: bool,
        ) -> Status;
        fn get_sst_internal_unique_id(
            self: &mut UniqueId64x3,
            db_id: &CxxString,
            db_session_id: &str,
            file_number: u64,
            force: bool,
        ) -> Status;
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

    /// Convert numerical format to byte format for public API
    fn encode_bytes(&self) -> UniquePtr<CxxString> {
        let mut res = crate::ffix::make_string();
        res.pin_mut().push_bytes(&encode_fixed_64(self.data[0]));
        res.pin_mut().push_bytes(&encode_fixed_64(self.data[1]));
        res
    }

    /// Reverse of encode_bytes.
    fn decode_bytes(&mut self, unique_id: &CxxString) -> crate::status::ffix::Status {
        if unique_id.len() != 16 {
            return Status::NotSupported(NotSupported {
                msg: "Not a valid unique_id".to_owned(),
            })
            .into();
        }

        self.data[0] = decode_fixed_64(&unique_id.as_bytes()[0..8]);
        self.data[1] = decode_fixed_64(&unique_id.as_bytes()[8..16]);
        Status::Ok.into()
    }

    /// Helper for GetUniqueIdFromTableProperties. This function can also be used
    /// for temporary ids for files without sufficient information in table
    /// properties. The internal unique id is more structured than the public
    /// unique id, so can be manipulated in more ways but very carefully.
    /// These must be long term stable to ensure GetUniqueIdFromTableProperties
    /// is long term stable.
    fn get_sst_internal_unique_id(
        &mut self,
        db_id: &CxxString,
        db_session_id: &str,
        file_number: u64,
        force: bool,
    ) -> crate::status::ffix::Status {
        let mut x3 = UniqueId64x3::default();
        let status = x3.get_sst_internal_unique_id(db_id, db_session_id, file_number, force);

        if status.ok() {
            self.data.copy_from_slice(&x3.data[..2]);
        }

        status
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

    /// Convert numerical format to byte format for public API
    fn encode_bytes(&self) -> UniquePtr<CxxString> {
        let mut res = crate::ffix::make_string();
        res.pin_mut().push_bytes(&encode_fixed_64(self.data[0]));
        res.pin_mut().push_bytes(&encode_fixed_64(self.data[1]));
        res.pin_mut().push_bytes(&encode_fixed_64(self.data[2]));
        res
    }

    /// Reverse of encode_bytes.
    fn decode_bytes(&mut self, unique_id: &CxxString) -> crate::status::ffix::Status {
        if unique_id.len() != 24 {
            return Status::NotSupported(NotSupported {
                msg: "Not a valid unique_id".to_owned(),
            })
            .into();
        }

        self.data[0] = decode_fixed_64(&unique_id.as_bytes()[0..8]);
        self.data[1] = decode_fixed_64(&unique_id.as_bytes()[8..16]);
        self.data[2] = decode_fixed_64(&unique_id.as_bytes()[16..24]);
        Status::Ok.into()
    }

    /// Helper for GetUniqueIdFromTableProperties. This function can also be used
    /// for temporary ids for files without sufficient information in table
    /// properties. The internal unique id is more structured than the public
    /// unique id, so can be manipulated in more ways but very carefully.
    /// These must be long term stable to ensure GetUniqueIdFromTableProperties
    /// is long term stable.
    fn get_sst_internal_unique_id(
        &mut self,
        db_id: &CxxString,
        db_session_id: &str,
        file_number: u64,
        force: bool,
    ) -> crate::status::ffix::Status {
        if !force {
            if db_id.is_empty() {
                return Status::NotSupported(NotSupported {
                    msg: "Missing db_id".to_owned(),
                })
                .into();
            }

            if file_number == 0 {
                return Status::NotSupported(NotSupported {
                    msg: "Missing or bad file number".to_owned(),
                })
                .into();
            }

            if db_session_id.is_empty() {
                return Status::NotSupported(NotSupported {
                    msg: "Missing db_session_id".to_owned(),
                })
                .into();
            }
        }

        let mut session_upper = 0;
        let mut session_lower = 0;

        let status = decode_session_id(db_session_id, &mut session_upper, &mut session_lower);
        if !status.ok() {
            if !force {
                return status;
            }

            // A reasonable fallback in case malformed
            (session_upper, session_lower) = hash2x64(db_session_id.as_bytes());
            if session_lower == 0 {
                session_lower = session_upper | 1;
            }
        }

        // Exactly preserve session lower to ensure that session ids generated
        // during the same process lifetime are guaranteed unique.
        // DBImpl also guarantees (in recent versions) that this is not zero,
        // so that we can guarantee unique ID is never all zeros. (Can't assert
        // that here because of testing and old versions.)
        // We put this first in anticipation of matching a small-ish set of cache
        // key prefixes to cover entries relevant to any DB.
        self.data[0] = session_lower;

        // Hash the session upper (~39 bits entropy) and DB id (120+ bits entropy)
        // for very high global uniqueness entropy.
        // (It is possible that many DBs descended from one common DB id are copied
        // around and proliferate, in which case session id is critical, but it is
        // more common for different DBs to have different DB ids.)
        let (db_a, db_b) = hash2x64_with_seed(db_id.as_bytes(), session_upper);

        // Xor in file number for guaranteed uniqueness by file number for a given
        // session and DB id. (Xor slightly better than + here. See
        // https://github.com/pdillinger/unique_id )
        self.data[1] = db_a ^ file_number;

        // Extra global uniqueness
        self.data[2] = db_b;

        Status::Ok.into()
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

/// Reverse of EncodeSessionId. Returns NotSupported on error rather than
/// Corruption because non-standard session IDs should be allowed with degraded
/// functionality.
fn decode_session_id(
    db_session_id: &str,
    upper: &mut u64,
    lower: &mut u64,
) -> crate::status::ffix::Status {
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
