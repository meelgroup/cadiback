#!/usr/bin/env bash
rm -rf libcadi.*
rm -f compile_commands.json
make clean
CXXFLAGS=-fPIC ./configure
bear -- make -j12
