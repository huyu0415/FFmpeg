#!/bin/bash

#  https://sourceforge.net/projects/lame/files/lame/

PREFIX=$(pwd)/../out
NDK=/home/huyu/huyu/android-ndk-r14b
# ARM
TOOLCHAIN=$NDK/toolchains/huyu/my-android-toolchain-arm/bin/arm-linux-androideabi-

PKG_CONFIG=/usr/bin/pkg-config
PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig:/usr/local/lib/pkgconfig:/usr/lib/x86_64-linux-gnu/pkgconfig:$PKG_CONFIG_PATH
CFLAGS="-O3 -DANDROID -march=armv5te"
LDFLAGS="-Wl,-dynamic-linker=/system/bin/linker"


function build_one
{
./configure \
CC="${TOOLCHAIN}gcc" \
--prefix=$PREFIX \
--with-pic \
--enable-static \
--disable-shared \
--host=arm-linux-android

make clean
make -j4
make install

}
build_one


