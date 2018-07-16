//
// Created by huyu on 2018/4/29.
//
//  #include< >  是定位系统或编译器自带的库
//  #include" "  是先定位项目下的路径，找不到再去系统库查找
//
#ifndef CODECTRL_MYHEAD_H
#define CODECTRL_MYHEAD_H

#define TRUE 1
#define FALSE 0


#include <jni.h>

#ifdef __ANDROID_API__
#include <android/native_window.h>
#include <android/native_window_jni.h>

#endif

#include <pthread.h>
#include <sys/prctl.h>

#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/pixdesc.h"
#include "libavutil/time.h"
#include "libavutil/opt.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/avfiltergraph.h"
#include "libavformat/avformat.h"

struct thread_func_args {
    char *descript;
    const char *filepath1;
    const char *filepath2;
};

typedef struct thread_func_args thread_args;

void *decode2Yuv420(void *args);

int encodeYuv2H264(const char *yuvpath, const char *h264parh);

int transformFMT(const char *srcpath, const char *dedtpath);

int decodeYuv2Rgb(const char *yuvpath, const char *rgbpath);

int encodePCM(const char *in_filepath, const char *out_file);

int addFilters(char *filter_descr, const char *srcyuvpath, const char *destyuvpath);

int playVideo(JNIEnv *env, jobject surface, const char *in_filepath);

int pauseVideo();

int resumePlayVideo();

int stopPlayVideo();

int getPlayStatus();

int playFilterVideo(JNIEnv *env, jobject surface, const char *pInputPath, const char *filters_descr);

int addBGM(const char *pVideoPath, const char *pMusicPath, const char *pOutPath);

#endif //CODECTRL_MYHEAD_H
