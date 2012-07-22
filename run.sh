#!/bin/sh
rm /tmp/crappycase.log
rm crappycase.dylib
gcc -arch x86_64 -arch i386 killerlib.c -dynamiclib -o crappycase.dylib
gcc -g testcase.m -framework Foundation -o testcase

export DYLD_INSERT_LIBRARIES=crappycase.dylib
export DYLD_FORCE_FLAT_NAMESPACE=
./testcase

unset DYLD_INSERT_LIBRARIES
unset DYLD_FORCE_FLAT_NAMESPACE
cat /tmp/crappycase.log

