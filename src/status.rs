use crate::status::ffi::{Code, RsStatus, Severity, SubCode};

#[cxx::bridge(namespace = "rocksdb")]
pub mod ffi {
    extern "Rust" {
        #[cxx_name = "RsStatus_new"]
        fn rs_status_new(
            code: Code,
            subcode: SubCode,
            severity: Severity,
            retryable: bool,
            data_loss: bool,
            scope: u8,
        ) -> RsStatus;
    }

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

    enum Severity {
        kNoError = 0,
        kSoftError = 1,
        kHardError = 2,
        kFatalError = 3,
        kUnrecoverableError = 4,
        kMaxSeverity,
    }

    struct RsStatus {
        code: Code,
        subcode: SubCode,
        severity: Severity,
        retryable: bool,
        data_loss: bool,
        scope: u8,
    }
}

pub fn rs_status_new(
    code: Code,
    subcode: SubCode,
    severity: Severity,
    retryable: bool,
    data_loss: bool,
    scope: u8,
) -> RsStatus {
    RsStatus {
        code,
        subcode,
        severity,
        retryable,
        data_loss,
        scope,
    }
}
