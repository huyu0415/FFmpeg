#!/bin/bash

#   https://sourceforge.net/projects/expat/files/expat/

PREFIX=$(pwd)/../out
NDK=/home/huyu/huyu/android-ndk-r14b
# ARM
TOOLCHAIN=$NDK/toolchains/huyu/my-android-toolchain-arm/bin/arm-linux-androideabi-

#CFLAGS="-O3 -DANDROID -march=armv5te"
#LDFLAGS="-Wl,-dynamic-linker=/system/bin/linker"

#  不知怎么生成 android 可用的 so,需要改 configure 配置,但是不知怎么改，所以改用静态库链接

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


