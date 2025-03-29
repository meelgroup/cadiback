#!/bin/bash
export CXX=/usr/lib/emscripten/em++
export CC=/usr/lib/emscripten/emcc
export AR=/usr/lib/emscripten/emar
export CXXFLAGS=-fPIC
./configure
make -f makefile.emscripten
