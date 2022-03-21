cmake_minimum_required(VERSION 3.21)
project(bic)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_BUILD_TYPE Debug)

include_directories(".")

FILE(GLOB SOURCES src/*.c)

add_executable(bic ${SOURCES})
