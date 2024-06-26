FROM rust:latest

RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    libgflags-dev \
    libclang-dev \
    clang \
    lld

RUN cargo install eza

ENV CC=/usr/bin/clang
ENV CXX=/usr/bin/clang++
