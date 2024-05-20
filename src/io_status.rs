use crate::io_status::ffix::{Code, SubCode};
use crate::status;
use cxx::{CxxString, UniquePtr};

#[cxx::bridge(namespace = "rocksdb_rs::io_status")]
mod ffix {
    #[derive(Debug)]
    enum IOErrorScope {
        kIOErrorScopeFileSystem,
        kIOErrorScopeFile,
        kIOErrorScopeRange,
        kIOErrorScopeMax,
    }

    #[derive(Debug, PartialEq, Clone)]
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
        #[cxx_name = "IOStatus_new"]
        fn io_status_new4(code: Code, subcode: SubCode, msg: &Slice, msg2: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_new"]
        fn io_status_new5(code: Code, msg: &Slice, msg2: &Slice) -> IOStatus;

        #[cxx_name = "IOStatus_OK"]
        fn io_status_ok() -> IOStatus;

        #[cxx_name = "IOStatus_NotSupported"]
        fn io_status_not_supported(msg: &Slice, msg2: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_NotSupported"]
        fn io_status_not_supported2(msg: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_NotSupported"]
        fn io_status_not_supported3(subcode: SubCode) -> IOStatus;
        #[cxx_name = "IOStatus_NotSupported"]
        fn io_status_not_supported4() -> IOStatus;

        #[cxx_name = "IOStatus_NotFound"]
        fn io_status_not_found(msg: &Slice, msg2: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_NotFound"]
        fn io_status_not_found2(msg: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_NotFound"]
        fn io_status_not_found3(subcode: SubCode) -> IOStatus;
        #[cxx_name = "IOStatus_NotFound"]
        fn io_status_not_found4() -> IOStatus;

        #[cxx_name = "IOStatus_Corruption"]
        fn io_status_corruption(msg: &Slice, msg2: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_Corruption"]
        fn io_status_corruption2(msg: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_Corruption"]
        fn io_status_corruption3(subcode: SubCode) -> IOStatus;
        #[cxx_name = "IOStatus_Corruption"]
        fn io_status_corruption4() -> IOStatus;

        #[cxx_name = "IOStatus_InvalidArgument"]
        fn io_status_invalid_argument(msg: &Slice, msg2: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_InvalidArgument"]
        fn io_status_invalid_argument2(msg: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_InvalidArgument"]
        fn io_status_invalid_argument3(subcode: SubCode) -> IOStatus;
        #[cxx_name = "IOStatus_InvalidArgument"]
        fn io_status_invalid_argument4() -> IOStatus;

        #[cxx_name = "IOStatus_IOError"]
        fn io_status_io_error(msg: &Slice, msg2: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_IOError"]
        fn io_status_io_error2(msg: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_IOError"]
        fn io_status_io_error3(subcode: SubCode) -> IOStatus;
        #[cxx_name = "IOStatus_IOError"]
        fn io_status_io_error4() -> IOStatus;

        #[cxx_name = "IOStatus_Busy"]
        fn io_status_busy(msg: &Slice, msg2: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_Busy"]
        fn io_status_busy2(msg: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_Busy"]
        fn io_status_busy3(subcode: SubCode) -> IOStatus;
        #[cxx_name = "IOStatus_Busy"]
        fn io_status_busy4() -> IOStatus;

        #[cxx_name = "IOStatus_TimedOut"]
        fn io_status_timed_out(msg: &Slice, msg2: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_TimedOut"]
        fn io_status_timed_out2(msg: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_TimedOut"]
        fn io_status_timed_out3(subcode: SubCode) -> IOStatus;
        #[cxx_name = "IOStatus_TimedOut"]
        fn io_status_timed_out4() -> IOStatus;

        #[cxx_name = "IOStatus_Aborted"]
        fn io_status_aborted(msg: &Slice, msg2: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_Aborted"]
        fn io_status_aborted2(msg: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_Aborted"]
        fn io_status_aborted3(subcode: SubCode) -> IOStatus;
        #[cxx_name = "IOStatus_Aborted"]
        fn io_status_aborted4() -> IOStatus;

        #[cxx_name = "IOStatus_NoSpace"]
        fn io_status_no_space(msg: &Slice, msg2: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_NoSpace"]
        fn io_status_no_space2(msg: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_NoSpace"]
        fn io_status_no_space3() -> IOStatus;

        #[cxx_name = "IOStatus_PathNotFound"]
        fn io_status_path_not_found(msg: &Slice, msg2: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_PathNotFound"]
        fn io_status_path_not_found2(msg: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_PathNotFound"]
        fn io_status_path_not_found3() -> IOStatus;

        #[cxx_name = "IOStatus_IOFenced"]
        fn io_status_io_fenced(msg: &Slice, msg2: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_IOFenced"]
        fn io_status_io_fenced2(msg: &Slice) -> IOStatus;
        #[cxx_name = "IOStatus_IOFenced"]
        fn io_status_io_fenced3() -> IOStatus;

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
        fn status(self: &IOStatus) -> Status;
        fn subcode(self: &IOStatus) -> SubCode;
        #[cxx_name = "ToString"]
        fn to_string(self: &IOStatus) -> UniquePtr<CxxString>;
        #[cxx_name = "getState"]
        fn get_state(self: &IOStatus) -> &UniquePtr<CxxString>;
        fn eq(self: &IOStatus, other: &IOStatus) -> bool;
        #[cxx_name = "Clone"]
        fn clone(self: &IOStatus) -> IOStatus;
    }

    #[namespace = "rocksdb_rs::status"]
    unsafe extern "C++" {
        include!("rocksdb-rs/src/status.rs.h");

        type Status = crate::status::ffix::Status;
        type Code = crate::status::ffix::Code;
        type SubCode = crate::status::ffix::SubCode;
    }

    #[namespace = "rocksdb"]
    unsafe extern "C++" {
        include!("rocksdb/slice.h");

        type Slice = crate::ffi::slice::Slice;
    }
}

impl ffix::IOStatus {
    fn new(code: Code, subcode: SubCode, msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
        ffix::IOStatus {
            status: status::status_new7(code, subcode, msg, msg2),
        }
    }

    fn new2(code: Code, msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
        ffix::IOStatus {
            status: status::status_new6(code, msg, msg2),
        }
    }

    fn new3(code: Code, subcode: SubCode) -> ffix::IOStatus {
        ffix::IOStatus {
            status: status::status_new3(code, subcode),
        }
    }

    fn new4(code: Code) -> ffix::IOStatus {
        let status = ffix::Status {
            code,
            scope: ffix::IOErrorScope::kIOErrorScopeFileSystem.repr,
            ..ffix::Status::default()
        };
        ffix::IOStatus { status }
    }

    fn new5() -> ffix::IOStatus {
        ffix::IOStatus {
            status: ffix::Status::default(),
        }
    }

    fn new6(code: Code, msg: &ffix::Slice) -> ffix::IOStatus {
        let status = ffix::Status {
            code,
            state: msg.ToUniquePtrString(),
            ..ffix::Status::default()
        };
        ffix::IOStatus { status }
    }

    fn set_retryable(&mut self, retryable: bool) {
        self.status.retryable = retryable;
    }

    fn set_data_loss(&mut self, data_loss: bool) {
        self.status.data_loss = data_loss;
    }

    fn set_scope(&mut self, scope: ffix::IOErrorScope) {
        self.status.scope = scope.repr;
    }

    fn get_retryable(&self) -> bool {
        self.status.retryable
    }

    fn get_data_loss(&self) -> bool {
        self.status.data_loss
    }

    fn get_scope(&self) -> ffix::IOErrorScope {
        match self.status.scope {
            0 => ffix::IOErrorScope::kIOErrorScopeFileSystem,
            1 => ffix::IOErrorScope::kIOErrorScopeFile,
            2 => ffix::IOErrorScope::kIOErrorScopeRange,
            3 => ffix::IOErrorScope::kIOErrorScopeMax,
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

    fn status(&self) -> ffix::Status {
        self.status.clone()
    }

    fn subcode(&self) -> SubCode {
        self.status.subcode
    }

    fn to_string(&self) -> UniquePtr<CxxString> {
        self.status.to_string()
    }

    fn get_state(&self) -> &UniquePtr<CxxString> {
        self.status.get_state()
    }
}

fn io_status_new(status: ffix::Status) -> ffix::IOStatus {
    ffix::IOStatus { status }
}

fn io_status_new1(code: Code, subcode: SubCode) -> ffix::IOStatus {
    ffix::IOStatus::new3(code, subcode)
}

fn io_status_new2(code: Code) -> ffix::IOStatus {
    ffix::IOStatus::new4(code)
}

fn io_status_new3() -> ffix::IOStatus {
    ffix::IOStatus::new5()
}

fn io_status_new4(
    code: Code,
    subcode: SubCode,
    msg: &ffix::Slice,
    msg2: &ffix::Slice,
) -> ffix::IOStatus {
    ffix::IOStatus::new(code, subcode, msg, msg2)
}

fn io_status_new5(code: Code, msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new2(code, msg, msg2)
}

fn io_status_ok() -> ffix::IOStatus {
    ffix::IOStatus::new5()
}

fn io_status_not_supported(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new2(Code::kNotSupported, msg, msg2)
}

fn io_status_not_supported2(msg: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new6(Code::kNotSupported, msg)
}

fn io_status_not_supported3(subcode: SubCode) -> ffix::IOStatus {
    ffix::IOStatus::new3(Code::kNotSupported, subcode)
}

fn io_status_not_supported4() -> ffix::IOStatus {
    ffix::IOStatus::new4(Code::kNotSupported)
}

fn io_status_not_found(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new2(Code::kNotFound, msg, msg2)
}

fn io_status_not_found2(msg: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new6(Code::kNotFound, msg)
}

fn io_status_not_found3(subcode: SubCode) -> ffix::IOStatus {
    ffix::IOStatus::new3(Code::kNotFound, subcode)
}

fn io_status_not_found4() -> ffix::IOStatus {
    ffix::IOStatus::new4(Code::kNotFound)
}

fn io_status_corruption(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new2(Code::kCorruption, msg, msg2)
}

fn io_status_corruption2(msg: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new6(Code::kCorruption, msg)
}

fn io_status_corruption3(subcode: SubCode) -> ffix::IOStatus {
    ffix::IOStatus::new3(Code::kCorruption, subcode)
}

fn io_status_corruption4() -> ffix::IOStatus {
    ffix::IOStatus::new4(Code::kCorruption)
}

fn io_status_invalid_argument(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new2(Code::kInvalidArgument, msg, msg2)
}

fn io_status_invalid_argument2(msg: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new6(Code::kInvalidArgument, msg)
}

fn io_status_invalid_argument3(subcode: SubCode) -> ffix::IOStatus {
    ffix::IOStatus::new3(Code::kInvalidArgument, subcode)
}

fn io_status_invalid_argument4() -> ffix::IOStatus {
    ffix::IOStatus::new4(Code::kInvalidArgument)
}

fn io_status_io_error(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new2(Code::kIOError, msg, msg2)
}

fn io_status_io_error2(msg: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new6(Code::kIOError, msg)
}

fn io_status_io_error3(subcode: SubCode) -> ffix::IOStatus {
    ffix::IOStatus::new3(Code::kIOError, subcode)
}

fn io_status_io_error4() -> ffix::IOStatus {
    ffix::IOStatus::new4(Code::kIOError)
}

fn io_status_busy(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new2(Code::kBusy, msg, msg2)
}

fn io_status_busy2(msg: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new6(Code::kBusy, msg)
}

fn io_status_busy3(subcode: SubCode) -> ffix::IOStatus {
    ffix::IOStatus::new3(Code::kBusy, subcode)
}

fn io_status_busy4() -> ffix::IOStatus {
    ffix::IOStatus::new4(Code::kBusy)
}

fn io_status_timed_out(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new2(Code::kTimedOut, msg, msg2)
}

fn io_status_timed_out2(msg: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new6(Code::kTimedOut, msg)
}

fn io_status_timed_out3(subcode: SubCode) -> ffix::IOStatus {
    ffix::IOStatus::new3(Code::kTimedOut, subcode)
}

fn io_status_timed_out4() -> ffix::IOStatus {
    ffix::IOStatus::new4(Code::kTimedOut)
}

fn io_status_aborted(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new2(Code::kAborted, msg, msg2)
}

fn io_status_aborted2(msg: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new6(Code::kAborted, msg)
}

fn io_status_aborted3(subcode: SubCode) -> ffix::IOStatus {
    ffix::IOStatus::new3(Code::kAborted, subcode)
}

fn io_status_aborted4() -> ffix::IOStatus {
    ffix::IOStatus::new4(Code::kAborted)
}

fn io_status_no_space(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new(Code::kIOError, SubCode::kNoSpace, msg, msg2)
}

fn io_status_no_space2(msg: &ffix::Slice) -> ffix::IOStatus {
    let status = ffix::Status {
        code: Code::kIOError,
        subcode: SubCode::kNoSpace,
        state: msg.ToUniquePtrString(),
        ..ffix::Status::default()
    };
    ffix::IOStatus { status }
}

fn io_status_no_space3() -> ffix::IOStatus {
    ffix::IOStatus::new3(Code::kIOError, SubCode::kNoSpace)
}

fn io_status_path_not_found(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new(Code::kIOError, SubCode::kPathNotFound, msg, msg2)
}

fn io_status_path_not_found2(msg: &ffix::Slice) -> ffix::IOStatus {
    let status = ffix::Status {
        code: Code::kIOError,
        subcode: SubCode::kPathNotFound,
        state: msg.ToUniquePtrString(),
        ..ffix::Status::default()
    };
    ffix::IOStatus { status }
}

fn io_status_path_not_found3() -> ffix::IOStatus {
    ffix::IOStatus::new3(Code::kIOError, SubCode::kPathNotFound)
}

fn io_status_io_fenced(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::IOStatus {
    ffix::IOStatus::new(Code::kIOError, SubCode::kIOFenced, msg, msg2)
}

fn io_status_io_fenced2(msg: &ffix::Slice) -> ffix::IOStatus {
    let status = ffix::Status {
        code: Code::kIOError,
        subcode: SubCode::kIOFenced,
        state: msg.ToUniquePtrString(),
        ..ffix::Status::default()
    };
    ffix::IOStatus { status }
}

fn io_status_io_fenced3() -> ffix::IOStatus {
    ffix::IOStatus::new3(Code::kIOError, SubCode::kIOFenced)
}
