use cxx::{CxxString, UniquePtr};

#[cxx::bridge(namespace = "rocksdb_rs::status")]
pub(crate) mod ffix {
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

        #[namespace = "rocksdb"]
        type Slice = crate::ffi::rocksdb::Slice;
    }
}

impl ffix::Status {
    fn new_with_slices(
        code: ffix::Code,
        subcode: ffix::SubCode,
        msg: &ffix::Slice,
        msg2: &ffix::Slice,
        sev: ffix::Severity,
    ) -> ffix::Status {
        assert_ne!(subcode, ffix::SubCode::kMaxSubCode);
        let msg = msg.ToUniquePtrString();
        let msg2 = msg2.ToUniquePtrString();

        ffix::Status::new_with_messages(code, subcode, msg, msg2, sev)
    }

    fn new_with_messages(
        code: ffix::Code,
        subcode: ffix::SubCode,
        mut msg: UniquePtr<CxxString>,
        msg2: UniquePtr<CxxString>,
        severity: ffix::Severity,
    ) -> ffix::Status {
        assert_ne!(subcode, ffix::SubCode::kMaxSubCode);

        if !msg2.is_null() && msg2.len() > 0 {
            msg.pin_mut().push_str(": ");

            // Can't treat this as a string yet because it's not guaranteed to be valid UTF-8.
            msg.pin_mut().push_bytes(msg2.as_bytes());
        }

        ffix::Status {
            code,
            subcode,
            severity,
            state: msg,
            ..ffix::Status::default()
        }
    }

    fn code(&self) -> ffix::Code {
        self.code
    }

    fn subcode(&self) -> ffix::SubCode {
        self.subcode
    }

    fn severity(&self) -> ffix::Severity {
        self.severity
    }

    pub(crate) fn get_state(&self) -> &UniquePtr<CxxString> {
        &self.state
    }

    /// Returns true iff the status indicates success.
    pub(crate) fn ok(&self) -> bool {
        self.code == ffix::Code::kOk
    }

    /// Returns true iff the status indicates success *with* something overwritten
    fn is_ok_overwritten(&self) -> bool {
        self.code == ffix::Code::kOk && self.subcode == ffix::SubCode::kOverwritten
    }

    /// Returns true iff the status indicates a NotFound error.
    pub(crate) fn is_not_found(&self) -> bool {
        self.code == ffix::Code::kNotFound
    }

    /// Returns true iff the status indicates a Corruption error.
    pub(crate) fn is_corruption(&self) -> bool {
        self.code == ffix::Code::kCorruption
    }

    /// Returns true iff the status indicates a NotSupported error.
    pub(crate) fn is_not_supported(&self) -> bool {
        self.code == ffix::Code::kNotSupported
    }

    /// Returns true iff the status indicates an InvalidArgument error.
    pub(crate) fn is_invalid_argument(&self) -> bool {
        self.code == ffix::Code::kInvalidArgument
    }

    /// Returns true iff the status indicates an IOError.
    pub(crate) fn is_io_error(&self) -> bool {
        self.code == ffix::Code::kIOError
    }

    /// Returns true iff the status indicates an MergeInProgress.
    fn is_merge_in_progress(&self) -> bool {
        self.code == ffix::Code::kMergeInProgress
    }

    /// Returns true iff the status indicates Incomplete
    pub(crate) fn is_incomplete(&self) -> bool {
        self.code == ffix::Code::kIncomplete
    }

    /// Returns true iff the status indicates Shutdown In progress
    pub(crate) fn is_shutdown_in_progress(&self) -> bool {
        self.code == ffix::Code::kShutdownInProgress
    }

    fn is_timed_out(&self) -> bool {
        self.code == ffix::Code::kTimedOut
    }

    pub(crate) fn is_aborted(&self) -> bool {
        self.code == ffix::Code::kAborted
    }

    fn is_lock_limit(&self) -> bool {
        self.code == ffix::Code::kAborted && self.subcode == ffix::SubCode::kLockLimit
    }

    /// Returns true iff the status indicates that a resource is Busy and temporarily could not be
    /// acquired.
    pub(crate) fn is_busy(&self) -> bool {
        self.code == ffix::Code::kBusy
    }

    fn is_deadlock(&self) -> bool {
        self.code == ffix::Code::kBusy && self.subcode == ffix::SubCode::kDeadlock
    }

    /// Returns true iff the status indicated that the operation has Expired.
    fn is_expired(&self) -> bool {
        self.code == ffix::Code::kExpired
    }

