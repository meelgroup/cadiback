#!/usr/bin/env bash
rm -rf build/libcadical.*
rm -f compile_commands.json
make clean
CXXFLAGS=-fPIC ./configure --debug
bear -- make -j12
