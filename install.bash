#!/usr/bin/env bash
# cmake make automake libbz2-dev libssl-dev libgmp3-dev autotools-dev build-essential libicu-dev autoconf libtool curl zlib1g-dev doxygen graphviz

PROJECT_DIR="."
INSTALL_DIR="~/usr/llvm/"
VERSION="Release"
ARCH="WebAssembly"
BUILD_DIR="$PROJECT_DIR/build/"

mkdir -p $BUILD_DIR
mkdir -p $INSTALL_DIR

cd $BUILD_DIR
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR  -DCMAKE_BUILD_TYPE=$VERSION -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=$ARCH ../ontdtcC_llvm

echo "configure done. press Y/N:"
read GoOn

make -j4
make install
