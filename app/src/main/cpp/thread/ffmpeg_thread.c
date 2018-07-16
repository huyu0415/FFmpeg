//
// Created by huyu on 2018/5/30.
//

#include "ffmpeg_thread.h"

pthread_t ntid;
char **argvs = NULL;
int num = 0;


void *thread(void *arg) {   //执行
    return (void *) run_cmd(num, argvs);
}

/**
 * 新建子线程执行ffmpeg命令
 */
int ffmpeg_thread_run_cmd(int cmdnum, char **argv) {
    num = cmdnum;
    argvs = argv;

    int temp = pthread_create(&ntid, NULL, thread, NULL);
    if (temp != 0) {
        LOGE("ffmpeg_thread: can't create thread: %s ",strerror(temp));
        return 1;
    }
    return 0;
}


int ffmpeg_thread_run(func funcArg, thread_args *args) {

    pthread_t tid;
    int temp = pthread_create(&tid, NULL, funcArg, args);
    if (temp != 0) {
        LOGE("ffmpeg_thread: can't create thread: %s ", strerror(temp));
        return 1;
    }
    return 0;
}

/**
 * 函数指针
 * @param ret
 */
static void (*ffmpeg_callback)(int ret);

/**
 * 注册线程回调
 */
void ffmpeg_thread_callback(void (*cb)(int ret)) {
    ffmpeg_callback = cb;
}

/**
 * 退出线程
 */
void ffmpeg_thread_exit(int ret) {
    if (ffmpeg_callback) {
        ffmpeg_callback(ret);
    }
    free(argvs);
    pthread_exit("ffmpeg_thread_exit");

}