cmake_minimum_required(VERSION 3.22)
project(look_header_file C)

set(CMAKE_C_STANDARD 99)
include_directories(include)

add_executable(look_header_file main.c)
