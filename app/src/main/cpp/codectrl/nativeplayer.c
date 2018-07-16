//
// Created by huyu on 2018/5/3.
//

#include "codectrl.h"
#include "../syslog/syslog.h"

enum STATUS {
    PLAYING, PAUSED, STOPPED
};


struct PlayVideoParam {
    JNIEnv *env;
    jobject surface;
    const char *in_filepath;
};


/*初始状态*/
static enum STATUS status = STOPPED;

pthread_mutex_t mutex;
pthread_cond_t cond;


/**
 * 回收资源
 */
static int recycleLock() {
    int error = 0;
    error = pthread_cond_destroy(&cond);
    if (error) {
        LOGE("回收资源，释放条件变量失败 error = %d", error);
        return -1;
    }
    error = pthread_mutex_destroy(&mutex);
    if (error) {
        LOGE("回收资源，释放锁失败 error = %d", error);
        return -1;
    }
    return error;
}


static void *play(void *arg) {

    prctl(PR_SET_NAME, "PlayVideoThread"); // 设置该线程名字

    int error = 0;

    if (status != STOPPED) {
        LOGE("已经处于播放就绪状态，直接退出");
        return (void *) -1;
    }

    struct PlayVideoParam *param = NULL;
    param = (struct PlayVideoParam *) arg;

    JNIEnv *env = param->env;
    jobject surface = param->surface;
    const char *in_filepath = param->in_filepath;

    //1.调用此方法，注册所有的编解码器
    av_register_all();
    avformat_network_init();

    //2.获的一个AVFormatContext封装格式上下文，保存视频文件封装格式的相关信息
    AVFormatContext *pFormatCtx = avformat_alloc_context();

    LOGI("文件路径 %s", in_filepath);

    //3.打开需要解码的音/视频文件用来获取相关信息，给 pFormatCtx 赋值
    if (avformat_open_input(&pFormatCtx, in_filepath, NULL, NULL) != 0) {
        LOGE("打开音/视频文件:%s失败\n", in_filepath);
        error = -2; // Couldn't open file
        goto end1;
    }

    //4.读取音/视频流的相关信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("读取音/视频流的相关信息失败");
        error = -3;
        goto end1;
    }

    //5.找到视频流,因为里面的流还有可能是音频流或者其他的，我们摄像头只关心视频流
    int videoStream = -1, i;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO &&
            videoStream < 0) {
            videoStream = i;
        }
    }
    if (videoStream == -1) {
        LOGE("没找到视频流");
        error = -4;
        goto end1;
    }

    // 这里打印音视频流信息
    LOGI("视频流帧率：%d fps\n", pFormatCtx->streams[videoStream]->r_frame_rate.num /
                           pFormatCtx->streams[videoStream]->r_frame_rate.den);
    int iTotalSeconds = (int) pFormatCtx->duration / 1000000;
    int iHour = iTotalSeconds / 3600;//小时
    int iMinute = iTotalSeconds % 3600 / 60;//分钟
    int iSecond = iTotalSeconds % 60;//秒
    LOGI("持续时间：%02d:%02d:%02d\n", iHour, iMinute, iSecond);
    LOGI("视频时长：%lld微秒\n", pFormatCtx->streams[videoStream]->duration);
    LOGI("持续时间：%lld微秒\n", pFormatCtx->duration);

    //6.获得编解码器上下文 AVCodecContext，里面有流数据的信息
    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    //7.根据编解码参数找到编解码器 AVCodec ， 软解
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        LOGE("没找到软解码器\n");
        error = -5;
        goto end1;
    }

    //9.打开音/视频文件用来解码
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("打开音/视频编解码器失败");
        error = -7; // Could not open codec
        goto end1;
    }

    //10.分配一个 AVFrame （一帧编码之前或者解码之后的像素数据）
    AVFrame *pFrame = av_frame_alloc();
    // 用于渲染
    AVFrame *pFrameRGBA = av_frame_alloc();

    if (pFrameRGBA == NULL || pFrame == NULL) {
        LOGE("Could not allocate video frame.");
        error = -1;
        goto end1;
    }

    // buffer中数据就是用于渲染的,且格式为RGBA
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height, 1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecCtx->width, pCodecCtx->height, 1);

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
                                                pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width,
                                                pCodecCtx->height,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);
    /*******底层播放********/
    // 获取native window
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    // 获取视频宽高
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;
/*
    float radio_video = 1.0f * videoWidth / videoHeight;
    float radio_view = 1.0f * viewWidth / viewHeight;
    LOGE("缩放前：\nvideoWidth = %d, videoHeight = %d, radio_video = %f\nviewWidth = %d, viewHeight = %d, radio_view = %f",
         videoWidth, videoHeight, radio_video, viewWidth, viewHeight, radio_view);

    if(radio_view >= radio_video) {
        videoHeight = viewHeight;
        videoWidth = (int) (videoHeight * radio_video);
    } else {
        videoWidth = viewWidth;
        videoHeight = (int) (videoWidth / radio_video);
    }
    LOGE("缩放后：\nvideoWidth = %d, videoHeight = %d", videoWidth, videoHeight);
*/
    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;
    /*******底层播放********/

    LOGI("----开始播放----");
    int frameFinished;
    AVPacket packet;
    status = PLAYING;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    //11.读取一个 AVPacket 存储一帧压缩的编码数据,包含编码之后的码流数据
    while (av_read_frame(pFormatCtx, &packet) >= 0) {

        //加锁
        pthread_mutex_lock(&mutex);

        if (status == PAUSED) {
            // 挂起
            LOGI("----暂停播放----");
            pthread_cond_wait(&cond, &mutex);
        } else if (status == STOPPED) {
            LOGI("----停止播放----");
            pthread_mutex_unlock(&mutex);
            goto end0;
        }

        // 判断读取的是不是视频流
        if (packet.stream_index == videoStream) {

            // 开始解码
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // 并不是decode一次就可解码出一帧
            if (frameFinished) {

                // lock native window buffer
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);

                // 格式转换
                sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);

                // 获取stride
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t *src = (pFrameRGBA->data[0]);
                int srcStride = pFrameRGBA->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制
                int h;
                for (h = 0; h < videoHeight; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
            }
        }
        av_packet_unref(&packet);

        //释放锁
        pthread_mutex_unlock(&mutex);
    }
    LOGI("----播放结束----");

    end0:
    ANativeWindow_release(nativeWindow); //释放 NativeWindow

    av_free(buffer);
    av_free(pFrameRGBA);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);

    recycleLock(); //回收锁资源

    end1:
    free(param); // 释放线程参数结构体指针

    status = STOPPED;
    return (void *) error;
}


