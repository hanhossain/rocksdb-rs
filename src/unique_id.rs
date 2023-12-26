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