    /// Returns true iff the status indicates a TryAgain error. This usually means that the
    /// operation failed, but may succeed if re-attempted.
    pub(crate) fn is_try_again(&self) -> bool {
        self.code == ffix::Code::kTryAgain
    }

    /// Returns true iff the status indicates the proposed compaction is too large
    fn is_compaction_too_large(&self) -> bool {
        self.code == ffix::Code::kCompactionTooLarge
    }

    /// Returns true iff the status indicates Column Family Dropped
    pub(crate) fn is_column_family_dropped(&self) -> bool {
        self.code == ffix::Code::kColumnFamilyDropped
    }

    /// Returns true iff the status indicates a NoSpace error. This is caused by an I/O error
    /// returning the specific "out of space" error condition. Stricto sensu, an NoSpace error is an
    /// I/O error with a specific subcode, enabling users to take the appropriate action if needed
    fn is_no_space(&self) -> bool {
        self.code == ffix::Code::kIOError && self.subcode == ffix::SubCode::kNoSpace
    }

    /// Returns true iff the status indicates a memory limit error. There may be cases where we
    /// limit the memory used in certain operations (eg. the size of a write batch) in order to
    /// avoid out of memory exceptions.
    fn is_memory_limit(&self) -> bool {
        self.code == ffix::Code::kAborted && self.subcode == ffix::SubCode::kMemoryLimit
    }

    /// Returns true iff the status indicates a PathNotFound error. This is caused by an I/O error
    /// returning the specific "no such file or directory" error condition. A PathNotFound error is
    /// an I/O error with a specific subcode, enabling users to take appropriate action if necessary
    pub(crate) fn is_path_not_found(&self) -> bool {
        (self.code == ffix::Code::kIOError || self.code == ffix::Code::kNotFound)
            && self.subcode == ffix::SubCode::kPathNotFound
    }

    /// Returns true iff the status indicates manual compaction paused. This is caused by a call to
    /// PauseManualCompaction
    fn is_manual_compaction_paused(&self) -> bool {
        self.code == ffix::Code::kIncomplete
            && self.subcode == ffix::SubCode::kManualCompactionPaused
    }

    /// Returns true iff the status indicates a TxnNotPrepared error.
    fn is_txn_not_prepared(&self) -> bool {
        self.code == ffix::Code::kInvalidArgument && self.subcode == ffix::SubCode::kTxnNotPrepared
    }

    /// Returns true iff the status indicates a IOFenced error.
    pub(crate) fn is_io_fenced(&self) -> bool {
        self.code == ffix::Code::kIOError && self.subcode == ffix::SubCode::kIOFenced
    }

    pub(crate) fn to_string(&self) -> UniquePtr<CxxString> {
        let msg = match self.code {
            ffix::Code::kOk => {
                let mut s = crate::ffi::make_string("");
                s.pin_mut().push_str("OK");
                return s;
            }
            ffix::Code::kNotFound => "NotFound: ",
            ffix::Code::kCorruption => "Corruption: ",
            ffix::Code::kNotSupported => "Not implemented: ",
            ffix::Code::kInvalidArgument => "Invalid argument: ",
            ffix::Code::kIOError => "IO error: ",
            ffix::Code::kMergeInProgress => "Merge in progress: ",
            ffix::Code::kIncomplete => "Result incomplete: ",
            ffix::Code::kShutdownInProgress => "Shutdown in progress: ",
            ffix::Code::kTimedOut => "Operation timed out: ",
            ffix::Code::kAborted => "Operation aborted: ",
            ffix::Code::kBusy => "Resource busy: ",
            ffix::Code::kExpired => "Operation expired: ",
            ffix::Code::kTryAgain => "Operation failed. Try again.: ",
            ffix::Code::kCompactionTooLarge => "Compaction too large: ",
            ffix::Code::kColumnFamilyDropped => "Column family dropped: ",
            x => unreachable!("{:?} is not a valid status code", x),
        };

        let mut res = crate::ffi::make_string("");
        res.pin_mut().push_str(msg);

        if self.subcode != ffix::SubCode::kNone {
            let subcode_msg = match self.subcode {
                ffix::SubCode::kMutexTimeout => "Timeout Acquiring Mutex",
                ffix::SubCode::kLockTimeout => "Timeout waiting to lock key",
                ffix::SubCode::kLockLimit => "Failed to acquire lock due to max_num_locks limit",
                ffix::SubCode::kNoSpace => "No space left on device",
                ffix::SubCode::kDeadlock => "Deadlock",
                ffix::SubCode::kStaleFile => "Stale file handle",
                ffix::SubCode::kMemoryLimit => "Memory limit reached",
                ffix::SubCode::kSpaceLimit => "Space limit reached",
                ffix::SubCode::kPathNotFound => "No such file or directory",
                ffix::SubCode::KMergeOperandsInsufficientCapacity => {
                    "Insufficient capacity for merge operands"
                }
                ffix::SubCode::kManualCompactionPaused => "Manual compaction paused",
                ffix::SubCode::kOverwritten => " (overwritten)",
                ffix::SubCode::kTxnNotPrepared => "Txn not prepared",
                ffix::SubCode::kIOFenced => "IO fenced off",
                ffix::SubCode::kMergeOperatorFailed => "Merge operator failed",
                x => unreachable!("{:?} is not a valid status subcode", x),
            };

            res.pin_mut().push_str(subcode_msg);
        }

        if !self.state.is_null() {
            if self.subcode != ffix::SubCode::kNone {
                res.pin_mut().push_str(": ");
            }
            res.pin_mut().push_bytes(self.state.as_bytes());
        }

        res
    }

