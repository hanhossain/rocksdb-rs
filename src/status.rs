use crate::slice::ffi::Slice;
use crate::status::ffi::{Code, Severity, Status, SubCode};
use cxx::{CxxString, UniquePtr};

#[cxx::bridge(namespace = "rocksdb")]
pub mod ffi {
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
    }

    unsafe extern "C++" {
        include!("rocksdb/slice.h");

        type Slice = crate::slice::ffi::Slice;
    }
}

impl Status {
    pub fn new_with_slices(
        code: Code,
        subcode: SubCode,
        msg: &Slice,
        msg2: &Slice,
        sev: Severity,
    ) -> Status {
        assert_ne!(subcode, SubCode::kMaxSubCode);
        let msg = msg.to_unique_ptr_string();
        let msg2 = msg2.to_unique_ptr_string();

        Status::new_with_messages(code, subcode, msg, msg2, sev)
    }

    fn new_with_messages(
        code: Code,
        subcode: SubCode,
        mut msg: UniquePtr<CxxString>,
        msg2: UniquePtr<CxxString>,
        severity: Severity,
    ) -> Status {
        assert_ne!(subcode, SubCode::kMaxSubCode);

        if !msg2.is_null() && msg2.len() > 0 {
            msg.pin_mut().push_str(": ");

            // Can't treat this as a string yet because it's not guaranteed to be valid UTF-8.
            msg.pin_mut().push_bytes(msg2.as_bytes());
        }

        Status {
            code,
            subcode,
            severity,
            state: msg,
            ..Status::default()
        }
    }

    pub fn code(&self) -> Code {
        self.code
    }

    pub fn subcode(&self) -> SubCode {
        self.subcode
    }

    pub fn severity(&self) -> Severity {
        self.severity
    }

    pub fn get_state(&self) -> &UniquePtr<CxxString> {
        &self.state
    }

    /// Returns true iff the status indicates success.
    pub fn ok(&self) -> bool {
        self.code == Code::kOk
    }

    /// Returns true iff the status indicates success *with* something overwritten
    pub fn is_ok_overwritten(&self) -> bool {
        self.code == Code::kOk && self.subcode == SubCode::kOverwritten
    }

    /// Returns true iff the status indicates a NotFound error.
    pub fn is_not_found(&self) -> bool {
        self.code == Code::kNotFound
    }

    /// Returns true iff the status indicates a Corruption error.
    pub fn is_corruption(&self) -> bool {
        self.code == Code::kCorruption
    }

    /// Returns true iff the status indicates a NotSupported error.
    pub fn is_not_supported(&self) -> bool {
        self.code == Code::kNotSupported
    }

    /// Returns true iff the status indicates an InvalidArgument error.
    pub fn is_invalid_argument(&self) -> bool {
        self.code == Code::kInvalidArgument
    }

    /// Returns true iff the status indicates an IOError.
    pub fn is_io_error(&self) -> bool {
        self.code == Code::kIOError
    }

    /// Returns true iff the status indicates an MergeInProgress.
    pub fn is_merge_in_progress(&self) -> bool {
        self.code == Code::kMergeInProgress
    }

    /// Returns true iff the status indicates Incomplete
    pub fn is_incomplete(&self) -> bool {
        self.code == Code::kIncomplete
    }

    /// Returns true iff the status indicates Shutdown In progress
    pub fn is_shutdown_in_progress(&self) -> bool {
        self.code == Code::kShutdownInProgress
    }

    pub fn is_timed_out(&self) -> bool {
        self.code == Code::kTimedOut
    }

    pub fn is_aborted(&self) -> bool {
        self.code == Code::kAborted
    }

    pub fn is_lock_limit(&self) -> bool {
        self.code == Code::kAborted && self.subcode == SubCode::kLockLimit
    }

    /// Returns true iff the status indicates that a resource is Busy and temporarily could not be
    /// acquired.
    pub fn is_busy(&self) -> bool {
        self.code == Code::kBusy
    }

    pub fn is_deadlock(&self) -> bool {
        self.code == Code::kBusy && self.subcode == SubCode::kDeadlock
    }

    /// Returns true iff the status indicated that the operation has Expired.
    pub fn is_expired(&self) -> bool {
        self.code == Code::kExpired
    }

