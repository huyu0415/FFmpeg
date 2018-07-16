#!/bin/bash

#  https://github.com/libass/libass/releases

PREFIX=$(pwd)/../out
NDK=/home/huyu/huyu/android-ndk-r14b
# ARM 
TOOLCHAIN=$NDK/toolchains/huyu/my-android-toolchain-arm/bin/arm-linux-androideabi-

PKG_CONFIG=/usr/bin/pkg-config
PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig:/usr/local/lib/pkgconfig:/usr/lib/x86_64-linux-gnu/pkgconfig:$PKG_CONFIG_PATH
CFLAGS="-O3 -DANDROID -march=armv5te -I${PREFIX}/include"
LDFLAGS="-Wl,-dynamic-linker=/system/bin/linker -L${PREFIX}/lib"

#  不知怎么生成 android 可用的 so,需要改 configure 配置,但是不知怎么改，所以改用静态库链接

#./autogen.sh
function build_one
{
./configure \
CC="${TOOLCHAIN}gcc" \
PKG_CONFIG=$PKG_CONFIG \
PKG_CONFIG_PATH=$PKG_CONFIG_PATH \
--prefix=$PREFIX \
--disable-dependency-tracking \
--with-pic \
--disable-asm \
--disable-harfbuzz \
--enable-fontconfig \
--enable-static \
--disable-shared \
--host=arm-linux-android

make clean
make -j4
make install

}
build_one

