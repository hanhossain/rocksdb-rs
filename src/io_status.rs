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
        fn ok(self: &IOStatus) -> bool;
        #[cxx_name = "IsNotFound"]
        fn is_not_found(self: &IOStatus) -> bool;
        #[cxx_name = "IsNotSupported"]
        fn is_not_supported(self: &IOStatus) -> bool;
        #[cxx_name = "IsIncomplete"]
        fn is_incomplete(self: &IOStatus) -> bool;
        #[cxx_name = "IsCorruption"]
        fn is_corruption(self: &IOStatus) -> bool;
        #[cxx_name = "IsBusy"]
        fn is_busy(self: &IOStatus) -> bool;
        #[cxx_name = "IsIOFenced"]
        fn is_io_fenced(self: &IOStatus) -> bool;
        #[cxx_name = "IsIOError"]
        fn is_io_error(self: &IOStatus) -> bool;
        #[cxx_name = "IsShutdownInProgress"]
        fn is_shutdown_in_progress(self: &IOStatus) -> bool;
        #[cxx_name = "IsColumnFamilyDropped"]
        fn is_column_family_dropped(self: &IOStatus) -> bool;
        #[cxx_name = "IsTryAgain"]
        fn is_try_again(self: &IOStatus) -> bool;
        #[cxx_name = "IsAborted"]
        fn is_aborted(self: &IOStatus) -> bool;
        #[cxx_name = "IsPathNotFound"]
        fn is_path_not_found(self: &IOStatus) -> bool;
        #[cxx_name = "IsInvalidArgument"]
        fn is_invalid_argument(self: &IOStatus) -> bool;
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

    fn ok(&self) -> bool {
        self.status.ok()
    }

    fn is_not_found(&self) -> bool {
        self.status.is_not_found()
    }

    fn is_not_supported(&self) -> bool {
        self.status.is_not_supported()
    }

    fn is_incomplete(&self) -> bool {
        self.status.is_incomplete()
    }

    fn is_corruption(&self) -> bool {
        self.status.is_corruption()
    }

    fn is_busy(&self) -> bool {
        self.status.is_busy()
    }

    fn is_io_fenced(&self) -> bool {
        self.status.is_io_fenced()
    }

    fn is_io_error(&self) -> bool {
        self.status.is_io_error()
    }

    fn is_shutdown_in_progress(&self) -> bool {
        self.status.is_shutdown_in_progress()
    }

    fn is_column_family_dropped(&self) -> bool {
        self.status.is_column_family_dropped()
    }

    fn is_try_again(&self) -> bool {
        self.status.is_try_again()
    }

    fn is_aborted(&self) -> bool {
        self.status.is_aborted()
    }

    fn is_path_not_found(&self) -> bool {
        self.status.is_path_not_found()
    }

    fn is_invalid_argument(&self) -> bool {
        self.status.is_invalid_argument()
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