/**
 * 播放已封装好的 视频文件 如 mp4, avi等
 * @param env
 * @param surface
 * @param in_filepath
 * @return
 */
int playVideo(JNIEnv *env, jobject surface, const char *in_filepath) {
    struct PlayVideoParam *param = malloc(sizeof(struct PlayVideoParam));
    param->env = env;
    param->surface = surface;
    param->in_filepath = in_filepath;


//    pthread_t videoThread;
//    int error = pthread_create(&videoThread, NULL, (void *(*)(void *)) play, param);

    int *error = play(param);

    return (int) error;
}



/**
 * 暂停播放
 */
int pauseVideo() {
    if (status == PLAYING) {
        int error = pthread_mutex_lock(&mutex);
        if (error) {
            LOGE("暂停播放，获取锁失败 error = %d", error);
            return -1;
        }
        status = PAUSED;
        error = pthread_mutex_unlock(&mutex);
        if (error) {
            LOGE("暂停播放，释放锁失败 error = %d", error);
            return -1;
        }
    }
    return 0;
}



/**
 * 恢复播放
 */
int resumePlayVideo() {
    if (status == PAUSED) {
        int error = pthread_mutex_lock(&mutex);
        if (error) {
            LOGE("恢复播放，获取锁失败 error = %d", error);
            return -1;
        }
        status = PLAYING;
        error = pthread_cond_signal(&cond);
        if (error) {
            LOGE("恢复播放，通知恢复失败 error = %d", error);
            return -1;
        }
        error = pthread_mutex_unlock(&mutex);
        if (error) {
            LOGE("恢复播放，释放锁失败 error = %d", error);
            return -1;
        }
    }
    return 0;
}