    fn copy_from(&mut self, other: &ffix::Status) {
        self.code = other.code;
        self.subcode = other.subcode;
        self.severity = other.severity;
        self.retryable = other.retryable;
        self.data_loss = other.data_loss;
        self.scope = other.scope;
        self.state = other.copy_state();
    }

    fn move_from(&mut self, other: ffix::Status) {
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

    fn create_vec(&self, n: usize) -> Vec<ffix::Status> {
        let mut v = Vec::new();
        for _ in 0..n {
            v.push(self.clone());
        }
        v
    }

    fn resize_vec(&self, v: &mut Vec<ffix::Status>, n: usize) {
        if n > v.len() {
            for _ in v.len()..n {
                v.push(self.clone());
            }
        } else {
            v.truncate(n);
        }
    }
}

impl Default for ffix::Status {
    fn default() -> Self {
        Self {
            code: ffix::Code::kOk,
            subcode: ffix::SubCode::kNone,
            severity: ffix::Severity::kNoError,
            retryable: false,
            data_loss: false,
            scope: 0,
            state: UniquePtr::null(),
        }
    }
}

impl Clone for ffix::Status {
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

impl PartialEq<Self> for ffix::Status {
    fn eq(&self, other: &Self) -> bool {
        // Original implementation only checks whether code == code.
        self.code == other.code
    }
}

impl Eq for ffix::Status {}

fn status_new(
    code: ffix::Code,
    subcode: ffix::SubCode,
    severity: ffix::Severity,
    retryable: bool,
    data_loss: bool,
    scope: u8,
    state: UniquePtr<CxxString>,
) -> ffix::Status {
    ffix::Status {
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
fn status_new1() -> ffix::Status {
    ffix::Status::default()
}

fn status_new2(code: ffix::Code) -> ffix::Status {
    ffix::Status {
        code,
        ..ffix::Status::default()
    }
}

pub(crate) fn status_new3(code: ffix::Code, subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_new4(
    code: ffix::Code,
    subcode: ffix::SubCode,
    retryable: bool,
    data_loss: bool,
    scope: u8,
) -> ffix::Status {
    ffix::Status {
        code,
        subcode,
        retryable,
        data_loss,
        scope,
        ..ffix::Status::default()
    }
}

fn status_new5(
    code: ffix::Code,
    subcode: ffix::SubCode,
    msg: &ffix::Slice,
    msg2: &ffix::Slice,
    sev: ffix::Severity,
) -> ffix::Status {
    ffix::Status::new_with_slices(code, subcode, msg, msg2, sev)
}

pub(crate) fn status_new6(code: ffix::Code, msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        code,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

pub(crate) fn status_new7(
    code: ffix::Code,
    subcode: ffix::SubCode,
    msg: &ffix::Slice,
    msg2: &ffix::Slice,
) -> ffix::Status {
    ffix::Status::new_with_slices(code, subcode, msg, msg2, ffix::Severity::kNoError)
}

fn status_new8(
    code: ffix::Code,
    subcode: ffix::SubCode,
    sev: ffix::Severity,
    msg: &ffix::Slice,
) -> ffix::Status {
    ffix::Status::new_with_messages(
        code,
        subcode,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        sev,
    )
}

fn status_new9(status: &ffix::Status, severity: ffix::Severity) -> ffix::Status {
    let state = status.copy_state();
    ffix::Status {
        code: status.code,
        subcode: status.subcode,
        severity,
        retryable: status.retryable,
        data_loss: status.data_loss,
        scope: status.scope,
        state,
    }
}

fn status_ok() -> ffix::Status {
    ffix::Status::default()
}

fn status_ok_overwritten() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kOk,
        subcode: ffix::SubCode::kOverwritten,
        ..ffix::Status::default()
    }
}

fn status_not_found() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kNotFound,
        ..ffix::Status::default()
    }
}

fn status_not_found1(msg: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kNotFound,
        subcode: msg,
        ..ffix::Status::default()
    }
}

fn status_not_found2(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kNotFound,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_not_found3(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kNotFound,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_not_found4(
    subcode: ffix::SubCode,
    msg: &ffix::Slice,
    msg2: &ffix::Slice,
) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kNotFound,
        subcode,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_not_found5(subcode: ffix::SubCode, msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kNotFound,
        subcode,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_corruption(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kCorruption,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_corruption2(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kCorruption,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_corruption3() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kCorruption,
        ..ffix::Status::default()
    }
}

fn status_corruption4(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kCorruption,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_not_supported(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kNotSupported,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_not_supported2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kNotSupported,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_not_supported3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kNotSupported,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_not_supported4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kNotSupported,
        ..ffix::Status::default()
    }
}

fn status_invalid_argument(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kInvalidArgument,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_invalid_argument2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kInvalidArgument,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_invalid_argument3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kInvalidArgument,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_invalid_argument4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kInvalidArgument,
        ..ffix::Status::default()
    }
}

fn status_io_error(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kIOError,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}
fn status_io_error2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kIOError,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}
fn status_io_error3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kIOError,
        subcode,
        ..ffix::Status::default()
    }
}
fn status_io_error4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kIOError,
        ..ffix::Status::default()
    }
}

