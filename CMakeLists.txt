cmake_minimum_required(VERSION 3.20)

project(caimr)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SHARED_LINKER_FLAGS "-static-libgcc -static-libstdc++")

set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

set(BUILD_INCLUDE_DIR "${CMAKE_BINARY_DIR}/caimr")

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

add_custom_command(
    TARGET caimr
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${INCLUDE_DIR}         
        ${BUILD_INCLUDE_DIR}    
    COMMENT "Copying headers to build directory..."
)