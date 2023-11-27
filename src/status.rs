use crate::slice::ffi::Slice;
use crate::status::ffi::{Code, RsStatus, Severity, SubCode};
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
    struct RsStatus {
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
        #[cxx_name = "RsStatus_new"]
        fn rs_status_new(
            code: Code,
            subcode: SubCode,
            severity: Severity,
            retryable: bool,
            data_loss: bool,
            scope: u8,
            state: UniquePtr<CxxString>,
        ) -> RsStatus;

        #[cxx_name = "RsStatus_new"]
        fn rs_status_new1() -> RsStatus;

        #[cxx_name = "RsStatus_new"]
        fn rs_status_new2(code: Code) -> RsStatus;

        #[cxx_name = "RsStatus_new"]
        fn rs_status_new3(code: Code, subcode: SubCode) -> RsStatus;

        #[cxx_name = "RsStatus_new"]
        fn rs_status_new4(
            code: Code,
            subcode: SubCode,
            retryable: bool,
            data_loss: bool,
            scope: u8,
        ) -> RsStatus;

        #[cxx_name = "RsStatus_new"]
        fn rs_status_new5(
            code: Code,
            subcode: SubCode,
            msg: &Slice,
            msg2: &Slice,
            sev: Severity,
        ) -> RsStatus;

        #[cxx_name = "RsStatus_new"]
        fn rs_status_new6(code: Code, msg: &Slice, msg2: &Slice) -> RsStatus;

        #[cxx_name = "RsStatus_new"]
        fn rs_status_new7(code: Code, subcode: SubCode, msg: &Slice, msg2: &Slice) -> RsStatus;

        #[cxx_name = "RsStatus_new"]
        fn rs_status_new8(code: Code, subcode: SubCode, sev: Severity, msg: &Slice) -> RsStatus;

        #[cxx_name = "RsStatus_OK"]
        fn rs_status_ok() -> RsStatus;

        #[cxx_name = "RsStatus_OkOverwritten"]
        fn rs_status_ok_overwritten() -> RsStatus;

        #[cxx_name = "RsStatus_NotFound"]
        fn rs_status_not_found() -> RsStatus;
        #[cxx_name = "RsStatus_NotFound"]
        fn rs_status_not_found1(msg: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_NotFound"]
        fn rs_status_not_found2(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_NotFound"]
        fn rs_status_not_found3(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_NotFound"]
        fn rs_status_not_found4(subcode: SubCode, msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_NotFound"]
        fn rs_status_not_found5(subcode: SubCode, msg: &Slice) -> RsStatus;

        #[cxx_name = "RsStatus_Corruption"]
        fn rs_status_corruption(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_Corruption"]
        fn rs_status_corruption2(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_Corruption"]
        fn rs_status_corruption3() -> RsStatus;

        #[cxx_name = "RsStatus_NotSupported"]
        fn rs_status_not_supported(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_NotSupported"]
        fn rs_status_not_supported2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_NotSupported"]
        fn rs_status_not_supported3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_NotSupported"]
        fn rs_status_not_supported4() -> RsStatus;

        #[cxx_name = "RsStatus_InvalidArgument"]
        fn rs_status_invalid_argument(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_InvalidArgument"]
        fn rs_status_invalid_argument2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_InvalidArgument"]
        fn rs_status_invalid_argument3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_InvalidArgument"]
        fn rs_status_invalid_argument4() -> RsStatus;

        #[cxx_name = "RsStatus_IOError"]
        fn rs_status_io_error(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_IOError"]
        fn rs_status_io_error2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_IOError"]
        fn rs_status_io_error3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_IOError"]
        fn rs_status_io_error4() -> RsStatus;

        #[cxx_name = "RsStatus_MergeInProgress"]
        fn rs_status_merge_in_progress(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_MergeInProgress"]
        fn rs_status_merge_in_progress2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_MergeInProgress"]
        fn rs_status_merge_in_progress3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_MergeInProgress"]
        fn rs_status_merge_in_progress4() -> RsStatus;

        #[cxx_name = "RsStatus_Incomplete"]
        fn rs_status_incomplete(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_Incomplete"]
        fn rs_status_incomplete2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_Incomplete"]
        fn rs_status_incomplete3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_Incomplete"]
        fn rs_status_incomplete4() -> RsStatus;

        #[cxx_name = "RsStatus_ShutdownInProgress"]
        fn rs_status_shutdown_in_progress(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_ShutdownInProgress"]
        fn rs_status_shutdown_in_progress2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_ShutdownInProgress"]
        fn rs_status_shutdown_in_progress3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_ShutdownInProgress"]
        fn rs_status_shutdown_in_progress4() -> RsStatus;

        #[cxx_name = "RsStatus_Aborted"]
        fn rs_status_aborted(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_Aborted"]
        fn rs_status_aborted2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_Aborted"]
        fn rs_status_aborted3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_Aborted"]
        fn rs_status_aborted4() -> RsStatus;

        #[cxx_name = "RsStatus_Busy"]
        fn rs_status_busy(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_Busy"]
        fn rs_status_busy2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_Busy"]
        fn rs_status_busy3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_Busy"]
        fn rs_status_busy4() -> RsStatus;

        #[cxx_name = "RsStatus_TimedOut"]
        fn rs_status_timed_out(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_TimedOut"]
        fn rs_status_timed_out2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_TimedOut"]
        fn rs_status_timed_out3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_TimedOut"]
        fn rs_status_timed_out4() -> RsStatus;

        #[cxx_name = "RsStatus_Expired"]
        fn rs_status_expired(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_Expired"]
        fn rs_status_expired2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_Expired"]
        fn rs_status_expired3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_Expired"]
        fn rs_status_expired4() -> RsStatus;

        #[cxx_name = "RsStatus_TryAgain"]
        fn rs_status_try_again(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_TryAgain"]
        fn rs_status_try_again2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_TryAgain"]
        fn rs_status_try_again3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_TryAgain"]
        fn rs_status_try_again4() -> RsStatus;

        #[cxx_name = "RsStatus_CompactionTooLarge"]
        fn rs_status_compaction_too_large(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_CompactionTooLarge"]
        fn rs_status_compaction_too_large2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_CompactionTooLarge"]
        fn rs_status_compaction_too_large3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_CompactionTooLarge"]
        fn rs_status_compaction_too_large4() -> RsStatus;

        #[cxx_name = "RsStatus_ColumnFamilyDropped"]
        fn rs_status_column_family_dropped(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_ColumnFamilyDropped"]
        fn rs_status_column_family_dropped2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_ColumnFamilyDropped"]
        fn rs_status_column_family_dropped3(subcode: SubCode) -> RsStatus;
        #[cxx_name = "RsStatus_ColumnFamilyDropped"]
        fn rs_status_column_family_dropped4() -> RsStatus;

        #[cxx_name = "RsStatus_NoSpace"]
        fn rs_status_no_space(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_NoSpace"]
        fn rs_status_no_space2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_NoSpace"]
        fn rs_status_no_space3() -> RsStatus;

        #[cxx_name = "RsStatus_MemoryLimit"]
        fn rs_status_memory_limit(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_MemoryLimit"]
        fn rs_status_memory_limit2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_MemoryLimit"]
        fn rs_status_memory_limit3() -> RsStatus;

        #[cxx_name = "RsStatus_SpaceLimit"]
        fn rs_status_space_limit(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_SpaceLimit"]
        fn rs_status_space_limit2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_SpaceLimit"]
        fn rs_status_space_limit3() -> RsStatus;

        #[cxx_name = "RsStatus_PathNotFound"]
        fn rs_status_path_not_found(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_PathNotFound"]
        fn rs_status_path_not_found2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_PathNotFound"]
        fn rs_status_path_not_found3() -> RsStatus;

        #[cxx_name = "RsStatus_TxnNotPrepared"]
        fn rs_status_txn_not_prepared(msg: &Slice, msg2: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_TxnNotPrepared"]
        fn rs_status_txn_not_prepared2(msg: &Slice) -> RsStatus;
        #[cxx_name = "RsStatus_TxnNotPrepared"]
        fn rs_status_txn_not_prepared3() -> RsStatus;

        #[cxx_name = "RsStatus_CopyState"]
        fn rs_status_copy_state(s: &CxxString) -> UniquePtr<CxxString>;

        #[cxx_name = "RsStatus_CopyAppendMessage"]
        fn rs_status_copy_append_message(status: &RsStatus, delim: &Slice, msg: &Slice)
            -> RsStatus;

        fn code(self: &RsStatus) -> Code;
        fn subcode(self: &RsStatus) -> SubCode;
        fn severity(self: &RsStatus) -> Severity;
        #[cxx_name = "getState"]
        fn get_state(self: &RsStatus) -> &UniquePtr<CxxString>;
        fn ok(self: &RsStatus) -> bool;
        #[cxx_name = "IsOkOverwritten"]
        fn is_ok_overwritten(self: &RsStatus) -> bool;
        #[cxx_name = "IsNotFound"]
        fn is_not_found(self: &RsStatus) -> bool;
        #[cxx_name = "IsCorruption"]
        fn is_corruption(self: &RsStatus) -> bool;
        #[cxx_name = "IsNotSupported"]
        fn is_not_supported(self: &RsStatus) -> bool;
        #[cxx_name = "IsInvalidArgument"]
        fn is_invalid_argument(self: &RsStatus) -> bool;
        #[cxx_name = "IsIOError"]
        fn is_io_error(self: &RsStatus) -> bool;
        #[cxx_name = "IsMergeInProgress"]
        fn is_merge_in_progress(self: &RsStatus) -> bool;
        #[cxx_name = "IsIncomplete"]
        fn is_incomplete(self: &RsStatus) -> bool;
        #[cxx_name = "IsShutdownInProgress"]
        fn is_shutdown_in_progress(self: &RsStatus) -> bool;
        #[cxx_name = "IsTimedOut"]
        fn is_timed_out(self: &RsStatus) -> bool;
        #[cxx_name = "IsAborted"]
        fn is_aborted(self: &RsStatus) -> bool;
        #[cxx_name = "IsLockLimit"]
        fn is_lock_limit(self: &RsStatus) -> bool;
        #[cxx_name = "IsBusy"]
        fn is_busy(self: &RsStatus) -> bool;
        #[cxx_name = "IsDeadlock"]
        fn is_deadlock(self: &RsStatus) -> bool;
        #[cxx_name = "IsExpired"]
        fn is_expired(self: &RsStatus) -> bool;
        #[cxx_name = "IsTryAgain"]
        fn is_try_again(self: &RsStatus) -> bool;
        #[cxx_name = "IsCompactionTooLarge"]
        fn is_compaction_too_large(self: &RsStatus) -> bool;
        #[cxx_name = "IsColumnFamilyDropped"]
        fn is_column_family_dropped(self: &RsStatus) -> bool;
        #[cxx_name = "IsNoSpace"]
        fn is_no_space(self: &RsStatus) -> bool;
        #[cxx_name = "IsMemoryLimit"]
        fn is_memory_limit(self: &RsStatus) -> bool;
        #[cxx_name = "IsPathNotFound"]
        fn is_path_not_found(self: &RsStatus) -> bool;
        #[cxx_name = "IsManualCompactionPaused"]
        fn is_manual_compaction_paused(self: &RsStatus) -> bool;
        #[cxx_name = "IsTxnNotPrepared"]
        fn is_txn_not_prepared(self: &RsStatus) -> bool;
        #[cxx_name = "IsIOFenced"]
        fn is_io_fenced(self: &RsStatus) -> bool;
        #[cxx_name = "ToString"]
        fn to_string(self: &RsStatus) -> UniquePtr<CxxString>;
    }

    unsafe extern "C++" {
        include!("rocksdb/slice.h");

        type Slice = crate::slice::ffi::Slice;
    }
}

impl RsStatus {
    pub fn new_with_slices(
        code: Code,
        subcode: SubCode,
        msg: &Slice,
        msg2: &Slice,
        sev: Severity,
    ) -> RsStatus {
        assert_ne!(subcode, SubCode::kMaxSubCode);
        let msg = msg.to_unique_ptr_string();
        let msg2 = msg2.to_unique_ptr_string();

        RsStatus::new_with_messages(code, subcode, msg, msg2, sev)
    }

    fn new_with_messages(
        code: Code,
        subcode: SubCode,
        mut msg: UniquePtr<CxxString>,
        msg2: UniquePtr<CxxString>,
        severity: Severity,
    ) -> RsStatus {
        assert_ne!(subcode, SubCode::kMaxSubCode);

        if !msg2.is_null() && msg2.len() > 0 {
            msg.pin_mut().push_str(": ");

            // Can't treat this as a string yet because it's not guaranteed to be valid UTF-8.
            msg.pin_mut().push_bytes(msg2.as_bytes());
        }

        RsStatus {
            code,
            subcode,
            severity,
            state: msg,
            ..RsStatus::default()
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
}

impl Default for RsStatus {
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

pub fn rs_status_new(
    code: Code,
    subcode: SubCode,
    severity: Severity,
    retryable: bool,
    data_loss: bool,
    scope: u8,
    state: UniquePtr<CxxString>,
) -> RsStatus {
    RsStatus {
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
pub fn rs_status_new1() -> RsStatus {
    RsStatus::default()
}

pub fn rs_status_new2(code: Code) -> RsStatus {
    RsStatus {
        code,
        ..RsStatus::default()
    }
}

pub fn rs_status_new3(code: Code, subcode: SubCode) -> RsStatus {
    RsStatus {
        code,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_new4(
    code: Code,
    subcode: SubCode,
    retryable: bool,
    data_loss: bool,
    scope: u8,
) -> RsStatus {
    RsStatus {
        code,
        subcode,
        retryable,
        data_loss,
        scope,
        ..RsStatus::default()
    }
}

pub fn rs_status_new5(
    code: Code,
    subcode: SubCode,
    msg: &Slice,
    msg2: &Slice,
    sev: Severity,
) -> RsStatus {
    RsStatus::new_with_slices(code, subcode, msg, msg2, sev)
}

pub fn rs_status_new6(code: Code, msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(code, SubCode::kNone, msg, msg2, Severity::kNoError)
}

pub fn rs_status_new7(code: Code, subcode: SubCode, msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(code, subcode, msg, msg2, Severity::kNoError)
}

pub fn rs_status_new8(code: Code, subcode: SubCode, sev: Severity, msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        code,
        subcode,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        sev,
    )
}

pub fn rs_status_ok() -> RsStatus {
    RsStatus::default()
}

pub fn rs_status_ok_overwritten() -> RsStatus {
    RsStatus {
        code: Code::kOk,
        subcode: SubCode::kOverwritten,
        ..RsStatus::default()
    }
}

pub fn rs_status_not_found() -> RsStatus {
    RsStatus {
        code: Code::kNotFound,
        ..RsStatus::default()
    }
}

pub fn rs_status_not_found1(msg: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kNotFound,
        subcode: msg,
        ..RsStatus::default()
    }
}

pub fn rs_status_not_found2(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kNotFound,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_not_found3(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kNotFound,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_not_found4(subcode: SubCode, msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(Code::kNotFound, subcode, msg, msg2, Severity::kNoError)
}

pub fn rs_status_not_found5(subcode: SubCode, msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kNotFound,
        subcode,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_corruption(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kCorruption,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_corruption2(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kCorruption,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_corruption3() -> RsStatus {
    RsStatus {
        code: Code::kCorruption,
        ..RsStatus::default()
    }
}

pub fn rs_status_not_supported(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kNotSupported,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_not_supported2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kNotSupported,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_not_supported3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kNotSupported,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_not_supported4() -> RsStatus {
    RsStatus {
        code: Code::kNotSupported,
        ..RsStatus::default()
    }
}

pub fn rs_status_invalid_argument(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kInvalidArgument,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_invalid_argument2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kInvalidArgument,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_invalid_argument3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kInvalidArgument,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_invalid_argument4() -> RsStatus {
    RsStatus {
        code: Code::kInvalidArgument,
        ..RsStatus::default()
    }
}

pub fn rs_status_io_error(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kIOError,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}
pub fn rs_status_io_error2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kIOError,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}
pub fn rs_status_io_error3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kIOError,
        subcode,
        ..RsStatus::default()
    }
}
pub fn rs_status_io_error4() -> RsStatus {
    RsStatus {
        code: Code::kIOError,
        ..RsStatus::default()
    }
}

pub fn rs_status_merge_in_progress(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kMergeInProgress,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_merge_in_progress2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kMergeInProgress,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_merge_in_progress3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kMergeInProgress,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_merge_in_progress4() -> RsStatus {
    RsStatus {
        code: Code::kMergeInProgress,
        ..RsStatus::default()
    }
}

pub fn rs_status_incomplete(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kIncomplete,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_incomplete2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kIncomplete,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_incomplete3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kIncomplete,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_incomplete4() -> RsStatus {
    RsStatus {
        code: Code::kIncomplete,
        ..RsStatus::default()
    }
}

pub fn rs_status_shutdown_in_progress(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kShutdownInProgress,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_shutdown_in_progress2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kShutdownInProgress,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_shutdown_in_progress3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kShutdownInProgress,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_shutdown_in_progress4() -> RsStatus {
    RsStatus {
        code: Code::kShutdownInProgress,
        ..RsStatus::default()
    }
}

pub fn rs_status_aborted(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kAborted,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_aborted2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kAborted,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_aborted3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kAborted,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_aborted4() -> RsStatus {
    RsStatus {
        code: Code::kAborted,
        ..RsStatus::default()
    }
}

pub fn rs_status_busy(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(Code::kBusy, SubCode::kNone, msg, msg2, Severity::kNoError)
}

pub fn rs_status_busy2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kBusy,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_busy3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kBusy,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_busy4() -> RsStatus {
    RsStatus {
        code: Code::kBusy,
        ..RsStatus::default()
    }
}

pub fn rs_status_timed_out(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kTimedOut,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_timed_out2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kTimedOut,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_timed_out3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kTimedOut,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_timed_out4() -> RsStatus {
    RsStatus {
        code: Code::kTimedOut,
        ..RsStatus::default()
    }
}

pub fn rs_status_expired(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kTimedOut,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_expired2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kExpired,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_expired3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kExpired,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_expired4() -> RsStatus {
    RsStatus {
        code: Code::kExpired,
        ..RsStatus::default()
    }
}

pub fn rs_status_try_again(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kTryAgain,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_try_again2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kTryAgain,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_try_again3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kTryAgain,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_try_again4() -> RsStatus {
    RsStatus {
        code: Code::kTryAgain,
        ..RsStatus::default()
    }
}

pub fn rs_status_compaction_too_large(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kCompactionTooLarge,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_compaction_too_large2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kCompactionTooLarge,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_compaction_too_large3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kCompactionTooLarge,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_compaction_too_large4() -> RsStatus {
    RsStatus {
        code: Code::kCompactionTooLarge,
        ..RsStatus::default()
    }
}

pub fn rs_status_column_family_dropped(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kColumnFamilyDropped,
        SubCode::kNone,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_column_family_dropped2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kColumnFamilyDropped,
        SubCode::kNone,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_column_family_dropped3(subcode: SubCode) -> RsStatus {
    RsStatus {
        code: Code::kColumnFamilyDropped,
        subcode,
        ..RsStatus::default()
    }
}

pub fn rs_status_column_family_dropped4() -> RsStatus {
    RsStatus {
        code: Code::kColumnFamilyDropped,
        ..RsStatus::default()
    }
}

pub fn rs_status_no_space(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kIOError,
        SubCode::kNoSpace,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_no_space2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kIOError,
        SubCode::kNoSpace,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_no_space3() -> RsStatus {
    RsStatus {
        code: Code::kIOError,
        subcode: SubCode::kNoSpace,
        ..RsStatus::default()
    }
}

pub fn rs_status_memory_limit(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kAborted,
        SubCode::kMemoryLimit,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_memory_limit2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kAborted,
        SubCode::kMemoryLimit,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_memory_limit3() -> RsStatus {
    RsStatus {
        code: Code::kAborted,
        subcode: SubCode::kMemoryLimit,
        ..RsStatus::default()
    }
}

pub fn rs_status_space_limit(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kIOError,
        SubCode::kSpaceLimit,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_space_limit2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kIOError,
        SubCode::kSpaceLimit,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_space_limit3() -> RsStatus {
    RsStatus {
        code: Code::kIOError,
        subcode: SubCode::kSpaceLimit,
        ..RsStatus::default()
    }
}

pub fn rs_status_path_not_found(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kIOError,
        SubCode::kPathNotFound,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_path_not_found2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kIOError,
        SubCode::kPathNotFound,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_path_not_found3() -> RsStatus {
    RsStatus {
        code: Code::kIOError,
        subcode: SubCode::kPathNotFound,
        ..RsStatus::default()
    }
}

pub fn rs_status_txn_not_prepared(msg: &Slice, msg2: &Slice) -> RsStatus {
    RsStatus::new_with_slices(
        Code::kInvalidArgument,
        SubCode::kTxnNotPrepared,
        msg,
        msg2,
        Severity::kNoError,
    )
}

pub fn rs_status_txn_not_prepared2(msg: &Slice) -> RsStatus {
    RsStatus::new_with_messages(
        Code::kInvalidArgument,
        SubCode::kTxnNotPrepared,
        msg.to_unique_ptr_string(),
        UniquePtr::null(),
        Severity::kNoError,
    )
}

pub fn rs_status_txn_not_prepared3() -> RsStatus {
    RsStatus {
        code: Code::kInvalidArgument,
        subcode: SubCode::kTxnNotPrepared,
        ..RsStatus::default()
    }
}

pub fn rs_status_copy_state(s: &CxxString) -> UniquePtr<CxxString> {
    let mut res = crate::ffi::make_string();
    res.pin_mut().push_bytes(s.as_bytes());
    res
}

pub fn rs_status_copy_append_message(status: &RsStatus, delim: &Slice, msg: &Slice) -> RsStatus {
    let mut new_msg = crate::ffi::make_string();
    new_msg.pin_mut().push_bytes(status.state.as_bytes());
    new_msg
        .pin_mut()
        .push_bytes(delim.to_unique_ptr_string().as_bytes());
    new_msg
        .pin_mut()
        .push_bytes(msg.to_unique_ptr_string().as_bytes());

    RsStatus {
        code: status.code,
        subcode: status.subcode,
        severity: status.severity,
        retryable: status.retryable,
        data_loss: status.data_loss,
        scope: status.scope,
        state: new_msg,
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn to_string_ok() {
        let status = RsStatus::default();
        let s = status.to_string();
        assert_eq!(s.to_str().unwrap(), "OK");
    }

    #[test]
    fn to_string_memory_limit() {
        let status = RsStatus {
            code: Code::kAborted,
            subcode: SubCode::kMemoryLimit,
            ..RsStatus::default()
        };
        let s = status.to_string();
        assert_eq!(
            s.to_str().unwrap(),
            "Operation aborted: Memory limit reached"
        );
    }
}
