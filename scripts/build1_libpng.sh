#!/bin/bash

#   https://sourceforge.net/projects/libpng-apng/files/libpng16/

PREFIX=$(pwd)/../out
NDK=/home/huyu/huyu/android-ndk-r14b
# ARM
TOOLCHAIN=$NDK/toolchains/huyu/my-android-toolchain-arm/bin/arm-linux-androideabi-

CFLAGS="-O3 -DANDROID -march=armv5te"
LDFLAGS="-Wl,-dynamic-linker=/system/bin/linker"

# arm 为 no, armeabi-v7a-neon 为 yes
ARM_NEON="no"
#armeabi-v7a-neon
#ARM_NEON="yes"


function build_one
{
./configure \
CC="${TOOLCHAIN}gcc" \
--prefix=$PREFIX \
--with-pic \
--host=arm-linux-android \
--enable-static \
--disable-shared \
--enable-arm-neon="$ARM_NEON"

make clean
make -j4
make install

}
build_one


