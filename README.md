# Migrating RocksDB to Rust

## C++ Development

You can build your own docker image and use that as a development environment.

Build static library:

```bash
docker build -t rocksdb --target=build .
```

Build and run basic tests:

```bash
docker build -t rocksdb --target=test .
```

Build and run all tests:

```bash
docker build -t rocksdb --target=test-all .
```

## Dev tips

Referencing a shared type from rust:

```rust
extern "C++" {
    include!("rocksdb-rs/src/transaction_log.rs.h");

    #[namespace = "rocksdb_rs::transaction_log"]
    type WalFileType = crate::transaction_log::ffi::WalFileType;
}
```

Referencing a c++ type defined in a different bridge:

```rust
#[namespace = "rocksdb"]
unsafe extern "C++" {
    include!("rocksdb/slice.h");

    type Slice = crate::slice::ffi::Slice;
}
```
