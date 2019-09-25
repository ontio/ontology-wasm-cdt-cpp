cmake_minimum_required(VERSION 3.5)
project(ontoalloc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-everything --target=wasm32 -nostdlib -fno-builtin -ffreestanding  -fno-threadsafe-statics  -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wno-everything --target=wasm32 -nostdlib -fno-builtin -ffreestanding  -fno-threadsafe-statics  -O3  -fno-rtti -fno-exceptions")

SET(SRC_FILENAMES malloc.cpp)

add_library(malloc ${SRC_FILENAMES})
INSTALL(TARGETS malloc
		ARCHIVE DESTINATION lib)
