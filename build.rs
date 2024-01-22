use build_common::{get_clang_flags, get_files, BUILD_VERSION_FILE};

fn main() {
    // This will be set when building rocksdb-rs from cmake.
    let skip_build_script = std::env::var("SKIP_BUILD_SCRIPT").map_or(false, |x| x == "1");

    let bridges = vec![
        "src/cache.rs",
        "src/compression_type.rs",
        "src/env.rs",
        "src/filename.rs",
        "src/hash.rs",
        "src/lib.rs",
        "src/port_defs.rs",
        "src/slice.rs",
        "src/status.rs",
        "src/unique_id.rs",
    ];

    if !skip_build_script {
        let target = std::env::var("TARGET").unwrap();
        let includes = ["rocksdb-cxx/include", "rocksdb-cxx"];
        let mut config = cxx_build::bridges(&bridges);

        for flag in get_clang_flags() {
            config.flag(flag);
        }

        // Let c++ know it's being built from rust.
        config.define("ROCKSDB_RS", None);

        if target.contains("darwin") {
            config.define("OS_MACOSX", None);
        } else if target.contains("linux") {
            config.define("OS_LINUX", None);
        } else {
            panic!("Unsupported target: {}", target);
        }

        config.define("ROCKSDB_PLATFORM_POSIX", None);
        config.define("ROCKSDB_LIB_IO_POSIX", None);

        config.includes(&includes);

        config.files(get_files());
        config.compile("rocksdb-cxx");
    }

    println!("cargo:rerun-if-changed=rocksdb-cxx");
    println!("cargo:rerun-if-changed={}", BUILD_VERSION_FILE);

    for bridge in bridges {
        println!("cargo:rerun-if-changed={bridge}");
    }
}
