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
