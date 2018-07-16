#!/bin/bash

#   http://download.videolan.org/pub/videolan/x265/

PREFIX=$(pwd)/../out
# 在 crosscompile.cmake 中更改交叉编译工具链
# 0.android不支持 log2() 和 posix_memalign() 函数
# 1.将 source/common/common.cpp 中 posix_memalign((void *)ptr, X265_ALIGNBYTES, size) 函数改为 void *ptr = memalign(X265_ALIGNBYTES, size);
# 2.将 source/common/common.h 中 log2(x) 替换为 (log((double)(x)) * 1.4426950408889640513713538072172)  log2f(x) 替换为 (logf((float)(x)) * 1.44269504088896405f)
# 3.将 source/CMakeLists.txt 中的 pthread 和 "-lpthread" 删除（共两处）android的ndk虽然有pthread.h,但是没有libpthread.a,集成到libc.a里了， 所以在编译x265时，不需要去链接-lpthread


#hg clone https://bitbucket.org/multicoreware/x265
cd build/arm-linux
cmake -DCMAKE_TOOLCHAIN_FILE=build7_x265_crosscompile.cmake -DCMAKE_INSTALL_PREFIX="$PREFIX" -DENABLE_SHARED:bool=off  -G "Unix Makefiles" ../../source
make -j4
make install


echo X265 ARM build finished