fn status_merge_in_progress(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kMergeInProgress,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_merge_in_progress2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kMergeInProgress,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_merge_in_progress3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kMergeInProgress,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_merge_in_progress4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kMergeInProgress,
        ..ffix::Status::default()
    }
}

fn status_incomplete(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kIncomplete,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_incomplete2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kIncomplete,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_incomplete3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kIncomplete,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_incomplete4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kIncomplete,
        ..ffix::Status::default()
    }
}

fn status_shutdown_in_progress(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kShutdownInProgress,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_shutdown_in_progress2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kShutdownInProgress,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_shutdown_in_progress3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kShutdownInProgress,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_shutdown_in_progress4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kShutdownInProgress,
        ..ffix::Status::default()
    }
}

fn status_aborted(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kAborted,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_aborted2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kAborted,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_aborted3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kAborted,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_aborted4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kAborted,
        ..ffix::Status::default()
    }
}

fn status_busy(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kBusy,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_busy2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kBusy,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_busy3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kBusy,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_busy4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kBusy,
        ..ffix::Status::default()
    }
}

fn status_timed_out(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kTimedOut,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_timed_out2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kTimedOut,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_timed_out3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kTimedOut,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_timed_out4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kTimedOut,
        ..ffix::Status::default()
    }
}

fn status_expired(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kTimedOut,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_expired2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kExpired,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_expired3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kExpired,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_expired4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kExpired,
        ..ffix::Status::default()
    }
}

fn status_try_again(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kTryAgain,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_try_again2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kTryAgain,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_try_again3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kTryAgain,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_try_again4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kTryAgain,
        ..ffix::Status::default()
    }
}

fn status_compaction_too_large(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kCompactionTooLarge,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_compaction_too_large2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kCompactionTooLarge,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_compaction_too_large3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kCompactionTooLarge,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_compaction_too_large4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kCompactionTooLarge,
        ..ffix::Status::default()
    }
}

fn status_column_family_dropped(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kColumnFamilyDropped,
        ffix::SubCode::kNone,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_column_family_dropped2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kColumnFamilyDropped,
        ffix::SubCode::kNone,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_column_family_dropped3(subcode: ffix::SubCode) -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kColumnFamilyDropped,
        subcode,
        ..ffix::Status::default()
    }
}

fn status_column_family_dropped4() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kColumnFamilyDropped,
        ..ffix::Status::default()
    }
}

