#!/bin/bash
CXXFLAGS=-fPIC ./configure
make -f makefile.emscripten
