fn main() {
    let include_path = std::env::var("DEP_ROCKSDB_CXXBRIDGE_DIR0").unwrap();
    println!("cargo:rustc-env=ROCKSDB_GENERATED_INCLUDE={}", include_path);
}
