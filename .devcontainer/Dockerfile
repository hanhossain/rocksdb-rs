FROM rust:latest

RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    libgflags-dev \
    libclang-dev \
    clang \
    lld

RUN cargo install eza
RUN cargo install cxxbridge-cmd@1.0.118

ENV LDFLAGS="-fuse-ld=lld"
ENV CC=/usr/bin/clang
ENV CXX=/usr/bin/clang++
