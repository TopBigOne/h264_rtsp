cmake_minimum_required(VERSION 3.10)
project(rtsp-h264 CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(MIPS_FLAGS "-std=c++11 -mabi=32 -mhard-float -mfp64 -mnan=2008 -mabs=2008 -Wall -EL -march=mips32r2 -flax-vector-conversions -w -fpermissive")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MIPS_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(SDK_ROOT ${CMAKE_SOURCE_DIR}/..)
set(NNA_ROOT ${SDK_ROOT}/magik-toolkit/InferenceKit/nna2/mips720-glibc229/T41)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${SDK_ROOT}/include
    ${NNA_ROOT}/include
)

link_directories(
    ${NNA_ROOT}/lib/glibc
)

set(C_SOURCES
    main.c
    ringfifo.c
    rtputils.c
    rtspservice.c
    rtsputils.c
    sample-common.c
    sample-Encoder-video.c
)

# These are .c files but rely on C++ syntax (e.g. brace-init struct assignment
# in sample-common.c) that only compiles under g++, not plain C. Force them
# through the CXX compiler instead of CMake's (unused) C toolchain.
set_source_files_properties(${C_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(rtsp-h264
    ${C_SOURCES}
    inference_nv12.cpp
)

target_link_libraries(rtsp-h264
    ${SDK_ROOT}/lib/glibc/libimp.a
    ${SDK_ROOT}/lib/glibc/libalog.a
    venus
    aip
    drivers
    pthread
    rt
    dl
    m
)