    /// Returns true iff the status indicates a TryAgain error. This usually means that the
    /// operation failed, but may succeed if re-attempted.
    pub fn is_try_again(&self) -> bool {
        self.code == Code::kTryAgain
    }

    /// Returns true iff the status indicates the proposed compaction is too large
    pub fn is_compaction_too_large(&self) -> bool {
        self.code == Code::kCompactionTooLarge
    }

    /// Returns true iff the status indicates Column Family Dropped
    pub fn is_column_family_dropped(&self) -> bool {
        self.code == Code::kColumnFamilyDropped
    }

    /// Returns true iff the status indicates a NoSpace error. This is caused by an I/O error
    /// returning the specific "out of space" error condition. Stricto sensu, an NoSpace error is an
    /// I/O error with a specific subcode, enabling users to take the appropriate action if needed
    pub fn is_no_space(&self) -> bool {
        self.code == Code::kIOError && self.subcode == SubCode::kNoSpace
    }

    /// Returns true iff the status indicates a memory limit error. There may be cases where we
    /// limit the memory used in certain operations (eg. the size of a write batch) in order to
    /// avoid out of memory exceptions.
    pub fn is_memory_limit(&self) -> bool {
        self.code == Code::kAborted && self.subcode == SubCode::kMemoryLimit
    }

    /// Returns true iff the status indicates a PathNotFound error. This is caused by an I/O error
    /// returning the specific "no such file or directory" error condition. A PathNotFound error is
    /// an I/O error with a specific subcode, enabling users to take appropriate action if necessary
    pub fn is_path_not_found(&self) -> bool {
        (self.code == Code::kIOError || self.code == Code::kNotFound)
            && self.subcode == SubCode::kPathNotFound
    }

    /// Returns true iff the status indicates manual compaction paused. This is caused by a call to
    /// PauseManualCompaction
    pub fn is_manual_compaction_paused(&self) -> bool {
        self.code == Code::kIncomplete && self.subcode == SubCode::kManualCompactionPaused
    }

    /// Returns true iff the status indicates a TxnNotPrepared error.
    pub fn is_txn_not_prepared(&self) -> bool {
        self.code == Code::kInvalidArgument && self.subcode == SubCode::kTxnNotPrepared
    }

    /// Returns true iff the status indicates a IOFenced error.
    pub fn is_io_fenced(&self) -> bool {
        self.code == Code::kIOError && self.subcode == SubCode::kIOFenced
    }

    pub fn to_string(&self) -> UniquePtr<CxxString> {
        let msg = match self.code {
            Code::kOk => {
                let mut s = crate::ffi::make_string();
                s.pin_mut().push_str("OK");
                return s;
            }
            Code::kNotFound => "NotFound: ",
            Code::kCorruption => "Corruption: ",
            Code::kNotSupported => "Not implemented: ",
            Code::kInvalidArgument => "Invalid argument: ",
            Code::kIOError => "IO error: ",
            Code::kMergeInProgress => "Merge in progress: ",
            Code::kIncomplete => "Result incomplete: ",
            Code::kShutdownInProgress => "Shutdown in progress: ",
            Code::kTimedOut => "Operation timed out: ",
            Code::kAborted => "Operation aborted: ",
            Code::kBusy => "Resource busy: ",
            Code::kExpired => "Operation expired: ",
            Code::kTryAgain => "Operation failed. Try again.: ",
            Code::kCompactionTooLarge => "Compaction too large: ",
            Code::kColumnFamilyDropped => "Column family dropped: ",
            x => unreachable!("{:?} is not a valid status code", x),
        };

        let mut res = crate::ffi::make_string();
        res.pin_mut().push_str(msg);

        if self.subcode != SubCode::kNone {
            let subcode_msg = match self.subcode {
                SubCode::kMutexTimeout => "Timeout Acquiring Mutex",
                SubCode::kLockTimeout => "Timeout waiting to lock key",
                SubCode::kLockLimit => "Failed to acquire lock due to max_num_locks limit",
                SubCode::kNoSpace => "No space left on device",
                SubCode::kDeadlock => "Deadlock",
                SubCode::kStaleFile => "Stale file handle",
                SubCode::kMemoryLimit => "Memory limit reached",
                SubCode::kSpaceLimit => "Space limit reached",
                SubCode::kPathNotFound => "No such file or directory",
                SubCode::KMergeOperandsInsufficientCapacity => {
                    "Insufficient capacity for merge operands"
                }
                SubCode::kManualCompactionPaused => "Manual compaction paused",
                SubCode::kOverwritten => " (overwritten)",
                SubCode::kTxnNotPrepared => "Txn not prepared",
                SubCode::kIOFenced => "IO fenced off",
                SubCode::kMergeOperatorFailed => "Merge operator failed",
                x => unreachable!("{:?} is not a valid status subcode", x),
            };

            res.pin_mut().push_str(subcode_msg);
        }

        if !self.state.is_null() {
            if self.subcode != SubCode::kNone {
                res.pin_mut().push_str(": ");
            }
            res.pin_mut().push_bytes(self.state.as_bytes());
        }

        res
    }

