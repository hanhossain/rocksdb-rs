#[cxx::bridge(namespace = "rocksdb_rs::port_defs")]
pub mod ffi {
    enum CpuPriority {
        kIdle = 0,
        kLow = 1,
        kNormal = 2,
        kHigh = 3,
    }
}
