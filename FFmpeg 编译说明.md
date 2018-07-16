## CPU架构
    armeabi       第5代 ARM v5TE，使用软件浮点运算，兼容所有ARM设备，通用性强，速度慢
    armeabi-v7a   第7代 ARM v7，使用硬件浮点运算，具有高级扩展功能
    arm64-v8a     第8代，64位，包含AArch32、AArch64两个执行状态对应32、64bit
    x86           32位，一般用于平板
    x86_64        intel 64位，一般用于平板
    mips          少接触
    mips64        少接触


## 环境变量配置
    sudo gedit /etc/profile   打开环境变量配置文件，编辑环境变量

####Java
    export JAVA_HOME=/home/huyu/soft/jdk1.8
    export JRE_HOME={JAVA_HOME}/jre
    export CLASSPATH=.:{JAVA_HOME}/lib:${JRE_HOME}/lib
    export PATH=$JAVA_HOME/bin:$PATH

#### NDK
    export NDK_HOME=/home/huyu/soft/android/Sdk/ndk-bundle
    export PATH=$NDK_HOME:$PATH

### PKG_CONFIG_PATH
    export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig

## FFmpeg源码说明
* compact       目录下主要是头文件，是为了兼容不同平台或系统的数据类型定义，例如compat/msvcrt/snprintf.h和snprintf.c是兼容C99的snprintf()和vsnprintf()声明及实现
* libavcodec    包含音视频编码器和解码器； 
* libavfilter   包含多媒体处理常用的滤镜功能； 
* libavformat   包含多种多媒体容器格式的封装、解封装工具； 
* libavutil     是多媒体应用常用的简化编程的工具，像fifo管理，内存管理，时间管理，随机数生成器、数据结构、数学函数等功能等； 
* libswscale    是颜色空间转换和大小尺寸缩放的库；
* libavdevice   是音视频数据采集和渲染等功能的设备相关的接口实现，比如sdl显示设备，alsa音频输出设备，opengl图形加速设备，v4l2视频设备（比如摄像头）； 
* libswresample 是音频重采样，格式转换和混合；功能和libavresample十分类似，是FFmpeg相对早期的实现方法； 
* libavresample 是一个重采样库，比如音频采样率从44.1KHz到96KHz； 
* libpostproc   是后期处理库； 
* cmdutils.c	是用户使用命令行进行ffmpeg操作时用到的工具； 
* ffmpeg.c	    是ffmpeg工具的源码； 
* ffplay.c	    是一个基于FFmpeg库文件的播放器实例； 
* ffprobe.c	    是解析媒体文件格式信息的例子； 
* ffserver.c	是RTSP/HTTP流媒体服务器的源代码； 
* configure	    是编译前执行配置的脚本；

