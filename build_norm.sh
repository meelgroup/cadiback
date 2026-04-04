#!/bin/bash
set -e
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -Dcadical_DIR=../cadical/build \
  -DCMAKE_INSTALL_PREFIX="$(pwd)/build"
cmake --build build -j$(nproc)
cmake --install build
