FROM rust:latest AS base

RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    libgflags-dev

FROM base AS dev
WORKDIR /src

COPY . .

FROM dev AS build-test
WORKDIR /src/build


RUN cmake -G Ninja -DWITH_ALL_TESTS=OFF ../rocksdb-cxx
RUN ninja

FROM build-test AS test
WORKDIR /src/build

RUN ctest -j $(nproc)

FROM dev AS build-test-all
WORKDIR /src/build

RUN cmake -G Ninja -DWITH_ALL_TESTS=ON ../rocksdb-cxx
RUN ninja

FROM build-test-all AS test-all
WORKDIR /src/build

RUN ctest -j $(nproc)

FROM dev AS build
WORKDIR /src/build

RUN cmake -G Ninja -DWITH_TESTS=OFF ../rocksdb-cxx
RUN ninja rocksdb
