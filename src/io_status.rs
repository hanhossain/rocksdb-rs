use crate::status::ffi::{Code, SubCode};

#[cxx::bridge(namespace = "rocksdb_rs::io_status")]
mod ffi {
    #[derive(Debug)]
    enum IOErrorScope {
        kIOErrorScopeFileSystem,
        kIOErrorScopeFile,
        kIOErrorScopeRange,
        kIOErrorScopeMax,
    }

    #[derive(Debug)]
    struct IOStatus {
        #[cxx_name = "status_"]
        status: Status,
    }

    extern "Rust" {
        #[cxx_name = "IOStatus_new"]
        fn io_status_new(status: Status) -> IOStatus;
        #[cxx_name = "IOStatus_new"]
        fn io_status_new1(code: Code, subcode: SubCode) -> IOStatus;
        #[cxx_name = "IOStatus_new"]
        fn io_status_new2(code: Code) -> IOStatus;
    }

    #[namespace = "rocksdb_rs::status"]
    unsafe extern "C++" {
        include!("rocksdb-rs/src/status.rs.h");

        type Status = crate::status::ffi::Status;
        type Code = crate::status::ffi::Code;
        type SubCode = crate::status::ffi::SubCode;
    }
}

fn io_status_new(status: ffi::Status) -> ffi::IOStatus {
    ffi::IOStatus { status }
}

fn io_status_new1(code: Code, subcode: SubCode) -> ffi::IOStatus {
    let status = ffi::Status {
        code,
        subcode,
        scope: ffi::IOErrorScope::kIOErrorScopeFileSystem.repr,
        ..ffi::Status::default()
    };
    ffi::IOStatus { status }
}

fn io_status_new2(code: Code) -> ffi::IOStatus {
    let status = ffi::Status {
        code,
        scope: ffi::IOErrorScope::kIOErrorScopeFileSystem.repr,
        ..ffi::Status::default()
    };
    ffi::IOStatus { status }
}
