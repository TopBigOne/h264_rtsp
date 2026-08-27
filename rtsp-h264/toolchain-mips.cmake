# CMake 工具链文件:告诉 CMake 用君正 MIPS 交叉编译器,而不是本机 gcc/g++。
# 用法: cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=toolchain-mips.cmake ...
# (同 study_w_1/rtsp-h264 项目里的同名文件,详细说明见那边的
#  DOC/toolchain-mips_cmake文件的作用.md)

# 声明目标平台是 Linux/MIPS,CMake 据此进入交叉编译模式。
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR mips)

set(TOOLCHAIN_ROOT /home/dev/Documents/Hai_si_work/hai_si/mips_gcc720_glibc229_r5_1_4)

# 本项目的 .c 文件实际依赖 C++ 语法(如 sample-common.c 里的花括号初始化赋值,
# 标准 C 不支持),必须用 g++ 编译。CMake 会拒绝把 C++ 编译器设成
# CMAKE_C_COMPILER,所以这里只设置 CXX 编译器;.c 文件改在 CMakeLists.txt 里
# 用 set_source_files_properties(... PROPERTIES LANGUAGE CXX) 强制按 CXX 编译。
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_ROOT}/bin/mips-linux-gnu-g++)

# 交叉编译时限制 CMake 的 find_* 命令的搜索范围,避免误用本机 x86 的库/头文件。
set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_ROOT})
# 找可执行程序(构建过程中用到的、跑在本机的工具)不受限制,仍可以在本机路径下找。
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# 找库文件、头文件、CMake 包时只在 CMAKE_FIND_ROOT_PATH(交叉工具链目录)下找。
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