    pub fn copy_from(&mut self, other: &Status) {
        self.code = other.code;
        self.subcode = other.subcode;
        self.severity = other.severity;
        self.retryable = other.retryable;
        self.data_loss = other.data_loss;
        self.scope = other.scope;
        self.state = other.copy_state();
    }

    pub fn move_from(&mut self, other: Status) {
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
}

impl Default for Status {
    fn default() -> Self {
        Self {
            code: Code::kOk,
            subcode: SubCode::kNone,
            severity: Severity::kNoError,
            retryable: false,
            data_loss: false,
            scope: 0,
            state: UniquePtr::null(),
        }
    }
}

impl Clone for Status {
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

impl PartialEq<Self> for Status {
    fn eq(&self, other: &Self) -> bool {
        // Original implementation only checks whether code == code.
        self.code == other.code
    }
}

impl Eq for Status {}

pub fn status_new(
    code: Code,
    subcode: SubCode,
    severity: Severity,
    retryable: bool,
    data_loss: bool,
    scope: u8,
    state: UniquePtr<CxxString>,
) -> Status {
    Status {
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
pub fn status_new1() -> Status {
    Status::default()
}

pub fn status_new2(code: Code) -> Status {
    Status {
        code,
        ..Status::default()
    }
}

pub fn status_new3(code: Code, subcode: SubCode) -> Status {
    Status {
        code,
        subcode,
        ..Status::default()
    }
}

pub fn status_new4(
    code: Code,
    subcode: SubCode,
    retryable: bool,
    data_loss: bool,
    scope: u8,
) -> Status {
    Status {
        code,
        subcode,
        retryable,
        data_loss,
        scope,
        ..Status::default()
    }
}

pub fn status_new5(
    code: Code,
    subcode: SubCode,
    msg: &Slice,
    msg2: &Slice,
    sev: Severity,
) -> Status {
    Status::new_with_slices(code, subcode, msg, msg2, sev)
}

pub fn status_new6(code: Code, msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(code, SubCode::kNone, msg, msg2, Severity::kNoError)
}

pub fn status_new7(code: Code, subcode: SubCode, msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(code, subcode, msg, msg2, Severity::kNoError)
}

pub fn status_new8(code: Code, subcode: SubCode, sev: Severity, msg: &Slice) -> Status {
    Status::new_with_messages(
        code,
        subcode,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        sev,
    )
}

pub fn status_new9(status: &Status, severity: Severity) -> Status {
    let state = status.copy_state();
    Status {
        code: status.code,
        subcode: status.subcode,
        severity,
        retryable: status.retryable,
        data_loss: status.data_loss,
        scope: status.scope,
        state,
    }
}

pub fn status_ok() -> Status {
    Status::default()
}

pub fn status_ok_overwritten() -> Status {
    Status {
        code: Code::kOk,
        subcode: SubCode::kOverwritten,
        ..Status::default()
    }
}

pub fn status_not_found() -> Status {
    Status {
        code: Code::kNotFound,
        ..Status::default()
    }
}

pub fn status_not_found1(msg: SubCode) -> Status {
    Status {
        code: Code::kNotFound,
        subcode: msg,
        ..Status::default()
    }
}

pub fn status_not_found2(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kNotFound,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_not_found3(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kNotFound,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_not_found4(subcode: SubCode, msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(Code::kNotFound, subcode, msg, msg2, Severity::kNoError)
}

pub fn status_not_found5(subcode: SubCode, msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kNotFound,
        subcode,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_corruption(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kCorruption,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_corruption2(subcode: SubCode) -> Status {
    Status {
        code: Code::kCorruption,
        subcode,
        ..Status::default()
    }
}

pub fn status_corruption3() -> Status {
    Status {
        code: Code::kCorruption,
        ..Status::default()
    }
}

pub fn status_not_supported(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kNotSupported,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_not_supported2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kNotSupported,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_not_supported3(subcode: SubCode) -> Status {
    Status {
        code: Code::kNotSupported,
        subcode,
        ..Status::default()
    }
}

pub fn status_not_supported4() -> Status {
    Status {
        code: Code::kNotSupported,
        ..Status::default()
    }
}

pub fn status_invalid_argument(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kInvalidArgument,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_invalid_argument2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kInvalidArgument,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_invalid_argument3(subcode: SubCode) -> Status {
    Status {
        code: Code::kInvalidArgument,
        subcode,
        ..Status::default()
    }
}

pub fn status_invalid_argument4() -> Status {
    Status {
        code: Code::kInvalidArgument,
        ..Status::default()
    }
}

pub fn status_io_error(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kIOError,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}
pub fn status_io_error2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kIOError,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}
pub fn status_io_error3(subcode: SubCode) -> Status {
    Status {
        code: Code::kIOError,
        subcode,
        ..Status::default()
    }
}
pub fn status_io_error4() -> Status {
    Status {
        code: Code::kIOError,
        ..Status::default()
    }
}

pub fn status_merge_in_progress(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kMergeInProgress,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_merge_in_progress2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kMergeInProgress,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_merge_in_progress3(subcode: SubCode) -> Status {
    Status {
        code: Code::kMergeInProgress,
        subcode,
        ..Status::default()
    }
}

pub fn status_merge_in_progress4() -> Status {
    Status {
        code: Code::kMergeInProgress,
        ..Status::default()
    }
}

pub fn status_incomplete(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kIncomplete,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_incomplete2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kIncomplete,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_incomplete3(subcode: SubCode) -> Status {
    Status {
        code: Code::kIncomplete,
        subcode,
        ..Status::default()
    }
}

pub fn status_incomplete4() -> Status {
    Status {
        code: Code::kIncomplete,
        ..Status::default()
    }
}

pub fn status_shutdown_in_progress(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kShutdownInProgress,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_shutdown_in_progress2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kShutdownInProgress,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_shutdown_in_progress3(subcode: SubCode) -> Status {
    Status {
        code: Code::kShutdownInProgress,
        subcode,
        ..Status::default()
    }
}

pub fn status_shutdown_in_progress4() -> Status {
    Status {
        code: Code::kShutdownInProgress,
        ..Status::default()
    }
}

pub fn status_aborted(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kAborted,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_aborted2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kAborted,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_aborted3(subcode: SubCode) -> Status {
    Status {
        code: Code::kAborted,
        subcode,
        ..Status::default()
    }
}

pub fn status_aborted4() -> Status {
    Status {
        code: Code::kAborted,
        ..Status::default()
    }
}

pub fn status_busy(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(Code::kBusy, SubCode::kNone, msg, msg2, Severity::kNoError)
}

pub fn status_busy2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kBusy,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_busy3(subcode: SubCode) -> Status {
    Status {
        code: Code::kBusy,
        subcode,
        ..Status::default()
    }
}

pub fn status_busy4() -> Status {
    Status {
        code: Code::kBusy,
        ..Status::default()
    }
}

pub fn status_timed_out(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kTimedOut,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_timed_out2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kTimedOut,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_timed_out3(subcode: SubCode) -> Status {
    Status {
        code: Code::kTimedOut,
        subcode,
        ..Status::default()
    }
}

pub fn status_timed_out4() -> Status {
    Status {
        code: Code::kTimedOut,
        ..Status::default()
    }
}

pub fn status_expired(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kTimedOut,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_expired2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kExpired,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_expired3(subcode: SubCode) -> Status {
    Status {
        code: Code::kExpired,
        subcode,
        ..Status::default()
    }
}

pub fn status_expired4() -> Status {
    Status {
        code: Code::kExpired,
        ..Status::default()
    }
}

pub fn status_try_again(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kTryAgain,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_try_again2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kTryAgain,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_try_again3(subcode: SubCode) -> Status {
    Status {
        code: Code::kTryAgain,
        subcode,
        ..Status::default()
    }
}

pub fn status_try_again4() -> Status {
    Status {
        code: Code::kTryAgain,
        ..Status::default()
    }
}

pub fn status_compaction_too_large(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kCompactionTooLarge,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_compaction_too_large2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kCompactionTooLarge,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_compaction_too_large3(subcode: SubCode) -> Status {
    Status {
        code: Code::kCompactionTooLarge,
        subcode,
        ..Status::default()
    }
}

pub fn status_compaction_too_large4() -> Status {
    Status {
        code: Code::kCompactionTooLarge,
        ..Status::default()
    }
}

pub fn status_column_family_dropped(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kColumnFamilyDropped,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_column_family_dropped2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kColumnFamilyDropped,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_column_family_dropped3(subcode: SubCode) -> Status {
    Status {
        code: Code::kColumnFamilyDropped,
        subcode,
        ..Status::default()
    }
}

pub fn status_column_family_dropped4() -> Status {
    Status {
        code: Code::kColumnFamilyDropped,
        ..Status::default()
    }
}

pub fn status_no_space(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kIOError,
        SubCode::kNoSpace,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_no_space2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kIOError,
        SubCode::kNoSpace,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_no_space3() -> Status {
    Status {
        code: Code::kIOError,
        subcode: SubCode::kNoSpace,
        ..Status::default()
    }
}

pub fn status_memory_limit(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kAborted,
        SubCode::kMemoryLimit,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_memory_limit2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kAborted,
        SubCode::kMemoryLimit,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_memory_limit3() -> Status {
    Status {
        code: Code::kAborted,
        subcode: SubCode::kMemoryLimit,
        ..Status::default()
    }
}

pub fn status_space_limit(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kIOError,
        SubCode::kSpaceLimit,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_space_limit2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kIOError,
        SubCode::kSpaceLimit,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_space_limit3() -> Status {
    Status {
        code: Code::kIOError,
        subcode: SubCode::kSpaceLimit,
        ..Status::default()
    }
}

pub fn status_path_not_found(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kIOError,
        SubCode::kPathNotFound,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_path_not_found2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kIOError,
        SubCode::kPathNotFound,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_path_not_found3() -> Status {
    Status {
        code: Code::kIOError,
        subcode: SubCode::kPathNotFound,
        ..Status::default()
    }
}

pub fn status_txn_not_prepared(msg: &Slice, msg2: &Slice) -> Status {
    Status::new_with_slices(
        Code::kInvalidArgument,
        SubCode::kTxnNotPrepared,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn status_txn_not_prepared2(msg: &Slice) -> Status {
    Status::new_with_messages(
        Code::kInvalidArgument,
        SubCode::kTxnNotPrepared,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn status_txn_not_prepared3() -> Status {
    Status {
        code: Code::kInvalidArgument,
        subcode: SubCode::kTxnNotPrepared,
        ..Status::default()
    }
}

pub fn status_copy_state(s: &CxxString) -> UniquePtr<CxxString> {
    let mut res = crate::ffi::make_string();
    res.pin_mut().push_bytes(s.as_bytes());
    res
}

pub fn status_copy_append_message(status: &Status, delim: &Slice, msg: &Slice) -> Status {
    let mut new_msg = crate::ffi::make_string();
    new_msg.pin_mut().push_bytes(status.state.as_bytes());
    new_msg
        .pin_mut()
        .push_bytes(delim.to_unique_ptr_string().as_bytes());
    new_msg
        .pin_mut()
        .push_bytes(msg.to_unique_ptr_string().as_bytes());

    Status {
        code: status.code,
        subcode: status.subcode,
        severity: status.severity,
        retryable: status.retryable,
        data_loss: status.data_loss,
        scope: status.scope,
        state: new_msg,
    }
}

pub fn status_move(status: Status) -> Status {
    Status {
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
        let status = Status::default();
        let s = status.to_string();
        assert_eq!(s.to_str().unwrap(), "OK");
    }

    #[test]
    fn to_string_memory_limit() {
        let status = Status {
            code: Code::kAborted,
            subcode: SubCode::kMemoryLimit,
            ..Status::default()
        };
        let s = status.to_string();
        assert_eq!(
            s.to_str().unwrap(),
            "Operation aborted: Memory limit reached"
        );
    }
}
