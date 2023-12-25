use crate::unique_id::ffi::{UniqueId64x2, UniqueId64x3, UniqueIdPtr};

#[cxx::bridge(namespace = "rocksdb")]
pub mod ffi {
    /// Standard size unique ID, good enough for almost all practical purposes
    #[derive(Debug, Copy, Clone, Eq, PartialEq)]
    struct UniqueId64x2 {
        data: [u64; 2],
    }

    /// Extended size unique ID, for extra certainty of uniqueness among SST files
    /// spanning many hosts over a long time (rarely if ever needed)
    #[derive(Debug, Copy, Clone, Eq, PartialEq)]
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
    }
}

impl UniqueId64x2 {
    fn as_unique_id_ptr(&mut self) -> UniqueIdPtr {
        UniqueIdPtr {
            ptr: self.data.as_mut_ptr(),
            extended: false,
        }
    }
}

impl UniqueId64x3 {
    fn as_unique_id_ptr(&mut self) -> UniqueIdPtr {
        UniqueIdPtr {
            ptr: self.data.as_mut_ptr(),
            extended: true,
        }
    }
}
