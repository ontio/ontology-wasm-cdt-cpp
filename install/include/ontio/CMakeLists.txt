file(GLOB HEADERS "*.hpp"
                  "*.h")

add_library(ontio
            ontiolib.cpp
            crypto.cpp
            ${HEADERS})

add_native_library(native_ontio
                   ontiolib.cpp
                   crypto.cpp
                   ${HEADERS})

target_include_directories(ontio PUBLIC
                                 ${CMAKE_SOURCE_DIR}/libc/musl/include
                                 ${CMAKE_SOURCE_DIR}/libc/musl/src/internal
                                 ${CMAKE_SOURCE_DIR}/libc/musl/src/crypt
                                 ${CMAKE_SOURCE_DIR}/libc/musl/arch/eos
                                 {CMAKE_SOURCE_DIR}/libcxx/include
                                 ${CMAKE_SOURCE_DIR}
                                 ${CMAKE_SOURCE_DIR}/boost/include)

target_include_directories(native_ontio PUBLIC
                                 ${CMAKE_SOURCE_DIR}/libc/musl/include
                                 ${CMAKE_SOURCE_DIR}/libc/musl/src/internal
                                 ${CMAKE_SOURCE_DIR}/libc/musl/src/crypt
                                 ${CMAKE_SOURCE_DIR}/libc/musl/arch/eos
                                 ${CMAKE_SOURCE_DIR}/libcxx/include
                                 ${CMAKE_SOURCE_DIR}
                                 ${CMAKE_SOURCE_DIR}/boost/include)

target_link_libraries( ontio c c++ )
target_link_libraries( native_ontio native_c native_c++ )
add_dependencies( native_ontio ontio )


add_custom_command( TARGET ontio POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ontio> ${BASE_BINARY_DIR}/lib )
add_custom_command( TARGET native_ontio POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:native_ontio> ${BASE_BINARY_DIR}/lib )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../ontiolib DESTINATION ${BASE_BINARY_DIR}/include FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
