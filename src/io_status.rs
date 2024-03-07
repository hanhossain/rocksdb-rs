#[cxx::bridge(namespace = "rocksdb_rs::io_status")]
mod ffi {
    enum IOErrorScope {
        kIOErrorScopeFileSystem,
        kIOErrorScopeFile,
        kIOErrorScopeRange,
        kIOErrorScopeMax,
    }
}
