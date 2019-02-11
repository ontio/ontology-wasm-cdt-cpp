#!/usr/bin/env bash

ont_c -nostdinc -I/home/cl/github/ont.dtcC/install/include/libc -L/home/cl/github/ont.dtcC/install/lib -lc -lmalloc -static test_memset.c
cp a.out /home/cl/go/src/github.com/go-interpreter/wagon/cmd/wasm-run/
