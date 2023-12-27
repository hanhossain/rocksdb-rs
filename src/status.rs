use cxx::{CxxString, UniquePtr};

#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
    #[derive(Debug)]
    enum Code {
        kOk = 0,
        kNotFound = 1,
        kCorruption = 2,
        kNotSupported = 3,
        kInvalidArgument = 4,
        kIOError = 5,
        kMergeInProgress = 6,
        kIncomplete = 7,
        kShutdownInProgress = 8,
        kTimedOut = 9,
        kAborted = 10,
        kBusy = 11,
        kExpired = 12,
        kTryAgain = 13,
        kCompactionTooLarge = 14,
        kColumnFamilyDropped = 15,
        kMaxCode,
    }

    #[derive(Debug)]
    enum SubCode {
        kNone = 0,
        kMutexTimeout = 1,
        kLockTimeout = 2,
        kLockLimit = 3,
        kNoSpace = 4,
        kDeadlock = 5,
        kStaleFile = 6,
        kMemoryLimit = 7,
        kSpaceLimit = 8,
        kPathNotFound = 9,
        KMergeOperandsInsufficientCapacity = 10,
        kManualCompactionPaused = 11,
        kOverwritten = 12,
        kTxnNotPrepared = 13,
        kIOFenced = 14,
        kMergeOperatorFailed = 15,
        kMaxSubCode,
    }

    #[derive(Debug)]
    enum Severity {
        kNoError = 0,
        kSoftError = 1,
        kHardError = 2,
        kFatalError = 3,
        kUnrecoverableError = 4,
        kMaxSeverity,
    }

    #[derive(Debug)]
    struct Status {
        #[cxx_name = "code_"]
        code: Code,
        #[cxx_name = "subcode_"]
        subcode: SubCode,
        #[cxx_name = "severity_"]
        severity: Severity,
        retryable: bool,
        data_loss: bool,
        scope: u8,
        state: UniquePtr<CxxString>,
    }

    extern "Rust" {
        #[cxx_name = "Status_new"]
        fn status_new(
            code: Code,
            subcode: SubCode,
            severity: Severity,
            retryable: bool,
            data_loss: bool,
            scope: u8,
            state: UniquePtr<CxxString>,
        ) -> Status;
        #[cxx_name = "Status_new"]
        fn status_new1() -> Status;
        #[cxx_name = "Status_new"]
        fn status_new2(code: Code) -> Status;
        #[cxx_name = "Status_new"]
        fn status_new3(code: Code, subcode: SubCode) -> Status;
        #[cxx_name = "Status_new"]
        fn status_new4(
            code: Code,
            subcode: SubCode,
            retryable: bool,
            data_loss: bool,
            scope: u8,
        ) -> Status;
        #[cxx_name = "Status_new"]
        fn status_new5(
            code: Code,
            subcode: SubCode,
            msg: &Slice,
            msg2: &Slice,
            sev: Severity,
        ) -> Status;
        #[cxx_name = "Status_new"]
        fn status_new6(code: Code, msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_new"]
        fn status_new7(code: Code, subcode: SubCode, msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_new"]
        fn status_new8(code: Code, subcode: SubCode, sev: Severity, msg: &Slice) -> Status;
        #[cxx_name = "Status_new"]
        fn status_new9(status: &Status, severity: Severity) -> Status;

        #[cxx_name = "Status_OK"]
        fn status_ok() -> Status;

        #[cxx_name = "Status_OkOverwritten"]
        fn status_ok_overwritten() -> Status;

        #[cxx_name = "Status_NotFound"]
        fn status_not_found() -> Status;
        #[cxx_name = "Status_NotFound"]
        fn status_not_found1(msg: SubCode) -> Status;
        #[cxx_name = "Status_NotFound"]
        fn status_not_found2(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_NotFound"]
        fn status_not_found3(msg: &Slice) -> Status;
        #[cxx_name = "Status_NotFound"]
        fn status_not_found4(subcode: SubCode, msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_NotFound"]
        fn status_not_found5(subcode: SubCode, msg: &Slice) -> Status;

        #[cxx_name = "Status_Corruption"]
        fn status_corruption(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_Corruption"]
        fn status_corruption2(subcode: SubCode) -> Status;
        #[cxx_name = "Status_Corruption"]
        fn status_corruption3() -> Status;
        #[cxx_name = "Status_Corruption"]
        fn status_corruption4(msg: &Slice) -> Status;

        #[cxx_name = "Status_NotSupported"]
        fn status_not_supported(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_NotSupported"]
        fn status_not_supported2(msg: &Slice) -> Status;
        #[cxx_name = "Status_NotSupported"]
        fn status_not_supported3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_NotSupported"]
        fn status_not_supported4() -> Status;

        #[cxx_name = "Status_InvalidArgument"]
        fn status_invalid_argument(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_InvalidArgument"]
        fn status_invalid_argument2(msg: &Slice) -> Status;
        #[cxx_name = "Status_InvalidArgument"]
        fn status_invalid_argument3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_InvalidArgument"]
        fn status_invalid_argument4() -> Status;

        #[cxx_name = "Status_IOError"]
        fn status_io_error(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_IOError"]
        fn status_io_error2(msg: &Slice) -> Status;
        #[cxx_name = "Status_IOError"]
        fn status_io_error3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_IOError"]
        fn status_io_error4() -> Status;

        #[cxx_name = "Status_MergeInProgress"]
        fn status_merge_in_progress(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_MergeInProgress"]
        fn status_merge_in_progress2(msg: &Slice) -> Status;
        #[cxx_name = "Status_MergeInProgress"]
        fn status_merge_in_progress3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_MergeInProgress"]
        fn status_merge_in_progress4() -> Status;

        #[cxx_name = "Status_Incomplete"]
        fn status_incomplete(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_Incomplete"]
        fn status_incomplete2(msg: &Slice) -> Status;
        #[cxx_name = "Status_Incomplete"]
        fn status_incomplete3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_Incomplete"]
        fn status_incomplete4() -> Status;

        #[cxx_name = "Status_ShutdownInProgress"]
        fn status_shutdown_in_progress(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_ShutdownInProgress"]
        fn status_shutdown_in_progress2(msg: &Slice) -> Status;
        #[cxx_name = "Status_ShutdownInProgress"]
        fn status_shutdown_in_progress3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_ShutdownInProgress"]
        fn status_shutdown_in_progress4() -> Status;

        #[cxx_name = "Status_Aborted"]
        fn status_aborted(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_Aborted"]
        fn status_aborted2(msg: &Slice) -> Status;
        #[cxx_name = "Status_Aborted"]
        fn status_aborted3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_Aborted"]
        fn status_aborted4() -> Status;

        #[cxx_name = "Status_Busy"]
        fn status_busy(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_Busy"]
        fn status_busy2(msg: &Slice) -> Status;
        #[cxx_name = "Status_Busy"]
        fn status_busy3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_Busy"]
        fn status_busy4() -> Status;

        #[cxx_name = "Status_TimedOut"]
        fn status_timed_out(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_TimedOut"]
        fn status_timed_out2(msg: &Slice) -> Status;
        #[cxx_name = "Status_TimedOut"]
        fn status_timed_out3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_TimedOut"]
        fn status_timed_out4() -> Status;

        #[cxx_name = "Status_Expired"]
        fn status_expired(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_Expired"]
        fn status_expired2(msg: &Slice) -> Status;
        #[cxx_name = "Status_Expired"]
        fn status_expired3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_Expired"]
        fn status_expired4() -> Status;

        #[cxx_name = "Status_TryAgain"]
        fn status_try_again(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_TryAgain"]
        fn status_try_again2(msg: &Slice) -> Status;
        #[cxx_name = "Status_TryAgain"]
        fn status_try_again3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_TryAgain"]
        fn status_try_again4() -> Status;

        #[cxx_name = "Status_CompactionTooLarge"]
        fn status_compaction_too_large(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_CompactionTooLarge"]
        fn status_compaction_too_large2(msg: &Slice) -> Status;
        #[cxx_name = "Status_CompactionTooLarge"]
        fn status_compaction_too_large3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_CompactionTooLarge"]
        fn status_compaction_too_large4() -> Status;

        #[cxx_name = "Status_ColumnFamilyDropped"]
        fn status_column_family_dropped(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_ColumnFamilyDropped"]
        fn status_column_family_dropped2(msg: &Slice) -> Status;
        #[cxx_name = "Status_ColumnFamilyDropped"]
        fn status_column_family_dropped3(subcode: SubCode) -> Status;
        #[cxx_name = "Status_ColumnFamilyDropped"]
        fn status_column_family_dropped4() -> Status;

        #[cxx_name = "Status_NoSpace"]
        fn status_no_space(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_NoSpace"]
        fn status_no_space2(msg: &Slice) -> Status;
        #[cxx_name = "Status_NoSpace"]
        fn status_no_space3() -> Status;

        #[cxx_name = "Status_MemoryLimit"]
        fn status_memory_limit(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_MemoryLimit"]
        fn status_memory_limit2(msg: &Slice) -> Status;
        #[cxx_name = "Status_MemoryLimit"]
        fn status_memory_limit3() -> Status;

        #[cxx_name = "Status_SpaceLimit"]
        fn status_space_limit(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_SpaceLimit"]
        fn status_space_limit2(msg: &Slice) -> Status;
        #[cxx_name = "Status_SpaceLimit"]
        fn status_space_limit3() -> Status;

        #[cxx_name = "Status_PathNotFound"]
        fn status_path_not_found(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_PathNotFound"]
        fn status_path_not_found2(msg: &Slice) -> Status;
        #[cxx_name = "Status_PathNotFound"]
        fn status_path_not_found3() -> Status;

        #[cxx_name = "Status_TxnNotPrepared"]
        fn status_txn_not_prepared(msg: &Slice, msg2: &Slice) -> Status;
        #[cxx_name = "Status_TxnNotPrepared"]
        fn status_txn_not_prepared2(msg: &Slice) -> Status;
        #[cxx_name = "Status_TxnNotPrepared"]
        fn status_txn_not_prepared3() -> Status;

        #[cxx_name = "Status_CopyState"]
        fn status_copy_state(s: &CxxString) -> UniquePtr<CxxString>;

        #[cxx_name = "Status_CopyAppendMessage"]
        fn status_copy_append_message(status: &Status, delim: &Slice, msg: &Slice) -> Status;

        #[cxx_name = "Status_Move"]
        fn status_move(status: Status) -> Status;

        fn code(self: &Status) -> Code;
        fn subcode(self: &Status) -> SubCode;
        fn severity(self: &Status) -> Severity;
        #[cxx_name = "getState"]
        fn get_state(self: &Status) -> &UniquePtr<CxxString>;
        fn ok(self: &Status) -> bool;
        #[cxx_name = "IsOkOverwritten"]
        fn is_ok_overwritten(self: &Status) -> bool;
        #[cxx_name = "IsNotFound"]
        fn is_not_found(self: &Status) -> bool;
        #[cxx_name = "IsCorruption"]
        fn is_corruption(self: &Status) -> bool;
        #[cxx_name = "IsNotSupported"]
        fn is_not_supported(self: &Status) -> bool;
        #[cxx_name = "IsInvalidArgument"]
        fn is_invalid_argument(self: &Status) -> bool;
        #[cxx_name = "IsIOError"]
        fn is_io_error(self: &Status) -> bool;
        #[cxx_name = "IsMergeInProgress"]
        fn is_merge_in_progress(self: &Status) -> bool;
        #[cxx_name = "IsIncomplete"]
        fn is_incomplete(self: &Status) -> bool;
        #[cxx_name = "IsShutdownInProgress"]
        fn is_shutdown_in_progress(self: &Status) -> bool;
        #[cxx_name = "IsTimedOut"]
        fn is_timed_out(self: &Status) -> bool;
        #[cxx_name = "IsAborted"]
        fn is_aborted(self: &Status) -> bool;
        #[cxx_name = "IsLockLimit"]
        fn is_lock_limit(self: &Status) -> bool;
        #[cxx_name = "IsBusy"]
        fn is_busy(self: &Status) -> bool;
        #[cxx_name = "IsDeadlock"]
        fn is_deadlock(self: &Status) -> bool;
        #[cxx_name = "IsExpired"]
        fn is_expired(self: &Status) -> bool;
        #[cxx_name = "IsTryAgain"]
        fn is_try_again(self: &Status) -> bool;
        #[cxx_name = "IsCompactionTooLarge"]
        fn is_compaction_too_large(self: &Status) -> bool;
        #[cxx_name = "IsColumnFamilyDropped"]
        fn is_column_family_dropped(self: &Status) -> bool;
        #[cxx_name = "IsNoSpace"]
        fn is_no_space(self: &Status) -> bool;
        #[cxx_name = "IsMemoryLimit"]
        fn is_memory_limit(self: &Status) -> bool;
        #[cxx_name = "IsPathNotFound"]
        fn is_path_not_found(self: &Status) -> bool;
        #[cxx_name = "IsManualCompactionPaused"]
        fn is_manual_compaction_paused(self: &Status) -> bool;
        #[cxx_name = "IsTxnNotPrepared"]
        fn is_txn_not_prepared(self: &Status) -> bool;
        #[cxx_name = "IsIOFenced"]
        fn is_io_fenced(self: &Status) -> bool;
        #[cxx_name = "ToString"]
        fn to_string(self: &Status) -> UniquePtr<CxxString>;
        #[cxx_name = "Clone"]
        fn clone(self: &Status) -> Status;
        fn eq(self: &Status, other: &Status) -> bool;
        fn copy_from(self: &mut Status, other: &Status);
        fn move_from(self: &mut Status, other: Status);
        fn create_vec(self: &Status, n: usize) -> Vec<Status>;
        fn resize_vec(self: &Status, v: &mut Vec<Status>, n: usize);
    }

    unsafe extern "C++" {
        include!("rocksdb/slice.h");

        type Slice = crate::slice::ffi::Slice;
    }
}

impl ffi::Status {
    fn new_with_slices(
        code: ffi::Code,
        subcode: ffi::SubCode,
        msg: &ffi::Slice,
        msg2: &ffi::Slice,
        sev: ffi::Severity,
    ) -> ffi::Status {
        assert_ne!(subcode, ffi::SubCode::kMaxSubCode);
        let msg = msg.to_unique_ptr_string();
        let msg2 = msg2.to_unique_ptr_string();

        ffi::Status::new_with_messages(code, subcode, msg, msg2, sev)
    }

    fn new_with_messages(
        code: ffi::Code,
        subcode: ffi::SubCode,
        mut msg: UniquePtr<CxxString>,
        msg2: UniquePtr<CxxString>,
        severity: ffi::Severity,
    ) -> ffi::Status {
        assert_ne!(subcode, ffi::SubCode::kMaxSubCode);

        if !msg2.is_null() && msg2.len() > 0 {
            msg.pin_mut().push_str(": ");

            // Can't treat this as a string yet because it's not guaranteed to be valid UTF-8.
            msg.pin_mut().push_bytes(msg2.as_bytes());
        }

        ffi::Status {
            code,
            subcode,
            severity,
            state: msg,
            ..ffi::Status::default()
        }
    }

    fn code(&self) -> ffi::Code {
        self.code
    }

    fn subcode(&self) -> ffi::SubCode {
        self.subcode
    }

    fn severity(&self) -> ffi::Severity {
        self.severity
    }

    fn get_state(&self) -> &UniquePtr<CxxString> {
        &self.state
    }

    /// Returns true iff the status indicates success.
    fn ok(&self) -> bool {
        self.code == ffi::Code::kOk
    }

    /// Returns true iff the status indicates success *with* something overwritten
    fn is_ok_overwritten(&self) -> bool {
        self.code == ffi::Code::kOk && self.subcode == ffi::SubCode::kOverwritten
    }

    /// Returns true iff the status indicates a NotFound error.
    fn is_not_found(&self) -> bool {
        self.code == ffi::Code::kNotFound
    }

    /// Returns true iff the status indicates a Corruption error.
    fn is_corruption(&self) -> bool {
        self.code == ffi::Code::kCorruption
    }

    /// Returns true iff the status indicates a NotSupported error.
    fn is_not_supported(&self) -> bool {
        self.code == ffi::Code::kNotSupported
    }

    /// Returns true iff the status indicates an InvalidArgument error.
    fn is_invalid_argument(&self) -> bool {
        self.code == ffi::Code::kInvalidArgument
    }

    /// Returns true iff the status indicates an IOError.
    fn is_io_error(&self) -> bool {
        self.code == ffi::Code::kIOError
    }

    /// Returns true iff the status indicates an MergeInProgress.
    fn is_merge_in_progress(&self) -> bool {
        self.code == ffi::Code::kMergeInProgress
    }

    /// Returns true iff the status indicates Incomplete
    fn is_incomplete(&self) -> bool {
        self.code == ffi::Code::kIncomplete
    }

    /// Returns true iff the status indicates Shutdown In progress
    fn is_shutdown_in_progress(&self) -> bool {
        self.code == ffi::Code::kShutdownInProgress
    }

    fn is_timed_out(&self) -> bool {
        self.code == ffi::Code::kTimedOut
    }

    fn is_aborted(&self) -> bool {
        self.code == ffi::Code::kAborted
    }

    fn is_lock_limit(&self) -> bool {
        self.code == ffi::Code::kAborted && self.subcode == ffi::SubCode::kLockLimit
    }

    /// Returns true iff the status indicates that a resource is Busy and temporarily could not be
    /// acquired.
    fn is_busy(&self) -> bool {
        self.code == ffi::Code::kBusy
    }

    fn is_deadlock(&self) -> bool {
        self.code == ffi::Code::kBusy && self.subcode == ffi::SubCode::kDeadlock
    }

    /// Returns true iff the status indicated that the operation has Expired.
    fn is_expired(&self) -> bool {
        self.code == ffi::Code::kExpired
    }

    /// Returns true iff the status indicates a TryAgain error. This usually means that the
    /// operation failed, but may succeed if re-attempted.
    fn is_try_again(&self) -> bool {
        self.code == ffi::Code::kTryAgain
    }

    /// Returns true iff the status indicates the proposed compaction is too large
    fn is_compaction_too_large(&self) -> bool {
        self.code == ffi::Code::kCompactionTooLarge
    }

    /// Returns true iff the status indicates Column Family Dropped
    fn is_column_family_dropped(&self) -> bool {
        self.code == ffi::Code::kColumnFamilyDropped
    }

    /// Returns true iff the status indicates a NoSpace error. This is caused by an I/O error
    /// returning the specific "out of space" error condition. Stricto sensu, an NoSpace error is an
    /// I/O error with a specific subcode, enabling users to take the appropriate action if needed
    fn is_no_space(&self) -> bool {
        self.code == ffi::Code::kIOError && self.subcode == ffi::SubCode::kNoSpace
    }

    /// Returns true iff the status indicates a memory limit error. There may be cases where we
    /// limit the memory used in certain operations (eg. the size of a write batch) in order to
    /// avoid out of memory exceptions.
    fn is_memory_limit(&self) -> bool {
        self.code == ffi::Code::kAborted && self.subcode == ffi::SubCode::kMemoryLimit
    }

    /// Returns true iff the status indicates a PathNotFound error. This is caused by an I/O error
    /// returning the specific "no such file or directory" error condition. A PathNotFound error is
    /// an I/O error with a specific subcode, enabling users to take appropriate action if necessary
    fn is_path_not_found(&self) -> bool {
        (self.code == ffi::Code::kIOError || self.code == ffi::Code::kNotFound)
            && self.subcode == ffi::SubCode::kPathNotFound
    }

    /// Returns true iff the status indicates manual compaction paused. This is caused by a call to
    /// PauseManualCompaction
    fn is_manual_compaction_paused(&self) -> bool {
        self.code == ffi::Code::kIncomplete && self.subcode == ffi::SubCode::kManualCompactionPaused
    }

    /// Returns true iff the status indicates a TxnNotPrepared error.
    fn is_txn_not_prepared(&self) -> bool {
        self.code == ffi::Code::kInvalidArgument && self.subcode == ffi::SubCode::kTxnNotPrepared
    }

    /// Returns true iff the status indicates a IOFenced error.
    fn is_io_fenced(&self) -> bool {
        self.code == ffi::Code::kIOError && self.subcode == ffi::SubCode::kIOFenced
    }

    fn to_string(&self) -> UniquePtr<CxxString> {
        let msg = match self.code {
            ffi::Code::kOk => {
                let mut s = crate::ffi::make_string();
                s.pin_mut().push_str("OK");
                return s;
            }
            ffi::Code::kNotFound => "NotFound: ",
            ffi::Code::kCorruption => "Corruption: ",
            ffi::Code::kNotSupported => "Not implemented: ",
            ffi::Code::kInvalidArgument => "Invalid argument: ",
            ffi::Code::kIOError => "IO error: ",
            ffi::Code::kMergeInProgress => "Merge in progress: ",
            ffi::Code::kIncomplete => "Result incomplete: ",
            ffi::Code::kShutdownInProgress => "Shutdown in progress: ",
            ffi::Code::kTimedOut => "Operation timed out: ",
            ffi::Code::kAborted => "Operation aborted: ",
            ffi::Code::kBusy => "Resource busy: ",
            ffi::Code::kExpired => "Operation expired: ",
            ffi::Code::kTryAgain => "Operation failed. Try again.: ",
            ffi::Code::kCompactionTooLarge => "Compaction too large: ",
            ffi::Code::kColumnFamilyDropped => "Column family dropped: ",
            x => unreachable!("{:?} is not a valid status code", x),
        };

        let mut res = crate::ffi::make_string();
        res.pin_mut().push_str(msg);

        if self.subcode != ffi::SubCode::kNone {
            let subcode_msg = match self.subcode {
                ffi::SubCode::kMutexTimeout => "Timeout Acquiring Mutex",
                ffi::SubCode::kLockTimeout => "Timeout waiting to lock key",
                ffi::SubCode::kLockLimit => "Failed to acquire lock due to max_num_locks limit",
                ffi::SubCode::kNoSpace => "No space left on device",
                ffi::SubCode::kDeadlock => "Deadlock",
                ffi::SubCode::kStaleFile => "Stale file handle",
                ffi::SubCode::kMemoryLimit => "Memory limit reached",
                ffi::SubCode::kSpaceLimit => "Space limit reached",
                ffi::SubCode::kPathNotFound => "No such file or directory",
                ffi::SubCode::KMergeOperandsInsufficientCapacity => {
                    "Insufficient capacity for merge operands"
                }
                ffi::SubCode::kManualCompactionPaused => "Manual compaction paused",
                ffi::SubCode::kOverwritten => " (overwritten)",
                ffi::SubCode::kTxnNotPrepared => "Txn not prepared",
                ffi::SubCode::kIOFenced => "IO fenced off",
                ffi::SubCode::kMergeOperatorFailed => "Merge operator failed",
                x => unreachable!("{:?} is not a valid status subcode", x),
            };

            res.pin_mut().push_str(subcode_msg);
        }

        if !self.state.is_null() {
            if self.subcode != ffi::SubCode::kNone {
                res.pin_mut().push_str(": ");
            }
            res.pin_mut().push_bytes(self.state.as_bytes());
        }

        res
    }

    fn copy_from(&mut self, other: &ffi::Status) {
        self.code = other.code;
        self.subcode = other.subcode;
        self.severity = other.severity;
        self.retryable = other.retryable;
        self.data_loss = other.data_loss;
        self.scope = other.scope;
        self.state = other.copy_state();
    }

    fn move_from(&mut self, other: ffi::Status) {
        self.code = other.code;
        self.subcode = other.subcode;
        self.severity = other.severity;
        self.retryable = other.retryable;
        self.data_loss = other.data_loss;
        self.scope = other.scope;
        self.state = other.state;
    }

    fn copy_state(&self) -> UniquePtr<CxxString> {
        if self.state.is_null() {
            UniquePtr::null()
        } else {
            status_copy_state(&self.state)
        }
    }

    fn create_vec(&self, n: usize) -> Vec<ffi::Status> {
        let mut v = Vec::new();
        for _ in 0..n {
            v.push(self.clone());
        }
        v
    }

    fn resize_vec(&self, v: &mut Vec<ffi::Status>, n: usize) {
        if n > v.len() {
            for _ in v.len()..n {
                v.push(self.clone());
            }
        } else {
            v.truncate(n);
        }
    }
}

impl Default for ffi::Status {
    fn default() -> Self {
        Self {
            code: ffi::Code::kOk,
            subcode: ffi::SubCode::kNone,
            severity: ffi::Severity::kNoError,
            retryable: false,
            data_loss: false,
            scope: 0,
            state: UniquePtr::null(),
        }
    }
}

impl Clone for ffi::Status {
    fn clone(&self) -> Self {
        let state = self.copy_state();
        Self {
            code: self.code,
            subcode: self.subcode,
            severity: self.severity,
            retryable: self.retryable,
            data_loss: self.data_loss,
            scope: self.scope,
            state,
        }
    }
}

impl PartialEq<Self> for ffi::Status {
    fn eq(&self, other: &Self) -> bool {
        // Original implementation only checks whether code == code.
        self.code == other.code
    }
}

impl Eq for ffi::Status {}

fn status_new(
    code: ffi::Code,
    subcode: ffi::SubCode,
    severity: ffi::Severity,
    retryable: bool,
    data_loss: bool,
    scope: u8,
    state: UniquePtr<CxxString>,
) -> ffi::Status {
    ffi::Status {
        code,
        subcode,
        severity,
        retryable,
        data_loss,
        scope,
        state,
    }
}

/// Create a success status.
fn status_new1() -> ffi::Status {
    ffi::Status::default()
}

fn status_new2(code: ffi::Code) -> ffi::Status {
    ffi::Status {
        code,
        ..ffi::Status::default()
    }
}

fn status_new3(code: ffi::Code, subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_new4(
    code: ffi::Code,
    subcode: ffi::SubCode,
    retryable: bool,
    data_loss: bool,
    scope: u8,
) -> ffi::Status {
    ffi::Status {
        code,
        subcode,
        retryable,
        data_loss,
        scope,
        ..ffi::Status::default()
    }
}

fn status_new5(
    code: ffi::Code,
    subcode: ffi::SubCode,
    msg: &ffi::Slice,
    msg2: &ffi::Slice,
    sev: ffi::Severity,
) -> ffi::Status {
    ffi::Status::new_with_slices(code, subcode, msg, msg2, sev)
}

fn status_new6(code: ffi::Code, msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        code,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_new7(
    code: ffi::Code,
    subcode: ffi::SubCode,
    msg: &ffi::Slice,
    msg2: &ffi::Slice,
) -> ffi::Status {
    ffi::Status::new_with_slices(code, subcode, msg, msg2, ffi::Severity::kNoError)
}

fn status_new8(
    code: ffi::Code,
    subcode: ffi::SubCode,
    sev: ffi::Severity,
    msg: &ffi::Slice,
) -> ffi::Status {
    ffi::Status::new_with_messages(
        code,
        subcode,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        sev,
    )
}

fn status_new9(status: &ffi::Status, severity: ffi::Severity) -> ffi::Status {
    let state = status.copy_state();
    ffi::Status {
        code: status.code,
        subcode: status.subcode,
        severity,
        retryable: status.retryable,
        data_loss: status.data_loss,
        scope: status.scope,
        state,
    }
}

fn status_ok() -> ffi::Status {
    ffi::Status::default()
}

fn status_ok_overwritten() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kOk,
        subcode: ffi::SubCode::kOverwritten,
        ..ffi::Status::default()
    }
}

fn status_not_found() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kNotFound,
        ..ffi::Status::default()
    }
}

fn status_not_found1(msg: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kNotFound,
        subcode: msg,
        ..ffi::Status::default()
    }
}

fn status_not_found2(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kNotFound,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_not_found3(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kNotFound,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_not_found4(subcode: ffi::SubCode, msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kNotFound,
        subcode,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_not_found5(subcode: ffi::SubCode, msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kNotFound,
        subcode,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_corruption(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kCorruption,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_corruption2(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kCorruption,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_corruption3() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kCorruption,
        ..ffi::Status::default()
    }
}

fn status_corruption4(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kCorruption,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_not_supported(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kNotSupported,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_not_supported2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kNotSupported,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_not_supported3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kNotSupported,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_not_supported4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kNotSupported,
        ..ffi::Status::default()
    }
}

fn status_invalid_argument(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kInvalidArgument,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_invalid_argument2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kInvalidArgument,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_invalid_argument3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kInvalidArgument,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_invalid_argument4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kInvalidArgument,
        ..ffi::Status::default()
    }
}

fn status_io_error(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kIOError,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}
fn status_io_error2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kIOError,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}
fn status_io_error3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kIOError,
        subcode,
        ..ffi::Status::default()
    }
}
fn status_io_error4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kIOError,
        ..ffi::Status::default()
    }
}

fn status_merge_in_progress(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kMergeInProgress,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_merge_in_progress2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kMergeInProgress,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_merge_in_progress3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kMergeInProgress,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_merge_in_progress4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kMergeInProgress,
        ..ffi::Status::default()
    }
}

fn status_incomplete(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kIncomplete,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_incomplete2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kIncomplete,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_incomplete3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kIncomplete,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_incomplete4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kIncomplete,
        ..ffi::Status::default()
    }
}

fn status_shutdown_in_progress(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kShutdownInProgress,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_shutdown_in_progress2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kShutdownInProgress,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_shutdown_in_progress3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kShutdownInProgress,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_shutdown_in_progress4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kShutdownInProgress,
        ..ffi::Status::default()
    }
}

fn status_aborted(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kAborted,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_aborted2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kAborted,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_aborted3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kAborted,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_aborted4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kAborted,
        ..ffi::Status::default()
    }
}

fn status_busy(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kBusy,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_busy2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kBusy,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_busy3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kBusy,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_busy4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kBusy,
        ..ffi::Status::default()
    }
}

fn status_timed_out(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kTimedOut,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_timed_out2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kTimedOut,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_timed_out3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kTimedOut,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_timed_out4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kTimedOut,
        ..ffi::Status::default()
    }
}

fn status_expired(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kTimedOut,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_expired2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kExpired,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_expired3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kExpired,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_expired4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kExpired,
        ..ffi::Status::default()
    }
}

fn status_try_again(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kTryAgain,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_try_again2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kTryAgain,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_try_again3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kTryAgain,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_try_again4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kTryAgain,
        ..ffi::Status::default()
    }
}

fn status_compaction_too_large(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kCompactionTooLarge,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_compaction_too_large2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kCompactionTooLarge,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_compaction_too_large3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kCompactionTooLarge,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_compaction_too_large4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kCompactionTooLarge,
        ..ffi::Status::default()
    }
}

fn status_column_family_dropped(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kColumnFamilyDropped,
        ffi::SubCode::kNone,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_column_family_dropped2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kColumnFamilyDropped,
        ffi::SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_column_family_dropped3(subcode: ffi::SubCode) -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kColumnFamilyDropped,
        subcode,
        ..ffi::Status::default()
    }
}

fn status_column_family_dropped4() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kColumnFamilyDropped,
        ..ffi::Status::default()
    }
}

fn status_no_space(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kIOError,
        ffi::SubCode::kNoSpace,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_no_space2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kIOError,
        ffi::SubCode::kNoSpace,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_no_space3() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kIOError,
        subcode: ffi::SubCode::kNoSpace,
        ..ffi::Status::default()
    }
}

fn status_memory_limit(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kAborted,
        ffi::SubCode::kMemoryLimit,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_memory_limit2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kAborted,
        ffi::SubCode::kMemoryLimit,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_memory_limit3() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kAborted,
        subcode: ffi::SubCode::kMemoryLimit,
        ..ffi::Status::default()
    }
}

fn status_space_limit(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kIOError,
        ffi::SubCode::kSpaceLimit,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_space_limit2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kIOError,
        ffi::SubCode::kSpaceLimit,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_space_limit3() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kIOError,
        subcode: ffi::SubCode::kSpaceLimit,
        ..ffi::Status::default()
    }
}

fn status_path_not_found(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kIOError,
        ffi::SubCode::kPathNotFound,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_path_not_found2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kIOError,
        ffi::SubCode::kPathNotFound,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_path_not_found3() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kIOError,
        subcode: ffi::SubCode::kPathNotFound,
        ..ffi::Status::default()
    }
}

