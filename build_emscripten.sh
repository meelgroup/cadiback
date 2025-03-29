#!/bin/bash
rm -f libcadi*
make clean
./configure
emmake make -f makefile.emscripten
cp libcadi* $EMINSTALL/lib/
