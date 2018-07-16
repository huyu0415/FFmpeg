#!/bin/bash

#ibxxxx.so.major.minor，xxxx 是该lib的名称，major是主版本号，minor是副版本号
########  linux 会优先动态链接第三方库，没有so 文件时才去静态链接，编译第三方脚本时，如果后期需要替换第三方库，可以生成 .so，否则只生成 .a 文件即可,这里默认使用静态库 


#0.先安装开发环境
#sudo apt-get update
#sudo apt-get install -y python2.7 python2.7-dev build-essential libssl-dev libevent-dev libjpeg-dev libxml2-dev libxslt1-dev python-pip virtualenv pkg-config cmake zlib1g-dev uuid-dev libfreetype6-dev gperf yasm mercurial cmake cmake-curses-gui nasm

#1.先构建单独编译工具链
#cd 到 $NDK/build/tools
#./make-standalone-toolchain.sh --platform=android-14 --install-dir=$NDK/toolchains/android-toolchain/arm [ --arch=x86 ]
#--arch 选项选择目标程序的指令架构，默认是为 arm。
#--install-dir 默认会生成 /tmp/ndk/<toolchain-name>.tar.bz2。
#执行 make-standalone-toolchain.sh --help 查看帮助

#2.***  configure: error: /bin/sh ./config.sub arm-linux-androideabi failed  
# http://git.savannah.gnu.org/gitweb/?p=config.git;a=tree
# 需要更换需要编译的库目录下的 config.sub 和 config.guess  


# EXTRA_LIBS 具体怎么写可以看生成的.pc文件
EXTRA_LIBS="-lm -lgcc"
#SUPPORTED_LIBS=(expat lame libpng fribidi freetype fontconfig libass x264 x265 aac ffmpeg)
SUPPORTED_LIBS=(ffmpeg1)

for i in "${SUPPORTED_LIBS[@]}"
do
case $i in
lame)
	echo ------------------------------------------
	echo ____00_____start build lame_______________                                 	
	echo ------------------------------------------
	cp build2_lame.sh ../lame-3.100/build2_lame.sh
	cd ../lame-3.100
	./build2_lame.sh || exit 1
	rm -f build2_lame.sh
	cd ../script2
	sleep 2
;;
libpng)
	echo ------------------------------------------
	echo ____11_____start build libpng_____________                                 	
	echo ------------------------------------------
	cp build1_libpng.sh ../libpng-1.6.34/build1_libpng.sh
	cd ../libpng-1.6.34
	./build1_libpng.sh || exit 1
	rm -f build1_libpng.sh
	cd ../script2
	EXTRA_LIBS=${EXTRA_LIBS}" -lpng16"
	sleep 2
;;
expat)
	echo ------------------------------------------
	echo ____22_____start build expat______________                            	
	echo ------------------------------------------
	cp build0_expat.sh ../expat-2.1.0/build0_expat.sh
	cd ../expat-2.1.0
	./build0_expat.sh || exit 1
	rm -f build0_expat.sh
	cd ../script2
	EXTRA_LIBS=${EXTRA_LIBS}" -lexpat"
	sleep 2
;;
fribidi)
	echo ------------------------------------------
	echo ____33_____start build fribidi____________                            	
	echo ------------------------------------------
	cp build3_fribidi.sh ../fribidi-1.0.2/build3_fribidi.sh
	cd ../fribidi-1.0.2
	./build3_fribidi.sh || exit 1
	rm -f build3_fribidi.sh
	cd ../script2
	EXTRA_LIBS=${EXTRA_LIBS}" -lfribidi"
	sleep 2
;;
freetype)
	echo ------------------------------------------
	echo ____44_____start build freetype___________
	echo ------------------------------------------
	cp build4_freetype.sh ../freetype-2.6.5/build4_freetype.sh
	cd ../freetype-2.6.5
	./build4_freetype.sh || exit 1
	rm -f build4_freetype.sh
	cd ../script2
	EXTRA_LIBS=${EXTRA_LIBS}" -lfreetype"
	sleep 2
;;
fontconfig)
	echo ------------------------------------------
	echo ____55_____start build fontconfig_________                             	
	echo ------------------------------------------
	cp build5_fontconfig.sh ../fontconfig-2.12.6/build5_fontconfig.sh
	cd ../fontconfig-2.12.6
	./build5_fontconfig.sh || exit 1
	rm -f build5_fontconfig.sh
	cd ../script2
	EXTRA_LIBS=${EXTRA_LIBS}" -lfontconfig"
	sleep 2
;;
libass)
	echo ------------------------------------------
	echo ____66_____start build libass_____________                         	
	echo ------------------------------------------
	cp build6_libass.sh ../libass-0.13.0/build6_libass.sh
	cd ../libass-0.13.0
	./build6_libass.sh || exit 1
	rm -f build6_libass.sh
	cd ../script2
	EXTRA_LIBS=${EXTRA_LIBS}" -lass"
	sleep 2
;;
x264)
	echo ------------------------------------------
	echo ____77_____start build x264_______________                               	
	echo ------------------------------------------
	cp build7_x264.sh ../libx264-20180614-2245/build7_x264.sh
	cd ../libx264-20180614-2245
	./build7_x264.sh || exit 1
	rm -f build7_x264.sh
	cd ../script2
	EXTRA_LIBS=${EXTRA_LIBS}" -lx264 -ldl"
	sleep 2
;;
x265)
	echo ------------------------------------------
	echo ____77_____start build x265_______________                               	
	echo ------------------------------------------
	cp build7_x265.sh ../libx265_v2.6/build7_x265.sh
	cp build7_x265_crosscompile.cmake ../libx265_v2.6/build/arm-linux/build7_x265_crosscompile.cmake
	cd ../libx265_v2.6
	./build7_x265.sh || exit 1
	rm -f build7_x265.sh build7_x265_crosscompile.cmake
	cd ../script2
	EXTRA_LIBS=${EXTRA_LIBS}" -lx265 -lstdc++"
	sleep 2
;;
aac)
	echo ------------------------------------------
	echo ____88_____start build libfdk-aac_________                            
	echo ------------------------------------------
	cp build8_aac.sh ../libfdk-aac-0.1.6/build8_aac.sh
	cd ../libfdk-aac-0.1.6
	./build8_aac.sh || exit 1
	rm -f build8_aac.sh
	cd ../script2
	EXTRA_LIBS=${EXTRA_LIBS}" -lfdk-aac"
	sleep 2
;;
ffmpeg)
	echo ------------------------------------------
	echo ____99_____start build ffmpeg_____________                           
	echo ------------------------------------------
	cp build9_ffmpeg.sh ../ffmpeg-3.2.5/build9_ffmpeg.sh
	cd ../ffmpeg-3.2.5 || exit 1
	./build9_ffmpeg.sh $EXTRA_LIBS
	rm -f build9_ffmpeg.sh
	cd ../script2
;;
ffmpeg1)
	echo ------------------------------------------
	echo ____99_____start build ffmpeg_____________                           
	echo ------------------------------------------
	cp build9_ffmpeg_one.sh ../ffmpeg-3.2.5/build9_ffmpeg_one.sh
	cd ../ffmpeg-3.2.5 || exit 1
	./build9_ffmpeg_one.sh $EXTRA_LIBS
	rm -f build9_ffmpeg_one.sh
	cd ../script2
;;
esac
done

sleep 1

#cd ../out/ffmpeg && ls