fn status_txn_not_prepared(msg: &ffi::Slice, msg2: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_slices(
        ffi::Code::kInvalidArgument,
        ffi::SubCode::kTxnNotPrepared,
        msg,
        msg2,
        ffi::Severity::kNoError,
    )
}

fn status_txn_not_prepared2(msg: &ffi::Slice) -> ffi::Status {
    ffi::Status::new_with_messages(
        ffi::Code::kInvalidArgument,
        ffi::SubCode::kTxnNotPrepared,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        ffi::Severity::kNoError,
    )
}

fn status_txn_not_prepared3() -> ffi::Status {
    ffi::Status {
        code: ffi::Code::kInvalidArgument,
        subcode: ffi::SubCode::kTxnNotPrepared,
        ..ffi::Status::default()
    }
}

fn status_copy_state(s: &CxxString) -> UniquePtr<CxxString> {
    let mut res = crate::ffi::make_string();
    res.pin_mut().push_bytes(s.as_bytes());
    res
}

fn status_copy_append_message(
    status: &ffi::Status,
    delim: &ffi::Slice,
    msg: &ffi::Slice,
) -> ffi::Status {
    let mut new_msg = crate::ffi::make_string();
    new_msg.pin_mut().push_bytes(status.state.as_bytes());
    new_msg
        .pin_mut()
        .push_bytes(delim.to_unique_ptr_string().as_bytes());
    new_msg
        .pin_mut()
        .push_bytes(msg.to_unique_ptr_string().as_bytes());

    ffi::Status {
        code: status.code,
        subcode: status.subcode,
        severity: status.severity,
        retryable: status.retryable,
        data_loss: status.data_loss,
        scope: status.scope,
        state: new_msg,
    }
}

fn status_move(status: ffi::Status) -> ffi::Status {
    ffi::Status {
        code: status.code,
        subcode: status.subcode,
        severity: status.severity,
        retryable: status.retryable,
        data_loss: status.data_loss,
        scope: status.scope,
        state: status.state,
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn to_string_ok() {
        let status = ffi::Status::default();
        let s = status.to_string();
        assert_eq!(s.to_str().unwrap(), "OK");
    }

    #[test]
    fn to_string_memory_limit() {
        let status = ffi::Status {
            code: ffi::Code::kAborted,
            subcode: ffi::SubCode::kMemoryLimit,
            ..ffi::Status::default()
        };
        let s = status.to_string();
        assert_eq!(
            s.to_str().unwrap(),
            "Operation aborted: Memory limit reached"
        );
    }
}
