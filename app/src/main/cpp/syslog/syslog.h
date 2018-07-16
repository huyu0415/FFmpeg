//
// Created by huyu on 2018/5/29.
//

#ifndef FFMPEG4ANDROID_MASTER_SYSLOG_H
#define FFMPEG4ANDROID_MASTER_SYSLOG_H

#include <libavutil/log.h>
#include <jni.h>

#if defined __cplusplus
extern "C"
{
#endif // __cplusplus

#define SYS_LOG_TAG "ffmpeg"
#define LOG_FILE_DIR "/storage/emulated/0/huyu"

jboolean WRITE_LOGCAT;
jboolean WRITE_LOGFILE;
void syslog_init();
void syslog_callback(void (*callback)(float ret));


#ifdef __ANDROID_API__
#include <android/log.h>

#define LOGV(format, ...)   __android_log_print(ANDROID_LOG_VERBOSE, SYS_LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)   __android_log_print(ANDROID_LOG_INFO, SYS_LOG_TAG, format, ##__VA_ARGS__)
#define LOGD(format, ...)   __android_log_print(ANDROID_LOG_DEBUG, SYS_LOG_TAG, format, ##__VA_ARGS__)
#define LOGW(format, ...)   __android_log_print(ANDROID_LOG_WARN, SYS_LOG_TAG, format, ##__VA_ARGS__)
#define LOGE(format, ...)   __android_log_print(ANDROID_LOG_ERROR, SYS_LOG_TAG, format, ##__VA_ARGS__)

/*
#else
#define LOGV(format, ...)   printf(SYS_LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)   printf(SYS_LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGD(format, ...)   printf(SYS_LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGW(format, ...)   printf(SYS_LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGE(format, ...)   printf(SYS_LOG_TAG format "\n", ##__VA_ARGS__)
*/
#endif // __ANDROID_API__


#if defined __cplusplus
}
#endif // __cplusplus

#endif //FFMPEG4ANDROID_MASTER_SYSLOG_H
