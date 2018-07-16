//
// Created by huyu on 2018/5/30.
//

#include <unistd.h>
#include <sys/stat.h>
#include "syslog.h"

jboolean WRITE_LOGCAT = 0;
jboolean WRITE_LOGFILE = 0;

static void (*log_callback)(float ret);

void syslog_callback(void (*callback)(float ret)) {
    log_callback = callback;
}

static void returnProgress(char *ret) {
    float result = 0;
    char timeStr[10] = "time=";
    char *tstart = strstr(ret, timeStr);
    if (tstart) {
        //日志信息中若包含"time="字符串
        char str[17] = {0};
        strncpy(str, tstart, 16);
        int h = (str[5] - '0') * 10 + (str[6] - '0');
        int m = (str[8] - '0') * 10 + (str[9] - '0');
        int s = (str[11] - '0') * 10 + (str[12] - '0');
        float ms = (str[14] - '0') * 0.1f + (str[15] - '0') * 0.01f;
        result = ms + s + m * 60 + h * 60 * 60;
    } else {
        return;
    }
    //已执行时长 result, 这里 可以回调到java
    if(log_callback)
        log_callback(result);
}

/**
 * 输出
 */
static void syslog_print(void *ptr, int level, const char *fmt, va_list vl) {

    int ret = 0;
    static int print_prefix = 1;
    static char prev[1024];
    char line[1024];
    av_log_format_line(ptr, level, fmt, vl, line, sizeof(line), &print_prefix);
    strcpy(prev, line);

    // 这里可以对 line 进行 字符串匹配 如： time= 来截取 进度
    returnProgress(line);


    if (WRITE_LOGCAT) {
        //优先输出到控制台
        switch (level) {
            case AV_LOG_VERBOSE:
                LOGV("%s", line);
                break;
            case AV_LOG_INFO:
                LOGI("%s", line);
                break;
            case AV_LOG_DEBUG:
                LOGD("%s", line);
                break;
            case AV_LOG_WARNING:
                LOGW("%s", line);
                break;
            case AV_LOG_ERROR:
            default:
                LOGV("%s", line);
                break;
        }
    } else if (WRITE_LOGFILE) {
        //输出到文件
        if(access(LOG_FILE_DIR, F_OK)){
            if(mkdir(LOG_FILE_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
                //文件夹创建失败
                return;
            }
        }

        char path[strlen(LOG_FILE_DIR) + 15];
        ret = sprintf(path, "%s/%s", LOG_FILE_DIR, "ffmpeg_log.txt");
        if(ret == -1)
            //字符串拷贝失败
            return;
        FILE *fp = fopen(path, "a+");
        if (fp) {
            vfprintf(fp, fmt, vl);
            fflush(fp);
            fclose(fp);
        }
    }
}


void syslog_init() {

    av_log_set_level(AV_LOG_VERBOSE);
    av_log_set_callback(syslog_print);

}


