fn main() {
    let bridges = vec![
        "src/cache.rs",
        "src/coding.rs",
        "src/coding_lean.rs",
        "src/compression_type.rs",
        "src/ffi.rs",
        "src/filename.rs",
        "src/hash.rs",
        "src/io_status.rs",
        "src/options.rs",
        "src/port_defs.rs",
        "src/slice.rs",
        "src/status.rs",
        "src/transaction_log.rs",
        "src/types.rs",
        "src/unique_id.rs",
        "src/util/bloom.rs",
        "src/util/fastrange.rs",
        "src/utilities/options_type.rs",
    ];

    let includes = ["rocksdb-cxx/include", "rocksdb-cxx"];
    let mut config = cxx_build::bridges(&bridges);

    config
        .includes(&includes)
        .std("c++17")
        .warnings(false)
        .compile("bindings");

    if cfg!(feature = "build-cpp") {
        let dst = cmake::Config::new("rocksdb-cxx")
            .init_cxx_cfg(config.clone())
            .generator("Ninja")
            .define("WITH_TESTS", "NO")
            .define("FROM_CARGO", "ON")
            .build_target("rocksdb")
            .build();
        println!("cargo::rustc-link-search=native={}/build", dst.display());
        println!("cargo::rustc-link-lib=static=rocksdb");
    }

    println!("cargo::rerun-if-changed=rocksdb-cxx");
}
