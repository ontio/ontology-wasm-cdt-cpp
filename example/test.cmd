#!/usr/bin/env bash

ont_c -I/home/cl/github/ont.dtcC/install/include/libc -L/home/cl/github/ont.dtcC/install/lib -lc -lmalloc test_memset.c
cp a.out /home/cl/go/src/github.com/go-interpreter/wagon/cmd/wasm-run/