/**
 * 停止播放
 */
int stopPlayVideo() {
    if (status != STOPPED) {
        int error = pthread_mutex_lock(&mutex);
        if (error) {
            LOGE("停止播放，获取锁失败 error = %d", error);
            return -1;
        }

        if (status == PAUSED) {
            status = STOPPED;
            error = pthread_cond_signal(&cond);
            if (error) {
                LOGE("停止播放，通知停止失败 error = %d", error);
                return -1;
            }
        } else {
            status = STOPPED;
        }
        error = pthread_mutex_unlock(&mutex);
        if (error) {
            LOGE("停止播放，释放锁失败 error = %d", error);
            return -1;
        }
    }
}



/**
 * 获取当前播放状态
 */
int getPlayStatus() {
    return status;
}



/**
 * 添加滤镜，播放
 * @param env
 * @param clazz
 * @param surface
 * @param pInputPath
 * @param filters_descr
 * @return
 */
int playFilterVideo(JNIEnv *env, jobject surface, const char *pInputPath,
                    const char *filters_descr) {

    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;

    av_register_all();

    avfilter_register_all();//for AVfilter

    AVFormatContext *pFormatCtx = avformat_alloc_context();

    // Open video file
    if (avformat_open_input(&pFormatCtx, pInputPath, NULL, NULL) != 0) {

        LOGE("Couldn't open file:%s\n", pInputPath);
        return -1; // Couldn't open file
    }

    // Retrieve stream information
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Couldn't find stream information.");
        return -1;
    }

    // Find the first video stream
    int videoStream = -1, i;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO
            && videoStream < 0) {
            videoStream = i;
        }
    }
    if (videoStream == -1) {
        LOGE("Didn't find a video stream.");
        return -1; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    //added by ws for AVfilter start----------init AVfilter--------------------------ws


    char args[512];
    int ret;
    AVFilter *buffersrc = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");//新版的ffmpeg库必须为buffersink
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};
    AVBufferSinkParams *buffersink_params;

    filter_graph = avfilter_graph_alloc();

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
             pCodecCtx->time_base.num, pCodecCtx->time_base.den,
             pCodecCtx->sample_aspect_ratio.num, pCodecCtx->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOGE("Cannot create buffer source\n");
        return ret;
    }

    /* buffer video sink: to terminate the filter chain. */
    buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, buffersink_params, filter_graph);
    av_free(buffersink_params);
    if (ret < 0) {
        LOGE("Cannot create buffer sink\n");
        return ret;
    }

    /* Endpoints for the filter graph. */
    outputs->name = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = NULL;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = NULL;

    // avfilter_link(buffersrc_ctx, 0, buffersink_ctx, 0);

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, &outputs, NULL)) < 0) {
        LOGE("Cannot avfilter_graph_parse_ptr\n");
        return ret;
    }

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
        LOGE("Cannot avfilter_graph_config\n");
        return ret;
    }

    //added by ws for AVfilter start------------init AVfilter------------------------------ws

    // Find the decoder for the video stream
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        LOGE("Codec not found.");
        return -1; // Codec not found
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec.");
        return -1; // Could not open codec
    }

    // 获取native window
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    // 获取视频宽高
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;

    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec.");
        return -1; // Could not open codec
    }

    // Allocate video frame
    AVFrame *pFrame = av_frame_alloc();

    // 用于渲染
    AVFrame *pFrameRGBA = av_frame_alloc();
    if (pFrameRGBA == NULL || pFrame == NULL) {
        LOGE("Could not allocate video frame.");
        return -1;
    }

    // Determine required buffer size and allocate buffer
    // buffer中数据就是用于渲染的,且格式为RGBA
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height,
                                            1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecCtx->width, pCodecCtx->height, 1);

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
                                                pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width,
                                                pCodecCtx->height,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

    int frameFinished;
    AVPacket packet;
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        // Is this a packet from the video stream?
        if (packet.stream_index == videoStream) {

            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);



            // 并不是decode一次就可解码出一帧
            if (frameFinished) {

                //added by ws for AVfilter start
                pFrame->pts = av_frame_get_best_effort_timestamp(pFrame);

                //* push the decoded frame into the filtergraph
                if (av_buffersrc_add_frame(buffersrc_ctx, pFrame) < 0) {
                    LOGE("Could not av_buffersrc_add_frame");
                    break;
                }

                ret = av_buffersink_get_frame(buffersink_ctx, pFrame);
                if (ret < 0) {
                    LOGE("Could not av_buffersink_get_frame");
                    break;
                }
                //added by ws for AVfilter end

                // lock native window buffer
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);

                // 格式转换
                sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);

                // 获取stride
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t *src = (pFrameRGBA->data[0]);
                int srcStride = pFrameRGBA->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制
                int h;
                for (h = 0; h < videoHeight; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
            }

        }
        av_packet_unref(&packet);
    }

    ANativeWindow_release(nativeWindow); //释放 NativeWindow
    av_free(buffer);
    av_free(pFrameRGBA);

    // Free the YUV frame
    av_free(pFrame);

    avfilter_graph_free(&filter_graph); //added by ws for avfilter
    // Close the codecs
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);
    return 0;
}



