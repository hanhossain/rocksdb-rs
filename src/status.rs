use crate::status::ffi::{Code, Severity, SubCode};

#[cxx::bridge(namespace = "rocksdb")]
pub mod ffi {
    extern "Rust" {
        type RsStatus;

        #[cxx_name = "RsStatus_new"]
        fn rs_status_new(
            code: Code,
            subcode: SubCode,
            severity: Severity,
            retryable: bool,
            data_loss: bool,
            scope: u8,
        ) -> Box<RsStatus>;

        #[cxx_name = "Code"]
        fn code(&self) -> Code;
        #[cxx_name = "SetCode"]
        fn set_code(&mut self, code: Code);
        #[cxx_name = "SubCode"]
        fn subcode(&self) -> SubCode;
        #[cxx_name = "SetSubCode"]
        fn set_subcode(&mut self, subcode: SubCode);
        #[cxx_name = "Severity"]
        fn severity(&self) -> Severity;
        #[cxx_name = "SetSeverity"]
        fn set_severity(&mut self, severity: Severity);
        #[cxx_name = "Retryable"]
        fn retryable(&self) -> bool;
        #[cxx_name = "SetRetryable"]
        fn set_retryable(&mut self, retryable: bool);
        #[cxx_name = "DataLoss"]
        fn data_loss(&self) -> bool;
        #[cxx_name = "SetDataLoss"]
        fn set_data_loss(&mut self, data_loss: bool);
        #[cxx_name = "Scope"]
        fn scope(&self) -> u8;
        #[cxx_name = "SetScope"]
        fn set_scope(&mut self, scope: u8);
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
}

pub struct RsStatus {
    code: Code,
    subcode: SubCode,
    severity: Severity,
    retryable: bool,
    data_loss: bool,
    scope: u8,
}

impl RsStatus {
    pub fn code(&self) -> Code {
        self.code
    }

    pub fn set_code(&mut self, code: Code) {
        self.code = code;
    }

    pub fn subcode(&self) -> SubCode {
        self.subcode
    }

    pub fn set_subcode(&mut self, subcode: SubCode) {
        self.subcode = subcode;
    }

    pub fn severity(&self) -> Severity {
        self.severity
    }

    pub fn set_severity(&mut self, severity: Severity) {
        self.severity = severity;
    }

    pub fn retryable(&self) -> bool {
        self.retryable
    }

    pub fn set_retryable(&mut self, retryable: bool) {
        self.retryable = retryable;
    }

    pub fn data_loss(&self) -> bool {
        self.data_loss
    }

    pub fn set_data_loss(&mut self, data_loss: bool) {
        self.data_loss = data_loss;
    }

    pub fn scope(&self) -> u8 {
        self.scope
    }

    pub fn set_scope(&mut self, scope: u8) {
        self.scope = scope;
    }
}

pub fn rs_status_new(
    code: Code,
    subcode: SubCode,
    severity: Severity,
    retryable: bool,
    data_loss: bool,
    scope: u8,
) -> Box<RsStatus> {
    Box::new(RsStatus {
        code,
        subcode,
        severity,
        retryable,
        data_loss,
        scope,
    })
}
