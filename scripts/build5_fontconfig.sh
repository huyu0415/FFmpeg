#!/bin/bash

#   https://www.freedesktop.org/software/fontconfig/release/

PREFIX=$(pwd)/../out
NDK=/home/huyu/huyu/android-ndk-r14b
# ARM 
TOOLCHAIN=$NDK/toolchains/huyu/my-android-toolchain-arm/bin/arm-linux-androideabi-

PKG_CONFIG=/usr/bin/pkg-config
PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig:/usr/local/lib/pkgconfig:/usr/lib/x86_64-linux-gnu/pkgconfig:$PKG_CONFIG_PATH
CFLAGS="-O3 -DANDROID -march=armv5te -I${PREFIX}/include"
LDFLAGS="-Wl,-dynamic-linker=/system/bin/linker -L${PREFIX}/lib"


#autoreconf -ivf
function build_one
{
./configure \
CC="${TOOLCHAIN}gcc" \
PKG_CONFIG=$PKG_CONFIG \
PKG_CONFIG_PATH=$PKG_CONFIG_PATH \
--prefix=$PREFIX \
--with-pic \
--host=arm-linux-android \
--enable-static \
--disable-shared \
--disable-libxml2 \
--disable-iconv \
--disable-docs


make clean
make -j4
make install

}
build_one

