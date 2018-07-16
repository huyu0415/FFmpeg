//
// Created by huyu on 2018/5/30.
//

#ifndef FFMPEG_CMD_FFMPEG_THREAD_H
#define FFMPEG_CMD_FFMPEG_THREAD_H

#include <pthread.h>
#include "../cmdctrl/ffmpeg.h"
#include "../syslog/syslog.h"
#include "../codectrl/codectrl.h"

typedef void* (func)(void *);

int ffmpeg_thread_run_cmd(int cmdnum,char **argv);

int ffmpeg_thread_run(func funcArg, thread_args *args);

void ffmpeg_thread_exit(int ret);

void ffmpeg_thread_callback(void (*cb)(int ret));

#endif //FFMPEG_CMD_FFMPEG_THREAD_H
