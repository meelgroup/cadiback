#!/bin/bash
rm -f libcadi*
emmake make -f makefile.emscripten
cp libcadi* $EMINSTALL/lib/
