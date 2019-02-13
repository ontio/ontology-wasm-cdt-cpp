cmake_minimum_required(VERSION 3.5)
project(ontology_libc++)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-everything --target=wasm32 -nostdlib -fno-builtin -ffreestanding  -fno-threadsafe-statics      -O3" -v)

SET(SRC_FILENAMES algorithm.cpp any.cpp bind.cpp condition_variable.cpp functional.cpp
                  future.cpp ios.cpp iostream.cpp locale.cpp memory.cpp mutex.cpp new.cpp optional.cpp
                  regex.cpp stdexcept.cpp string.cpp strstream.cpp system_error.cpp exception.cpp
                  typeinfo.cpp utility.cpp valarray.cpp variant.cpp vector.cpp eosio.cpp)

SET(SRC_FILES "")
FOREACH(FN ${SRC_FILENAMES})
   LIST(APPEND SRC_FILES "libcxx/src/${FN}")
ENDFOREACH(FN)

add_library(c++
            ${SRC_FILES})

target_include_directories(c++
                           PUBLIC 
                           ${CMAKE_CURRENT_SOURCE_DIR}/libcxx/include
                           ${CMAKE_SOURCE_DIR}/../libc/musl/src/internal
                           ${CMAKE_SOURCE_DIR}/../libc/musl/src/crypt
                           ${CMAKE_SOURCE_DIR}/../libc/musl/arch/ont
                           ${CMAKE_SOURCE_DIR}/../libc/musl/include)

target_link_libraries(c++ c)
