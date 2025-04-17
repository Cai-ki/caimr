cmake_minimum_required(VERSION 3.20)

project(caimr)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SHARED_LINKER_FLAGS "-static-libgcc -static-libstdc++")

set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

include_directories(${src_dir})

file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)

add_library(caimr SHARED ${SRC_FILES})

target_include_directories(caimr
    PUBLIC
    ${INCLUDE_DIR}
)

set_target_properties(${lib_name} PROPERTIES
    OUTPUT_NAME "caimr"
    LINKER_LANGUAGE CXX
)