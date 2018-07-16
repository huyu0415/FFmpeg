#!/bin/bash
# 需要自己创建
PREFIX=$(pwd)/../out
export TMPDIR=$(pwd)/../out/cache
NDK=/home/huyu/huyu/android-ndk-r14b
TOOLCHAIN=$NDK/toolchains/huyu/my-android-toolchain-arm/bin/arm-linux-androideabi-

export PKG_CONFIG=/usr/bin/pkg-config
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig:/usr/local/lib/pkgconfig:/usr/lib/x86_64-linux-gnu/pkgconfig:$PKG_CONFIG_PATH
export PKG_CONFIG_LIBDIR="${PREFIX}/lib/pkgconfig"

ARCH=arm

EXT_INCLUDE=$(pwd)/../out/include
EXT_LIB=$(pwd)/../out/lib

FF_EXTRA_CFLAGS="-O3 -fpic -fasm -Wno-psabi -fno-short-enums -fno-strict-aliasing -finline-limit=300 -mfloat-abi=softfp -mfpu=vfp -marm -march=armv6 "
FF_CFLAGS="-O3 -Wall -pipe \
-ffast-math \
-fstrict-aliasing -Werror=strict-aliasing \
-Wno-psabi -Wa,--noexecstack \
-DANDROID  "
FF_LDFLAGS="-Wl,-dynamic-linker=/system/bin/linker -lc -lm -llog"

#编译了32位so文件后再编译64位so文件时，提示strtod.o的文件格式不对。这是由于对于3.2版本的ffmpeg，make clean不会删除compat下的strtod.o，strtod.d, msvcrt/snprintf.o, msvcrt/snprintf.d四个文件，只要手动删除后重新编译即可。具体可以参考简书上esonyf的这篇文章：http://www.jianshu.com/p/612ef67e42bd
rm -f compat/strtod.o compat/strtod.d  compat/msvcrt/snprintf.o compat/msvcrt/snprintf.d

#--enable-avresample \  重采样库，需要时开启
#--enable-libmp3lame \
#--enable-libfribidi \
#--enable-libfreetype \
#--enable-libfontconfig \
#--enable-libass \
#--enable-libx264 \
#--enable-encoder=libx264 \
#--enable-libfdk-aac \
#--enable-encoder=libfdk_aac \
#--extra-libs="-lm -lgcc -lpng -lexpat" \

build_one(){
./configure \
--prefix=$PREFIX/ffmpeg \
--enable-cross-compile \
--cross-prefix=$TOOLCHAIN \
--target-os=android \
--host-os=arm-linux-androideabi \
--arch=$ARCH \
--cc=${TOOLCHAIN}gcc \
--nm=${TOOLCHAIN}nm \
--pkg-config=${PKG_CONFIG} \
--disable-gray \
--disable-htmlpages \
--disable-manpages \
--disable-podpages \
--disable-txtpages \
--disable-d3d11va \
--disable-dxva2 \
--disable-vaapi \
--disable-vda \
--disable-iconv \
--disable-vdpau \
--disable-hwaccels \
--disable-stripping \
--disable-videotoolbox \
--disable-swscale-alpha \
--disable-runtime-cpudetect \
--disable-programs \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-ffserver \
--disable-symver \
--disable-debug \
--disable-doc \
--disable-decoders \
--disable-encoders \
--disable-muxers \
--disable-demuxers \
--disable-bsfs \
--disable-parsers \
--disable-protocols \
--disable-filters \
--disable-indevs \
--disable-outdevs \
--disable-avdevice \
--disable-postproc \
--disable-asm \
--disable-static \
--enable-shared \
--enable-gpl \
--enable-pic \
--enable-version3 \
--enable-nonfree \
--enable-hardcoded-tables \
--enable-neon \
--enable-yasm \
--enable-pthreads \
--enable-small \
--enable-postproc \
--enable-avformat \
--enable-avcodec \
--enable-avutil \
--enable-swresample \
--enable-swscale \
--enable-avfilter \
--enable-encoder=rawvideo \
--enable-encoder=flv \
--enable-encoder=mpeg4 \
--enable-encoder=mjpeg \
--enable-encoder=png \
--enable-decoder=aac \
--enable-decoder=aac_latm \
--enable-decoder=h264 \
--enable-decoder=hevc \
--enable-decoder=rawvideo \
--enable-decoder=flv \
--enable-decoder=mpeg4 \
--enable-decoder=mjpeg \
--enable-decoder=png \
--enable-muxer=mov \
--enable-muxer=flv \
--enable-muxer=mp4 \
--enable-muxer=avi \
--enable-muxer=h264 \
--enable-muxer=hevc \
--enable-demuxer=image2 \
--enable-demuxer=h264 \
--enable-demuxer=hevc \
--enable-demuxer=aac \
--enable-demuxer=avi \
--enable-demuxer=flv \
--enable-demuxer=mpegvideo \
--enable-demuxer=mpc \
--enable-demuxer=mov \
--enable-demuxer=mpegts \
--enable-parser=aac \
--enable-parser=ac3 \
--enable-parser=h264 \
--enable-parser=hevc \
--enable-protocol=file \
--enable-protocol=rtmp \
--enable-filters \
--enable-network \
--enable-zlib \
--enable-runtime-cpudetect \
--enable-libmp3lame \
--enable-libx264 \
--enable-encoder=libx264 \
--enable-libfdk-aac \
--enable-encoder=libfdk_aac \
--enable-decoder=libfdk_aac \
--enable-muxer=adts \
--enable-libfreetype \
--enable-libfribidi \
--enable-libfontconfig \
--enable-libass \
--extra-libs="-lm -lgcc -lfdk-aac -lexpat -lfribidi -lfreetype -lfontconfig -lass -lpng16 -lx264 -ldl" \
--extra-cflags="-I$EXT_INCLUDE $FF_EXTRA_CFLAGS $FF_CFLAGS" \
--extra-ldflags="-L$EXT_LIB $FF_LDFLAGS"
}
build_one

# -lx265 -lstdc++ 
#--enable-libx265 \
#--enable-encoder=libx265 \

make clean
make -j4
make install

echo FFmpeg ARM build finished

#静态链接时不需拷贝
#cp $EXT_LIB/libmp3lame.so $PREFIX/ffmpeg/lib
#cp $EXT_LIB/libfdk-aac.so $PREFIX/ffmpeg/lib
#cp $EXT_LIB/libass.so $PREFIX/ffmpeg/lib
#cp $EXT_LIB/llibfreetype.so $PREFIX/ffmpeg/lib
#cp $EXT_LIB/libfribidi.so $PREFIX/ffmpeg/lib
#cp $EXT_LIB/libfontconfig.so $PREFIX/ffmpeg/lib


