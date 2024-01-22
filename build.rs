use build_common::{get_clang_defines, get_clang_flags, get_files, BUILD_VERSION_FILE};

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
        let includes = ["rocksdb-cxx/include", "rocksdb-cxx"];
        let mut config = cxx_build::bridges(&bridges);

        for flag in get_clang_flags() {
            config.flag(flag);
        }

        for def in get_clang_defines() {
            config.define(def, None);
        }

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