/**
 * 整合音屏文件和视频文件
 * @param pVideoPath    视频文件路径
 * @param pMusicPath    音频文件路径
 * @param pOutPath      是输出文件路径 "/storage/emulated/0/wsAddMusic.flv";//Output file URL
 * @return
 */
int addBGM(const char *pVideoPath, const char *pMusicPath, const char *pOutPath) {

    AVOutputFormat *ofmt = NULL;
    //Input AVFormatContext and Output AVFormatContext
    AVFormatContext *ifmt_ctx_v = NULL, *ifmt_ctx_a = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;
    int ret, i;
    int videoindex_v = -1, videoindex_out = -1;
    int audioindex_a = -1, audioindex_out = -1;
    int frame_index = 0;
    int64_t cur_pts_v = 0, cur_pts_a = 0;


    av_register_all();
    //Input
    if ((ret = avformat_open_input(&ifmt_ctx_v, pVideoPath, 0, 0)) < 0) {//打开输入的视频文件
        LOGE("Could not open input file.");
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx_v, 0)) < 0) {//获取视频文件信息
        LOGE("Failed to retrieve input stream information");
        goto end;
    }

    if ((ret = avformat_open_input(&ifmt_ctx_a, pMusicPath, 0, 0)) < 0) {//打开输入的音频文件
        LOGE("Could not open input file.");
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx_a, 0)) < 0) {//获取音频文件信息
        LOGE("Failed to retrieve input stream information");
        goto end;
    }

//    LOGE("===========Input Information==========\n");
//    av_dump_format(ifmt_ctx_v, 0, pVideoPath, 0);
//    av_dump_format(ifmt_ctx_a, 0, pMusicPath, 0);
//    LOGE("======================================\n");

    //Output
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, pOutPath);//初始化输出码流的AVFormatContext。
    if (!ofmt_ctx) {
        LOGE("Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        return -1;
    }
    ofmt = ofmt_ctx->oformat;

    //从输入的AVStream中获取一个输出的out_stream
    for (i = 0; i < ifmt_ctx_v->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        if (ifmt_ctx_v->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVStream *in_stream = ifmt_ctx_v->streams[i];
            AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);//创建流通道AVStream
            videoindex_v = i;
            if (!out_stream) {
                LOGE("Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                break;
            }
            videoindex_out = out_stream->index;
            //Copy the settings of AVCodecContext
            if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
                LOGE("Failed to copy context from input to output stream codec context\n");
                break;
            }
            out_stream->codec->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
            break;
        }
    }

    for (i = 0; i < ifmt_ctx_a->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        if (ifmt_ctx_a->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            AVStream *in_stream = ifmt_ctx_a->streams[i];
            AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
            audioindex_a = i;
            if (!out_stream) {
                LOGE("Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                goto end;
            }
            audioindex_out = out_stream->index;
            //Copy the settings of AVCodecContext
            if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
                LOGE("Failed to copy context from input to output stream codec context\n");
                goto end;
            }
            out_stream->codec->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

            break;
        }
    }