## FFmpeg的配置
[全平台最全脚本](https://github.com/mabeijianxi/FFmpeg4Android)

./configure --help
如果需要加gpl的库需要使用gpl的许可证，例如libx264就是gpl的，如果需要加入libx264则需要--enable-gpl。

####关于FFmpeg的裁剪
    PREFIX=$(pwd)/../out
    ADDI_CFLAGS="-I$PREFIX/include -DANDROID"
    ADDI_LDFLAGS="-L$PREFIX/lib -Wl,-dynamic-linker=/system/bin/linker -lc -lm -llog"
	
	./configure \
    --prefix=$PREFIX \
	--disable-static \
    --enable-shared \
    --cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
    --target-os=android \
    --arch=arm \
    --enable-cross-compile \
    --sysroot=$PLATFORM \
    --extra-cflags="-I$PLATFORM/usr/include -Os -fpic $ADDI_CFLAGS" \
	--extra-ldflags="$ADDI_LDFLAGS" \
	--disable-gpl \
    --disable-nonfree \
    --disable-gray \
    --disable-htmlpages \
    --disable-manpages \
    --disable-podpages \
    --disable-txtpages \
    --disable-d3d11va \
    --disable-dxva2 \
    --disable-vaapi \
    --disable-vda \
    --disable-vdpau \
    --disable-videotoolbox \
    --disable-swscale-alpha \
	        
    --disable-doc \
    --disable-ffmpeg \      不生成ffmpeg ffmpeg是一个命令行工具，非常强大的音视频转码、转换器,但我们通过代码开发，就不需要它了
    --disable-ffplay \      不生成简单的音视频播放器 这些都是不需要的工具
    --disable-ffprobe \     不生成简单的多媒体码流分析器 这些都是不需要的工具
    --disable-ffserver \    不生成流媒体服务器 这些都是不需要的工具
    --disable-avdevice \    这个库是用来操作一些硬件设备的，如摄像头这些
    --disable-doc \         不生成doc 文档
    --disable-symver \      禁用 symbol versioning
        
    //禁止组件，再打开需要的编解码，格式，和协议
    --disable-encoders \    禁用所有编码器　
    --disable-decoders \    禁用所有解码器　
    --disable-muxers \      禁用所有混合器（将音频和视频混合）
    --disable-demuxers \    禁用所有解混合器（将音频和视频分离）
    --disable-parsers \     禁用所有解析器
    --disable-bsfs \        禁用所有码流过率器
    --disable-protocols \   禁用所有协议
    --disable-indevs \      禁用所有输入设备
    --disable-outdevs \     禁用所有输出设备
    --disable-filters \     禁用所有过率器
            
    //ffmpeg中自带h264的解码，但是没有包含编码
    //打开需要支持的组件，支持MP4封装、支持H.264、H.265、AAC解码、支持RTMP协议的ffmpeg
    --enable-protocol=rtmp \
    --enable-protocol=file \
    --enable-encoder=rawvideo \
    --enable-muxer=rawvideo \
    --enable-decoder=h264 \
    --enable-decoder=hevc \
    --enable-decoder=aac \
    --enable-muxer=mp4  \
	--enable-libfreetype \  在 使用 avfilter 的 文字水印 drawtext 时必须添加这3项
	--enable-libfontconfig \
	--enable-libfribidi \
	--enable-libass \       在使用 avfilter 加字幕 时必须添加此项
    --extra-libs="-lm -lgcc -lfdk-aac -lexpat -lfribidi -lfreetype -lfontconfig -lass -lpng16 -lx264 -ldl" \

##链接库时注意：
1.  链接其它库的时候，需要在 --extra-libs="" 中加上该库，具体怎么写可以看各个库生成的.pc文件
2.  如果编译时静态链接第三方库，则只需复制 ffmpeg 生成的 so
3.  如果编译时动态链接第三方库，则需要在编译完成后，将第三方动态库一起复制到AS
4.  如果在AS里，代码需要使用第三方库，则需要像 ffmpeg 的动态库一样 import

<<<<<<< HEAD
##使用方法：
1.  下载 NDK14 或之前版本（之后的版本默认采用 Clang 编译，不使用 GCC ）

2.  下载 FFmpeg-3.2.5 以及各个需要的第三方库

3.  安装开发环境  

        sudo apt-get update
        sudo apt-get install -y python2.7 python2.7-dev build-essential libssl-dev libevent-dev libjpeg-dev libxml2-dev libxslt1-dev python-pip virtualenv pkg-config cmake zlib1g-dev uuid-dev libfreetype6-dev gperf yasm mercurial cmake cmake-curses-gui nasm

4.  构建单独编译工具链  
        
        cd $NDK/build/tools
        ./make-standalone-toolchain.sh --platform=android-14 --install-dir=$NDK/toolchains/android-toolchain/arm [ --arch=x86 ]
        --arch 选项选择目标程序的指令架构，默认是为 arm。
        --install-dir 默认会生成 /tmp/ndk/<toolchain-name>.tar.bz2。
        执行 make-standalone-toolchain.sh --help 查看帮助
=======
##--------编译问题汇总[(详细参见)](http://blog.sina.com.cn/s/blog_61bc01360102w815.html)-------

####1.libass编译
##### libass依赖库为 FreeType，FriBidi，Fontconfig  
[libass]([https://github.com/libass/libass/releases)  
[FreeType](https://www.freetype.org/download.html)  
[FriBidi](git clone https://github.com/fribidi/fribidi.git)  
[Fontconfig](https://www.freedesktop.org/software/fontconfig/release/)  


##### freetype　
    ./configure --prefix=/usr --disable-static && make && sudo make install
    
+ 如果报错　
        
        configure: error: The pkg-config script could not be found or is too old.
        
+ 执行　
        
        apt-get install -y pkg-config
        
+ 如果还是找不到，就不指定输出路径　
        
        ./configure --disable-static && make && sudo make install


##### fontconfig
    ./configure --prefix=/usr -disable-docs && make && sudo make install　
    
+ 如果报错 
        
        configure: error: Package requirements (uuid) were not met
        
+ 执行　
        
        sudo apt-get install uuid-dev
        
+ 如果报错 
        
        'gperf' is missing on your system
        
+ 执行　
        
        sudo apt-get install gperf
        
+ 如果还是找不到，就不指定输出路径  
        
        ./configure -disable-docs && make && sudo make install


##### fribidi　　依赖　expat 和 uuid
+ 下载　[expat](https://github.com/libexpat/libexpat/releases)
+ 执行　　

        ./configure && make && sudo make install
        
+ 安装　uuid
        
        sudo apt-get install uuid-dev
        
+ 再　cd 到　fribidi　执行
            
        ./configure --prefix=/usr --disable-docs && make && sudo make install
        
+ 如果还是找不到，就不指定输出路径　
        
        ./configure --disable-docs && make && sudo make install


##### libass  
    ./configure --prefix=/usr --disable-static && make && sudo make install
>>>>>>> parent of 2723c27... 更新命令
    
5.   编译时报错 configure: error: /bin/sh ./config.sub arm-linux-androideabi failed  
     需要更换需要编译的库目录下的 config.sub 和 config.guess  
     http://git.savannah.gnu.org/gitweb/?p=config.git;a=tree

6.  复制 scripts 目录下的脚本，修改各个脚本里的NDK路径，以及 build_share.sh 中的第三方库路径

7.  给各个脚本执行权限 sudo chmod 777 buildxxx.sh

8.  执行脚本 ./build_share.sh

9.  完成后会在上层目录下看到 out 的目录，里面的 include 和 lib 文件夹就是编译好的

10. 将编译后的 FFmpeg-3.2.5 源码文件拷贝到AS项目根目录 ffmpeg 下

11. 将 lib 下面的 so 文件复制到　src/main/jniLibs/armeabi　目录下

12. 如果使用命令行调用，需要将 源码里的 cmdutils.c  cmdutils.h  cmdutils_common_opts.h  ffmpeg.c  ffmpeg.h  ffmpeg_filter.c  ffmpeg_opt.c 复制到 cpp/cmdctrl 目录下

13. 修改如下几个位置
    > 1. cmdutils.c 修改 exit_program 函数为：
    >   ```c
    >        int exit_program(int ret)
    >        {
    >            if (program_exit)
    >                program_exit(ret);
    >            //TODO 3.执行命令后清除数据
    >            progress_avio = NULL;
    >
    >            //TODO 4.防止指令调用错误或完成导致程序终止,屏蔽 exit函数
    >            av_log(NULL, AV_LOG_FATAL,"退出code =  %d %s",ret, (ret == 0 ? "正常结束" : "错误结束"));
    >            ffmpeg_thread_exit(ret);
    >        //    exit(ret);
    >            return ret;
    >        }
    >   ```
    > 2. cmdutils.h   修改为：       
            int exit_program(int ret);
    > 3. ffmpeg.c 修改 main 函数 ：        
            函数名改为 run_cmd （可自定义）        
            首行添加日志打印
    > 4. ffmpeg.c 修改 ffmpeg_cleanup 函数 ：        
            释放指针，重置变量, 详见 TODO
    > 5. ffmpeg.h 末尾 ：   
            添加   run_cmd 函数 声明

14. 新建 ffmpeg工具类 FFmpegUtil.java  ， 加载 ffmpeg 的静态 so 文件 ，定义 native 函数， Ctrl + Enter ，生成 C 文件

15. 在 c 文件实现 native 函数
