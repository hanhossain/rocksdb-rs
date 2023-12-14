#[cxx::bridge(namespace = "rocksdb")]
pub mod ffi {
    enum CpuPriority {
        kIdle = 0,
        kLow = 1,
        kNormal = 2,
        kHigh = 3,
    }
}
