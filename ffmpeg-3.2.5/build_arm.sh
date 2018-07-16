#!/bin/bash
# 需要自己创建
export TMPDIR=$(pwd)/../out/cache
# NDK的路径，根据自己的安装位置进行设置
NDK=/home/huyu/huyu/android-ndk-r14b
# 编译针对的平台，可以根据自己的需求进行设置
# 这里选择最低支持android-14, arm架构，若针对x86架构，要选择arch-x86
PLATFORM=$NDK/platforms/android-14/arch-arm
# 工具链的路径，根据编译的平台不同而不同
# arm-linux-androideabi-4.9与上面设置的PLATFORM对应，4.9为工具的版本号，
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64

CPU=arm

EXT_INCLUDE=$(pwd)/../out/include
EXT_LIB=$(pwd)/../out/lib

FF_EXTRA_CFLAGS="-O3 -fpic -fasm -Wno-psabi -fno-short-enums -fno-strict-aliasing -finline-limit=300 -mfloat-abi=softfp -mfpu=vfp -marm -march=armv6 "
FF_CFLAGS="-O3 -Wall -pipe \
-ffast-math \
-fstrict-aliasing -Werror=strict-aliasing \
-Wno-psabi -Wa,--noexecstack \
-DANDROID  "
PREFIX=$(pwd)/../out/ffmpeg

#编译了32位so文件后再编译64位so文件时，提示strtod.o的文件格式不对。这是由于对于3.2版本的ffmpeg，make clean不会删除compat下的strtod.o，strtod.d, msvcrt/snprintf.o, msvcrt/snprintf.d四个文件，只要手动删除后重新编译即可。具体可以参考简书上esonyf的这篇文章：http://www.jianshu.com/p/612ef67e42bd
rm -f compat/strtod.o compat/strtod.d  compat/msvcrt/snprintf.o compat/msvcrt/snprintf.d

build_one(){
./configure \
--prefix=$PREFIX \
--enable-cross-compile \
--disable-runtime-cpudetect \
--disable-asm \
--arch=$CPU \
--target-os=android \
--cc=$TOOLCHAIN/bin/arm-linux-androideabi-gcc \
--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
--disable-stripping \
--nm=$TOOLCHAIN/bin/arm-linux-androideabi-nm \
--sysroot=$PLATFORM \
--extra-cflags="-I$EXT_INCLUDE " \
--extra-ldflags="-L$EXT_LIB" \
--disable-vda \
--disable-iconv \
--enable-neon \
--enable-yasm \
--enable-gpl \
--enable-nonfree \
--enable-version3 \
--enable-pthreads \
--enable-small \
--disable-static \
--enable-shared \
--disable-encoders \
--enable-libx264 \
--enable-libfdk-aac \
--enable-encoder=libx264 \
--enable-encoder=libfdk_aac \
--enable-encoder=mjpeg \
--enable-encoder=png \
--disable-decoders \
--enable-decoder=aac \
--enable-decoder=aac_latm \
--enable-decoder=h264 \
--enable-decoder=mpeg4 \
--enable-decoder=mjpeg \
--enable-decoder=png \
--disable-muxers \
--enable-muxer=mov \
--enable-muxer=mp4 \
--enable-muxer=h264 \
--enable-muxer=avi \
--disable-demuxers \
--enable-demuxer=image2 \
--enable-demuxer=h264 \
--enable-demuxer=aac \
--enable-demuxer=avi \
--enable-demuxer=mpc \
--enable-demuxer=mov \
--enable-demuxer=mpegts \
--disable-parsers \
--enable-parser=aac \
--enable-parser=ac3 \
--enable-parser=h264 \
--enable-zlib \
--disable-indevs \
--disable-outdevs \
--disable-ffprobe \
--disable-ffplay \
--disable-ffmpeg \
--disable-ffserver \
--disable-symver \
--disable-stripping \
--disable-debug \
--disable-postproc \
--disable-avdevice \
--extra-cflags="$FF_EXTRA_CFLAGS  $FF_CFLAGS" \
--extra-ldflags="  "
}
build_one


make clean
make -j4
make install

echo FFmpeg ARM build finished

cp $EXT_LIB/libfdk-aac.so $PREFIX/lib


