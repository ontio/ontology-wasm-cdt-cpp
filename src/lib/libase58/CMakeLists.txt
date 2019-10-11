cmake_minimum_required(VERSION 3.5)
project(ontoalloc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-everything --target=wasm32 -nostdlib -fno-builtin -ffreestanding  -fno-threadsafe-statics  -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wno-everything --target=wasm32 -nostdlib -fno-builtin -ffreestanding  -fno-threadsafe-statics  -O3  -fno-rtti -fno-exceptions")

SET(SRC_FILENAMES base58.c)

add_library(base58 ${SRC_FILENAMES})

target_include_directories(base58
                           PUBLIC 
                           ${CMAKE_CURRENT_SOURCE_DIR}/../libc/musl/arch/onto
                           ${CMAKE_CURRENT_SOURCE_DIR}/../libc/musl/include)

INSTALL(TARGETS base58
		ARCHIVE DESTINATION lib)
