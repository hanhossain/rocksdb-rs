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
        #[cxx_name = "IOStatus_new"]
        fn io_status_new3() -> IOStatus;

        #[cxx_name = "SetRetryable"]
        fn set_retryable(self: &mut IOStatus, retryable: bool);
        #[cxx_name = "SetDataLoss"]
        fn set_data_loss(self: &mut IOStatus, data_loss: bool);
        #[cxx_name = "SetScope"]
        fn set_scope(self: &mut IOStatus, scope: IOErrorScope);
        #[cxx_name = "GetRetryable"]
        fn get_retryable(self: &IOStatus) -> bool;
        #[cxx_name = "GetDataLoss"]
        fn get_data_loss(self: &IOStatus) -> bool;
        #[cxx_name = "GetScope"]
        fn get_scope(self: &IOStatus) -> IOErrorScope;
    }

    #[namespace = "rocksdb_rs::status"]
    unsafe extern "C++" {
        include!("rocksdb-rs/src/status.rs.h");

        type Status = crate::status::ffi::Status;
        type Code = crate::status::ffi::Code;
        type SubCode = crate::status::ffi::SubCode;
    }
}

impl ffi::IOStatus {
    fn set_retryable(&mut self, retryable: bool) {
        self.status.retryable = retryable;
    }

    fn set_data_loss(&mut self, data_loss: bool) {
        self.status.data_loss = data_loss;
    }

    fn set_scope(&mut self, scope: ffi::IOErrorScope) {
        self.status.scope = scope.repr;
    }

    fn get_retryable(&self) -> bool {
        self.status.retryable
    }

    fn get_data_loss(&self) -> bool {
        self.status.data_loss
    }

    fn get_scope(&self) -> ffi::IOErrorScope {
        match self.status.scope {
            0 => ffi::IOErrorScope::kIOErrorScopeFileSystem,
            1 => ffi::IOErrorScope::kIOErrorScopeFile,
            2 => ffi::IOErrorScope::kIOErrorScopeRange,
            3 => ffi::IOErrorScope::kIOErrorScopeMax,
            _ => panic!("Invalid scope"),
        }
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

fn io_status_new3() -> ffi::IOStatus {
    ffi::IOStatus {
        status: ffi::Status::default(),
    }
}
