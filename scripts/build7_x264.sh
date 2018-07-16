#!/bin/bash

#   http://download.videolan.org/pub/videolan/x264/snapshots/

PREFIX=$(pwd)/../out
NDK=/home/huyu/huyu/android-ndk-r14b
# ARM 
TOOLCHAIN=$NDK/toolchains/huyu/my-android-toolchain-arm/bin/arm-linux-androideabi-

# 生成 android 可用so, 将最后一行"SONAME=libx264.so.$API"这里改为"SONAME=libx264.so"

#x86   --host=i686-linux-android
function build_one
{
./configure \
--prefix=$PREFIX \
--enable-static \
--enable-pic \
--disable-asm \
--disable-cli \
--host=arm-linux-android \
--cross-prefix=$TOOLCHAIN \
--extra-cflags="-O3 -DANDROID -march=armv5te"  \
--extra-ldflags="-Wl,--no-undefined -Wl,-dynamic-linker=/system/bin/linker -lc -lm -lgcc"

make clean
make -j4
make install

}
build_one

echo X264 ARM build finished
