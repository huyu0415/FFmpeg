#!/bin/bash

#   https://sourceforge.net/projects/opencore-amr/files/fdk-aac/

PREFIX=$(pwd)/../out
NDK=/home/huyu/huyu/android-ndk-r14b
#arm 
TOOLCHAIN=$NDK/toolchains/huyu/my-android-toolchain-arm/bin/arm-linux-androideabi-


function build_one
{
./configure \
CC="${TOOLCHAIN}gcc" \
CFLAGS="-O3 -DANDROID -march=armv5te" \
LDFLAGS="-Wl,-dynamic-linker=/system/bin/linker" \
CPP="${TOOLCHAIN}cpp" \
CXX="${TOOLCHAIN}g++" \
--enable-static=yes \
--enable-shared=no \
--prefix=$PREFIX \
--host=arm-linux-android

make clean
make -j4
make install

}
build_one

echo libfdk-aac ARM build finished