//    LOGE("==========Output Information==========\n");
//    av_dump_format(ofmt_ctx, 0, pOutPath, 1);
//    LOGE("======================================\n");
    //Open output file
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        if (avio_open(&ofmt_ctx->pb, pOutPath, AVIO_FLAG_WRITE) < 0) {//打开输出文件。
            LOGE("Could not open output file '%s'", pOutPath);
            return -1;
        }
    }
    //Write file header
    if (avformat_write_header(ofmt_ctx, NULL) < 0) {
        LOGE("Error occurred when opening output file\n");
        return -1;
    }


    //FIX
#if USE_H264BSF
    AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
#endif
#if USE_AACBSF
    AVBitStreamFilterContext* aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");
#endif

    while (1) {
        AVFormatContext *ifmt_ctx;
        int stream_index = 0;
        AVStream *in_stream, *out_stream;

        //Get an AVPacket .   av_compare_ts是比较时间戳用的。通过该函数可以决定该写入视频还是音频。
        if (av_compare_ts(cur_pts_v, ifmt_ctx_v->streams[videoindex_v]->time_base, cur_pts_a,
                          ifmt_ctx_a->streams[audioindex_a]->time_base) <= 0) {
            ifmt_ctx = ifmt_ctx_v;
            stream_index = videoindex_out;

            if (av_read_frame(ifmt_ctx, &pkt) >= 0) {
                do {
                    in_stream = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if (pkt.stream_index == videoindex_v) {
                        //FIX：No PTS (Example: Raw H.264) H.264裸流没有PTS，因此必须手动写入PTS
                        //Simple Write PTS
                        if (pkt.pts == AV_NOPTS_VALUE) {
                            //Write PTS
                            AVRational time_base1 = in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration =
                                    (double) AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts = (double) (frame_index * calc_duration) /
                                      (double) (av_q2d(time_base1) * AV_TIME_BASE);
                            pkt.dts = pkt.pts;
                            pkt.duration = (double) calc_duration /
                                           (double) (av_q2d(time_base1) * AV_TIME_BASE);
                            frame_index++;
                        }

                        cur_pts_v = pkt.pts;
                        break;
                    }
                } while (av_read_frame(ifmt_ctx, &pkt) >= 0);
            } else {
                break;
            }
        } else {
            ifmt_ctx = ifmt_ctx_a;
            stream_index = audioindex_out;
            if (av_read_frame(ifmt_ctx, &pkt) >= 0) {
                do {
                    in_stream = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if (pkt.stream_index == audioindex_a) {

                        //FIX：No PTS
                        //Simple Write PTS
                        if (pkt.pts == AV_NOPTS_VALUE) {
                            //Write PTS
                            AVRational time_base1 = in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration =
                                    (double) AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts = (double) (frame_index * calc_duration) /
                                      (double) (av_q2d(time_base1) * AV_TIME_BASE);
                            pkt.dts = pkt.pts;
                            pkt.duration = (double) calc_duration /
                                           (double) (av_q2d(time_base1) * AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_a = pkt.pts;

                        break;
                    }
                } while (av_read_frame(ifmt_ctx, &pkt) >= 0);
            } else {
                break;
            }

        }

        //FIX:Bitstream Filter
#if USE_H264BSF
        av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
#if USE_AACBSF
        av_bitstream_filter_filter(aacbsfc, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif


        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index = stream_index;

//        LOGE("Write 1 Packet. size:%5d\tpts:%lld\n", pkt.size, pkt.pts);
        //Write AVPacket 音频或视频裸流
        if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
            LOGE("Error muxing packet\n");
            break;
        }
        av_free_packet(&pkt);

    }
    //Write file trailer
    av_write_trailer(ofmt_ctx);

#if USE_H264BSF
    av_bitstream_filter_close(h264bsfc);
#endif
#if USE_AACBSF
    av_bitstream_filter_close(aacbsfc);
#endif

    end:
    avformat_close_input(&ifmt_ctx_v);
    avformat_close_input(&ifmt_ctx_a);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        LOGE("Error occurred.\n");
        return -1;
    }
    return 0;
}






