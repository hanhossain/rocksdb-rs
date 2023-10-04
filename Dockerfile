FROM rust:latest AS base

RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    libgflags-dev

FROM base AS dev

WORKDIR /src
COPY . .
