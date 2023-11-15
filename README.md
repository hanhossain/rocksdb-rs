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
Regex to find enums which need to be convered to enum class: `(?<!(static )|\w)enum\s+(?!class)(?!.*=)`

Use this to include the bridging header.
```c++
#ifndef ROCKSDB_RS
#include "rocksdb-rs-cxx/lib.h"
#else
#include "rocksdb-rs/src/lib.rs.h"
#endif
```