fn status_no_space(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kIOError,
        ffix::SubCode::kNoSpace,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_no_space2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kIOError,
        ffix::SubCode::kNoSpace,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_no_space3() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kIOError,
        subcode: ffix::SubCode::kNoSpace,
        ..ffix::Status::default()
    }
}

fn status_memory_limit(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kAborted,
        ffix::SubCode::kMemoryLimit,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_memory_limit2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kAborted,
        ffix::SubCode::kMemoryLimit,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_memory_limit3() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kAborted,
        subcode: ffix::SubCode::kMemoryLimit,
        ..ffix::Status::default()
    }
}

fn status_space_limit(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kIOError,
        ffix::SubCode::kSpaceLimit,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_space_limit2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kIOError,
        ffix::SubCode::kSpaceLimit,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_space_limit3() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kIOError,
        subcode: ffix::SubCode::kSpaceLimit,
        ..ffix::Status::default()
    }
}

fn status_path_not_found(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kIOError,
        ffix::SubCode::kPathNotFound,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_path_not_found2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kIOError,
        ffix::SubCode::kPathNotFound,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_path_not_found3() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kIOError,
        subcode: ffix::SubCode::kPathNotFound,
        ..ffix::Status::default()
    }
}

fn status_txn_not_prepared(msg: &ffix::Slice, msg2: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_slices(
        ffix::Code::kInvalidArgument,
        ffix::SubCode::kTxnNotPrepared,
        msg,
        msg2,
        ffix::Severity::kNoError,
    )
}

fn status_txn_not_prepared2(msg: &ffix::Slice) -> ffix::Status {
    ffix::Status::new_with_messages(
        ffix::Code::kInvalidArgument,
        ffix::SubCode::kTxnNotPrepared,
        msg.ToUniquePtrString(),
        UniquePtr::null(),
        ffix::Severity::kNoError,
    )
}

fn status_txn_not_prepared3() -> ffix::Status {
    ffix::Status {
        code: ffix::Code::kInvalidArgument,
        subcode: ffix::SubCode::kTxnNotPrepared,
        ..ffix::Status::default()
    }
}

fn status_copy_state(s: &CxxString) -> UniquePtr<CxxString> {
    let mut res = crate::ffi::make_string("");
    res.pin_mut().push_bytes(s.as_bytes());
    res
}

fn status_copy_append_message(
    status: &ffix::Status,
    delim: &ffix::Slice,
    msg: &ffix::Slice,
) -> ffix::Status {
    let mut new_msg = crate::ffi::make_string("");
    new_msg.pin_mut().push_bytes(status.state.as_bytes());
    new_msg
        .pin_mut()
        .push_bytes(delim.ToUniquePtrString().as_bytes());
    new_msg
        .pin_mut()
        .push_bytes(msg.ToUniquePtrString().as_bytes());

    ffix::Status {
        code: status.code,
        subcode: status.subcode,
        severity: status.severity,
        retryable: status.retryable,
        data_loss: status.data_loss,
        scope: status.scope,
        state: new_msg,
    }
}

fn status_move(status: ffix::Status) -> ffix::Status {
    ffix::Status {
        code: status.code,
        subcode: status.subcode,
        severity: status.severity,
        retryable: status.retryable,
        data_loss: status.data_loss,
        scope: status.scope,
        state: status.state,
    }
}

pub(crate) enum Status {
    Ok,
    NotSupported(NotSupported),
}

pub(crate) struct NotSupported {
    pub(crate) msg: String,
}

impl From<Status> for ffix::Status {
    fn from(value: Status) -> Self {
        match value {
            Status::Ok => Self::default(),
            Status::NotSupported(s) => {
                let mut msg = crate::ffi::make_string("");
                msg.pin_mut().push_str(&s.msg);
                Self::new_with_messages(
                    ffix::Code::kNotSupported,
                    ffix::SubCode::kNone,
                    msg,
                    UniquePtr::null(),
                    ffix::Severity::kNoError,
                )
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn to_string_ok() {
        let status = ffix::Status::default();
        let s = status.to_string();
        assert_eq!(s.to_str().unwrap(), "OK");
    }

    #[test]
    fn to_string_memory_limit() {
        let status = ffix::Status {
            code: ffix::Code::kAborted,
            subcode: ffix::SubCode::kMemoryLimit,
            ..ffix::Status::default()
        };
        let s = status.to_string();
        assert_eq!(
            s.to_str().unwrap(),
            "Operation aborted: Memory limit reached"
        );
    }
}
