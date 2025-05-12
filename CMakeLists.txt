cmake_minimum_required(VERSION 3.20)

project(caimr)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_SHARED_LINKER_FLAGS "-static-libgcc -static-libstdc++")

set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

set(HEADER_COPY_DIR "${CMAKE_BINARY_DIR}/${PROJECT_NAME}")

file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${INCLUDE_DIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    LINKER_LANGUAGE CXX
)

target_link_options(${PROJECT_NAME} PRIVATE "-static-libgcc" "-static-libstdc++")

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${INCLUDE_DIR}         
        ${HEADER_COPY_DIR}    
    COMMENT "Copying headers to build directory..."
)