# FFmpeg
<<<<<<< HEAD
android端基于FFmpeg库的使用。<br>
基于ffmpeg3.2.5版本，可编译生成libffmpeg.so 或者 7个单独的so(可裁剪) 文件。<br>
添加编译ffmpeg源码的参考脚本 scripts <br>
目前音视频相关处理：<br>

- #### 命令行和代码共存控制
- #### 简单的jni回调java

后续会完善音视频播放、推流直播。

## 说明：  
1. 参照网上教程，拷贝 so 库文件后，无需拷贝 include 文件夹，而是拷贝编译后的 ffmpeg-3.2.5 源码文件夹；
在 cmake.txt 文件中包含该源码文件即可。
2. 常用编译命令，参考 FFmpeg 命令说明.md
3. 常用编译命令，参考 FFmpeg 编译说明.md

<br><br>
