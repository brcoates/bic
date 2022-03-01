cmake_minimum_required(VERSION 3.21)
project(pic C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

include_directories(".")

FILE(GLOB SOURCES src/*.c)
add_executable(bic ${SOURCES})
