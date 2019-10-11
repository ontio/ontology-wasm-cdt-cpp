#!/usr/bin/env bash
set -ev

rm -rf build 
mkdir -p build; cd build
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=../install ..
make install
