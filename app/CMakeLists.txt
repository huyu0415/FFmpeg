cmake_minimum_required(VERSION 3.4.1)

# 宏定义 so 文件目录
set(SO_SRC_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI})

add_library( ffmpeg_jni

             SHARED

             src/main/cpp/syslog/syslog.c
             src/main/cpp/thread/ffmpeg_thread.c

             src/main/cpp/jni/ffmpeg_code_jni.c
             src/main/cpp/jni/ffmpeg_cmd_jni.c

             src/main/cpp/cmdctrl/cmdutils.c
             src/main/cpp/cmdctrl/ffmpeg.c
             src/main/cpp/cmdctrl/ffmpeg_filter.c
             src/main/cpp/cmdctrl/ffmpeg_opt.c

             src/main/cpp/codectrl/encodeutil.c
             src/main/cpp/codectrl/liveplayer.c
             src/main/cpp/codectrl/nativeplayer.c
             )


find_library( log-lib
              log )


#添加libavcodec-57.so
add_library( avcodec SHARED IMPORTED)
set_target_properties( avcodec PROPERTIES IMPORTED_LOCATION ${SO_SRC_DIR}/libavcodec.so)

add_library( avfilter SHARED IMPORTED)
set_target_properties( avfilter PROPERTIES IMPORTED_LOCATION ${SO_SRC_DIR}/libavfilter.so)

add_library( avformat SHARED IMPORTED)
set_target_properties( avformat PROPERTIES IMPORTED_LOCATION ${SO_SRC_DIR}/libavformat.so)

add_library( avutil SHARED IMPORTED)
set_target_properties( avutil PROPERTIES IMPORTED_LOCATION ${SO_SRC_DIR}/libavutil.so)

add_library( swresample SHARED IMPORTED)
set_target_properties( swresample PROPERTIES IMPORTED_LOCATION ${SO_SRC_DIR}/libswresample.so)

add_library( swscale SHARED IMPORTED)
set_target_properties( swscale PROPERTIES IMPORTED_LOCATION ${SO_SRC_DIR}/libswscale.so)

add_library( postproc SHARED IMPORTED)
set_target_properties( postproc PROPERTIES IMPORTED_LOCATION ${SO_SRC_DIR}/libpostproc.so)

#add_library( avdevice SHARED IMPORTED)
#set_target_properties( avdevice PROPERTIES IMPORTED_LOCATION ${SO_SRC_DIR}/libavdevice.so)

#add_library( fdk-aac SHARED IMPORTED)
#set_target_properties( fdk-aac PROPERTIES IMPORTED_LOCATION ${SO_SRC_DIR}/libfdk-aac.so)

#add_library( mp3lame SHARED IMPORTED)
#set_target_properties( mp3lame PROPERTIES IMPORTED_LOCATION ${SO_SRC_DIR}/libmp3lame.so.0)

#导入FFmpeg的头文件
include_directories(${CMAKE_SOURCE_DIR}/../ffmpeg-3.2.5)


# 4.关联库，将上面步骤3 找到的 so 或者源码，关联到 我们自己的so, 以便我们在自己写的源码中引用
target_link_libraries( # Specifies the target library.
                       ffmpeg_jni                             # 引用了其它so的目标文件

                       android                                # 因为引用了 native_window 等相关文件
                       # fdk-aac                              # 被引用的so, 可以有多个，如果第三方库同样在代码中需要引用，则需要像 ffmpeg 的动态库一样 import
                       # mp3lame
                       # avdevice
                       postproc
                       avcodec
                       avfilter
                       avformat
                       avutil
                       swresample
                       swscale
                       ${log-lib} )