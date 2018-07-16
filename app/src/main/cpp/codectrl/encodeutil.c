/*
FFMPEG中使用的几个主要数据结构的作用

    AVFormatContext  封装格式上下文，统领全局的结构体，保存了视频文件封装格式的相关信息
    AVInputFormat    每种封装格式（mp4, avi等）对应一个该结构体
    AVStream         视频文件中每个视频（音频）流对应一个该结构体
    AVCodecContext   AVCodec所代表的上下文信息，保存了视频（音频）AVCodec所需要的一些参数。对于实现编码功能，我们可以在这个结构中设置我们指定的编码参数
    AVCodec          AVCodec结构保存了一个编解码器的实例，实现实际的编码功能。每个视频（音频）编解码器（如H.264解码器）对应一个该结构体。
    AVPacket         码流包结构，包含编码之后的码流数据。存储一帧压缩编码数据，该结构可以不定义指针，以一个对象的形式定义。
    AVFrame          一帧编码之前或者解码之后的像素数据。其在程序中也是一个指针的形式。

    mp4是一种文件格式，而mpeg4是一个编码标准，二者不是一个意义上的概念。
    你可以这么理解，mp4是支持mpeg4的标准的音频视频文件，而支持mpeg4标准的文件格式有很多种，mp4和avi都是其中的一种

知识延伸：

1.I,P，B帧和PTS，DTS的关系
    1.1基本概念：
        I frame：帧内编码帧 又称 intra picture，I 帧通常是每个 GOP（MPEG 所使用的一种视频压缩技术）的第一个帧，经过适度地压缩，做为随机访问的参考点，可以当成图象。
                 I帧可以看成是一个图像经过压缩后的产物。
        P frame: 前向预测编码帧 又称predictive-frame，通过充分将低于图像序列中前面已编码帧的时间冗余信息来压缩传输数据量的编码图像，也叫预测帧；
        B frame: 双向预测内插编码帧 又称bi-directional interpolated prediction frame，既考虑与源图像序列前面已编码帧，
                 也顾及源图像序列后面已编码帧之间的时间冗余信息来压缩传输数据量的编码图像，也叫双向预测帧；
        PTS：Presentation Time Stamp。PTS主要用于度量解码后的视频帧什么时候被显示出来
        DTS：Decode Time Stamp。DTS主要是标识读入内存中的ｂｉｔ流在什么时候开始送入解码器中进行解码。
    在没有B帧存在的情况下DTS的顺序和PTS的顺序应该是一样的。

    1.2ＩＰＢ帧的不同：
        I frame:自身可以通过视频解压算法解压成一张单独的完整的图片。
        P frame：需要参考其前面的一个I frame 或者B frame来生成一张完整的图片。
        B frame:则要参考其前一个I或者P帧及其后面的一个P帧来生成一张完整的图片。
        两个I frame之间形成一个GOP，在x264中同时可以通过参数来设定bf的大小，即：I 和p或者两个P之间B的数量。
    通过上述基本可以说明如果有B frame 存在的情况下一个GOP的最后一个frame一定是P.

    1.3 DTS和PTS的不同：
        DTS主要用于视频的解码,在解码阶段使用.PTS主要用于视频的同步和输出.在display的时候使用.在没有B frame的情况下.DTS和PTS的输出顺序是一样的.


2.libswscale里面实现了各种图像像素格式的转换。
    2.1 libswscale使用起来很方便，最主要的函数只有3个：
        （1） sws_getContext()：使用参数初始化SwsContext结构体。
        （2） sws_scale()：转换一帧图像。
        （3） sws_freeContext()：释放SwsContext结构体。
    其中sws_getContext()也可以用另一个接口函数sws_getCachedContext()取代。

    2.2 初始化方法
        初始化SwsContext除了调用sws_getContext()之外还有另一种方法，更加灵活，可以配置更多的参数。该方法调用的函数如下所示。
        （1） sws_alloc_context()：为SwsContext结构体分配内存。
        （2） av_opt_set_XXX()：通过av_opt_set_int()，av_opt_set()…等等一系列方法设置SwsContext结构体的值。
            在这里需要注意，SwsContext结构体的定义看不到，所以不能对其中的成员变量直接进行赋值，必须通过av_opt_set()这类的API才能对其进行赋值。
        （3） sws_init_context()：初始化SwsContext结构体。
        这种复杂的方法可以配置一些sws_getContext()配置不了的参数。
        比如说设置图像的YUV像素的取值范围是JPEG标准（Y、U、V取值范围都是0-255）还是MPEG标准（Y取值范围是16-235，U、V的取值范围是16-240）。
        可以通过使用av_opt_set()设置“src_range”和“dst_range”来设置输入和输出的YUV的取值范围。
        如果“dst_range”字段设置为“1”的话，则代表输出的YUV的取值范围遵循“jpeg”标准；如果“dst_range”字段设置为“0”的话，则代表输出的YUV的取值范围遵循“mpeg”标准。

        通过av_pix_fmt_desc_get()可以获得指定像素格式的AVPixFmtDescriptor结构体。
        通过AVPixFmtDescriptor结构体可以获得不同像素格式的一些信息。
        例如av_get_bits_per_pixel()，通过该函数可以获得指定像素格式每个像素占用的比特数（Bit Per Pixel）。

    2.3 图像拉伸
        SWS_BICUBIC 性能比较好；
        SWS_FAST_BILINEAR 在性能和速度之间有一个比好好的平衡。
 */

#include "codectrl.h"
#include "../syslog/syslog.h"


/**
 * Mp4，mkv等格式解码为 .h264 和 .yuv数据
 * @param filepath
 * @param yuvpath
 * @param h264path
 * @return
 */
void *decode2Yuv420(void *args) {

    thread_args *arg = (thread_args *)args;
    const char *filepath = arg->filepath1;
    const char *yuvpath = arg->filepath2;

    struct AVFormatContext *pFormatCtx;
    int i, videoindex;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    int y_size;
    int ret = 0, got_picture = 0;
    struct SwsContext *img_convert_ctx;

    //创建解码后的输出文件
    FILE *fp_yuv = NULL;
    if (yuvpath)
        fp_yuv = fopen(yuvpath /*"output.yuv"*/, "wb+");
    else {
        ret = -1;
        goto finish;
    }

    LOGI("decode2Yuv420  start decode .\n");

    av_register_all();//注册所有组件
    avformat_network_init();//初始化网络
    pFormatCtx = avformat_alloc_context();//初始化一个AVFormatContext

    if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) {//打开输入的视频文件
        LOGE("Couldn't open input stream.\n");
        ret = -1;
        goto finish;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {//获取视频文件信息
        LOGE("Couldn't find stream information.\n");
        ret = -1;
        goto finish;
    }
    videoindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) //获取视频流的索引位置, 遍历所有类型的流（音频流、视频流、字幕流），找到视频流
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }

    if (videoindex == -1) {
        LOGE("Didn't find a video stream.\n");
        ret = -1;
        goto finish;
    }

    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);//查找解码器, 只有知道视频的编码方式，才能够根据编码方式去找到解码器
    if (pCodec == NULL) {
        LOGE("Codec not found.\n");
        ret = -1;
        goto finish;
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {//打开解码器
        LOGE("Could not open codec.\n");
        ret = -1;
        goto finish;
    }

    pFrame = av_frame_alloc();  //临时缓存数据
    pFrameYUV = av_frame_alloc(); //用于存储解码后的像素数据(YUV)
    out_buffer = (uint8_t *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1)); //只有指定了AVFrame的像素格式、画面大小才能真正分配内存
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P,
                         pCodecCtx->width, pCodecCtx->height, 1); //初始化解码后存储区
    packet = (AVPacket *) av_malloc(sizeof(AVPacket)); //缓冲区，开辟空间, 用于存储一帧一帧的压缩数据（H264）


    //用于转码（缩放）的参数，转之前的宽高，转之后的宽高，格式等
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, NULL, NULL, NULL);

    /*****打印信息和计时*******/
    int frame_cnt = 0;
    char info[1000] = {0};
    clock_t time_start, time_finish;

    sprintf(info, "\n[Input     ] %s\n", filepath);
    sprintf(info, "%s[Output    ] %s\n", info, yuvpath);
    sprintf(info, "%s[Format    ] %s\n", info, pFormatCtx->iformat->name);
    sprintf(info, "%s[Codec     ] %s\n", info, pCodecCtx->codec->name);
    sprintf(info, "%s[Resolution] %dx%d\n", info, pCodecCtx->width, pCodecCtx->height);

    time_start = clock();
    /*****打印信息和计时*******/

    //一帧一帧的读取压缩数据
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        //只要视频压缩数据（根据流的索引位置判断）
        if (packet->stream_index == videoindex) {

            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,
                                        packet); //解码一帧视频压缩数据，得到视频像素数据
            if (ret < 0) {
                LOGE("Decode Error.\n");
                goto finish;
            }
            //为0说明解码完成，非0正在解码
            if (got_picture) {
                //转换像素
                sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize,
                          0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

                //输出到YUV文件 三个颜色分量Y/U/V的地址分别为AVframe::data[0]、AVframe::data[1]和AVframe::data[2]，
                //图像的宽度分别为AVframe::linesize[0]、AVframe::linesize[1]和AVframe::linesize[2]。
                //data解码后的图像像素数据（音频采样数据）
                //Y 亮度 UV 色度（压缩了） 人对亮度更加敏感, U V 个数是Y的1/4
                y_size = pCodecCtx->width * pCodecCtx->height;
                fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
                fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
                fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V

                /*****打印信息*******/
                /*
                char pictype_str[10] = {0};
                switch (pFrame->pict_type) {
                    case AV_PICTURE_TYPE_I: sprintf(pictype_str, "I"); break;
                    case AV_PICTURE_TYPE_P: sprintf(pictype_str, "P"); break;
                    case AV_PICTURE_TYPE_B: sprintf(pictype_str, "B"); break;
                    default: sprintf(pictype_str, "Other"); break;
                }
                LOGI("Frame Index: %5d. Type:%s",frame_cnt,pictype_str);
                 */
                /*****打印信息*******/
                frame_cnt++;
            }
        }
        //释放资源
        av_packet_unref(packet);
    }

    if (!fp_yuv) {
        goto end;
    }

    //flush decoder
    /*当av_read_frame()循环退出的时候，实际上解码器中可能还包含剩余的几帧数据。 因此需要通过“flush_decoder”将这几帧数据输出。
   “flush_decoder”功能简而言之即直接调用avcodec_decode_video2()获得AVFrame，而不再向解码器传递AVPacket。*/
    while (1) {
        ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
        if (ret < 0)
            break;
        if (!got_picture)
            break;
        sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize, 0,
                  pCodecCtx->height,
                  pFrameYUV->data, pFrameYUV->linesize);

        int ySize = pCodecCtx->width * pCodecCtx->height;
        fwrite(pFrameYUV->data[0], 1, ySize, fp_yuv);    //Y
        fwrite(pFrameYUV->data[1], 1, ySize / 4, fp_yuv);  //U
        fwrite(pFrameYUV->data[2], 1, ySize / 4, fp_yuv);  //V

        frame_cnt++;
    }

    end:
    /*****计时*******/
    time_finish = clock();

    sprintf(info, "%s[FrameCount] %d\n", info, frame_cnt);
    sprintf(info, "%s[CostTime  ] %fs\n", info,
            (double) (time_finish - time_start) / CLOCKS_PER_SEC);
    LOGI("%s", info);
    /*****计时*******/

    sws_freeContext(img_convert_ctx);

    //关闭文件以及释放内存
    if (fp_yuv) {
        fclose(fp_yuv);
    }

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    finish:
    free(args);
    return (void *) ret;
}



/**
 * Mp4，mkv等格式解码为 .h264  有问题，未修复
 * @param filepath
 * @param h264path
 * @return
 */
int decode2H264(const char *filepath, const char *h264path) {
    struct AVFormatContext *pFormatCtx;
    int i, videoindex;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    struct SwsContext *img_convert_ctx;

    //创建解码后的输出文件
    FILE *fp_h264 = NULL;
    if (h264path)
        fp_h264 = fopen(h264path /*"output.h264"*/, "wb+");
    else
        return -1;

    LOGI("decode2H264  start decode .\n");

    av_register_all();//注册所有组件
    avformat_network_init();//初始化网络
    pFormatCtx = avformat_alloc_context();//初始化一个AVFormatContext

    if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) {//打开输入的视频文件
        LOGE("Couldn't open input stream.\n");
        return -1;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {//获取视频文件信息
        LOGE("Couldn't find stream information.\n");
        return -1;
    }
    videoindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) //获取视频流的索引位置, 遍历所有类型的流（音频流、视频流、字幕流），找到视频流
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }

    if (videoindex == -1) {
        LOGE("Didn't find a video stream.\n");
        return -1;
    }

    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);//查找解码器, 只有知道视频的编码方式，才能够根据编码方式去找到解码器
    if (pCodec == NULL) {
        LOGE("Codec not found.\n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {//打开解码器
        LOGE("Could not open codec.\n");
        return -1;
    }

    pFrame = av_frame_alloc();  //临时缓存数据
    pFrameYUV = av_frame_alloc(); //用于存储解码后的像素数据(YUV)
    out_buffer = (uint8_t *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1)); //只有指定了AVFrame的像素格式、画面大小才能真正分配内存
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P,
                         pCodecCtx->width, pCodecCtx->height, 1); //初始化解码后存储区
    packet = (AVPacket *) av_malloc(sizeof(AVPacket)); //缓冲区，开辟空间, 用于存储一帧一帧的压缩数据（H264）


    //用于转码（缩放）的参数，转之前的宽高，转之后的宽高，格式等
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, NULL, NULL, NULL);

    /*****打印信息和计时*******/
    int frame_cnt = 0;
    char info[1000] = {0};
    clock_t time_start, time_finish;

    sprintf(info, "\n[Input     ] %s\n", filepath);
    sprintf(info, "%s[Output    ] %s\n", info, h264path);
    sprintf(info, "%s[Format    ] %s\n", info, pFormatCtx->iformat->name);
    sprintf(info, "%s[Codec     ] %s\n", info, pCodecCtx->codec->name);
    sprintf(info, "%s[Resolution] %dx%d\n", info, pCodecCtx->width, pCodecCtx->height);

    time_start = clock();
    /*****打印信息和计时*******/

    //一帧一帧的读取压缩数据
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        //只要视频压缩数据（根据流的索引位置判断）
        if (packet->stream_index == videoindex) {

            if (fp_h264) {
                fwrite(packet->data, 1, (size_t) packet->size, fp_h264); //把H264数据写入fp_h264文件
            }

            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,
                                        packet); //解码一帧视频压缩数据，得到视频像素数据
            if (ret < 0) {
                LOGE("Decode Error.\n");
                return -1;
            }
            //为0说明解码完成，非0正在解码
            if (got_picture) {
                //转换像素
                sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize,
                          0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);



                frame_cnt++;
            }
        }
        //释放资源
        av_packet_unref(packet);
    }


    //flush decoder
    /*当av_read_frame()循环退出的时候，实际上解码器中可能还包含剩余的几帧数据。 因此需要通过“flush_decoder”将这几帧数据输出。
   “flush_decoder”功能简而言之即直接调用avcodec_decode_video2()获得AVFrame，而不再向解码器传递AVPacket。*/
    while (1) {
        ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
        if (ret < 0)
            break;
        if (!got_picture)
            break;
        sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize, 0,
                  pCodecCtx->height,
                  pFrameYUV->data, pFrameYUV->linesize);


        frame_cnt++;
    }

    end:
    /*****计时*******/
    time_finish = clock();

    sprintf(info, "%s[FrameCount] %d\n", info, frame_cnt);
    sprintf(info, "%s[CostTime  ] %fs\n", info,
            (double) (time_finish - time_start) / CLOCKS_PER_SEC);
    LOGI("%s", info);
    /*****计时*******/

    sws_freeContext(img_convert_ctx);

    //关闭文件以及释放内存
    if (fp_h264) {
        fclose(fp_h264);
    }

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
}



/**
 * 将 .yuv 数据编码为 .264
 * @param yuvpath
 * @return
 */
int encodeYuv2H264(const char *yuvpath, const char *h264parh) {
    AVCodec *pCodec;
    AVCodecContext *pCodecCtx = NULL;
    int i, ret, got_output;
    FILE *fp_in;
    FILE *fp_out;
    AVFrame *pFrame;
    AVPacket pkt;
    int y_size;
    int framecnt = 0;

    LOGI("encodeYuv2H264OrH265  start encode .\n");

    enum AVCodecID codec_id = AV_CODEC_ID_HEVC;

    int in_w = 480, in_h = 272;
    int framenum = 100;

    //注册所有的编解码器
    avcodec_register_all();

    //查找编码器
    pCodec = avcodec_find_encoder(codec_id);
    if (!pCodec) {
        LOGE("Codec not found\n");
        return -1;
    }
    //为AVCodecContext分配内存，创建AVCodecContext结构体
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx) {
        LOGE("Could not allocate video codec context\n");
        return -1;
    }
    pCodecCtx->bit_rate = 400000;
    pCodecCtx->width = in_w;
    pCodecCtx->height = in_h;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;
    pCodecCtx->gop_size = 10;
    pCodecCtx->max_b_frames = 1;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec_id == AV_CODEC_ID_H264)
        av_opt_set(pCodecCtx->priv_data, "preset", "slow", 0);

    //打开编码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec\n");
        return -1;
    }

    pFrame = av_frame_alloc();
    if (!pFrame) {
        LOGE("Could not allocate video frame\n");
        return -1;
    }
    pFrame->format = pCodecCtx->pix_fmt;
    pFrame->width = pCodecCtx->width;
    pFrame->height = pCodecCtx->height;

    ret = av_image_alloc(pFrame->data, pFrame->linesize, pCodecCtx->width, pCodecCtx->height,
                         pCodecCtx->pix_fmt, 16);
    if (ret < 0) {
        LOGE("Could not allocate raw picture buffer\n");
        return -1;
    }
    //Input raw data
    fp_in = fopen(yuvpath, "rb");
    if (!fp_in) {
        LOGE("Could not open %s\n", yuvpath);
        return -1;
    }
    //Output bitstream
    fp_out = fopen(h264parh, "wb");
    if (!fp_out) {
        LOGE("Could not open %s\n", h264parh);
        return -1;
    }

    y_size = pCodecCtx->width * pCodecCtx->height;
    //Encode
    for (i = 0; i < framenum; i++) {
        av_init_packet(&pkt);
        pkt.data = NULL;    // packet data will be allocated by the encoder
        pkt.size = 0;
        //Read raw YUV data
        if (fread(pFrame->data[0], 1, y_size, fp_in) <= 0 ||     // Y
            fread(pFrame->data[1], 1, y_size / 4, fp_in) <= 0 ||   // U
            fread(pFrame->data[2], 1, y_size / 4, fp_in) <= 0) {   // V
            return -1;
        } else if (feof(fp_in)) {
            break;
        }

        pFrame->pts = i;
        //编码一帧数据
        ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_output);
        if (ret < 0) {
            LOGE("Error encoding frame\n");
            return -1;
        }
        if (got_output) {
            LOGE("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, pkt.size);
            framecnt++;
            fwrite(pkt.data, 1, pkt.size, fp_out);
            av_free_packet(&pkt);
        }
    }
    //Flush Encoder
    for (got_output = 1; got_output; i++) {
        ret = avcodec_encode_video2(pCodecCtx, &pkt, NULL, &got_output);
        if (ret < 0) {
            LOGE("Error encoding frame\n");
            return -1;
        }
        if (got_output) {
            LOGE("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n", pkt.size);
            fwrite(pkt.data, 1, pkt.size, fp_out);
            av_free_packet(&pkt);
        }
    }

    fclose(fp_out);
    avcodec_close(pCodecCtx);
    av_free(pCodecCtx);
    av_freep(&pFrame->data[0]);
    av_frame_free(&pFrame);

    return 0;
}



/**
 * 格式的转化
 * 本程序并不进行视音频的编码和解码工作。
 * 而是直接将视音频压缩码流从一种封装格式文件中获取出来然后打包成另外一种封装格式的文件。
 * 因为不需要进行视音频的编码和解码，所以不会有视音频的压缩损伤
 * @param srcpath
 * @param dedtpath
 * @return
 */
int transformFMT(const char *srcpath, const char *dedtpath) {
    AVOutputFormat *ofmt = NULL;
    //Input AVFormatContext and Output AVFormatContext
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;
    const char *in_filename, *out_filename;
    int ret, i;
    int frame_index = 0;
    in_filename = "ws.mp4";//Input file URL
    out_filename = "ws.avi";//Output file URL

    av_register_all();
    //Input
    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {//打开媒体文件
        LOGE("Could not open input file.");
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {//获取视频信息
        LOGE("Failed to retrieve input stream information");
        goto end;
    }
    av_dump_format(ifmt_ctx, 0, in_filename, 0);
    //初始化输出视频码流的AVFormatContext。
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    if (!ofmt_ctx) {
        LOGE("Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    ofmt = ofmt_ctx->oformat;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx,
                                                   in_stream->codec->codec);// 初始化输出码流AVStream
        if (!out_stream) {
            LOGE("Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        //关键：Copy the settings of AVCodecContext 拷贝输入视频码流的AVCodecContex 到输出视频的AVCodecContext
        if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
            LOGE("Failed to copy context from input to output stream codec context\n");
            goto end;
        }
        out_stream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
    //Output information------------------
    av_dump_format(ofmt_ctx, 0, out_filename, 1);

    //Open output file
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);//打开输出文件。
        if (ret < 0) {
            LOGE("Could not open output file '%s'", out_filename);
            goto end;
        }
    }
    //Write file header
    if (avformat_write_header(ofmt_ctx, NULL) < 0) { //写文件头（对于某些没有文件头的封装格式，不需要此函数。比如说MPEG2TS）
        LOGE("Error occurred when opening output file\n");
        goto end;
    }

    while (1) {
        AVStream *in_stream, *out_stream;
        //Get an AVPacket 从输入文件中读取一个AVPacket
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;
        in_stream = ifmt_ctx->streams[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];

        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        //Write
        if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {//将AVPacket（存储视频压缩码流数据）写入文件
            LOGE("Error muxing packet\n");
            break;
        }
        LOGE("Write %8d frames to output file\n", frame_index);
        av_free_packet(&pkt);
        frame_index++;
    }
    //写文件尾（对于某些没有文件头的封装格式，不需要此函数。比如说MPEG2TS）
    av_write_trailer(ofmt_ctx);
    end:
    avformat_close_input(&ifmt_ctx);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    return 0;
}



/**
 * libswscale实现YUV转RGB; .yuv 转成 .rgb
 * @param yuvpath
 * @param rgbpath
 * @return
 */
int decodeYuv2Rgb(const char *yuvpath, const char *rgbpath) {
    //Parameters
    FILE *src_file = fopen(yuvpath /*"ws.yuv"*/, "rb");
    const int src_w = 1920, src_h = 1080;
    enum AVPixelFormat src_pixfmt = AV_PIX_FMT_YUV420P;

    //该函数可以获得指定像素格式每个像素占用的比特数（Bit Per Pixel）,
    //av_pix_fmt_desc_get()可以获得指定像素格式的AVPixFmtDescriptor结构体。
    int src_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(src_pixfmt));

    FILE *dst_file = fopen(rgbpath /*"ws.rgb"*/, "wb");
    const int dst_w = 1280, dst_h = 720;
    enum AVPixelFormat dst_pixfmt = AV_PIX_FMT_RGB24;
    int dst_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(dst_pixfmt));

    //Structures
    uint8_t *src_data[4];
    int src_linesize[4];

    uint8_t *dst_data[4];
    int dst_linesize[4];

    struct SwsContext *img_convert_ctx;
    uint8_t *temp_buffer = (uint8_t *) malloc(src_w * src_h * src_bpp / 8);

    int frame_idx = 0;
    int ret = 0;
    ret = av_image_alloc(src_data, src_linesize, src_w, src_h, src_pixfmt, 1);//分配资源控件
    if (ret < 0) {
        LOGE("Could not allocate source image\n");
        return -1;
    }
    ret = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pixfmt, 1);
    if (ret < 0) {
        LOGE("Could not allocate destination image\n");
        return -1;
    }
    //-----------------------------

    img_convert_ctx = sws_alloc_context();//为SwsContext结构体分配内存。
    //Show AVOption
    av_opt_show2(img_convert_ctx, stdout, AV_OPT_FLAG_VIDEO_PARAM, 0);
    //Set Value
    av_opt_set_int(img_convert_ctx, "sws_flags", SWS_BICUBIC | SWS_PRINT_INFO, 0);
    av_opt_set_int(img_convert_ctx, "srcw", src_w, 0);
    av_opt_set_int(img_convert_ctx, "srch", src_h, 0);
    av_opt_set_int(img_convert_ctx, "src_format", src_pixfmt, 0);
    //'0' for MPEG (Y:16-235); '1' for JPEG (Y:0-255)
    av_opt_set_int(img_convert_ctx, "src_range", 1, 0);
    av_opt_set_int(img_convert_ctx, "dstw", dst_w, 0);
    av_opt_set_int(img_convert_ctx, "dsth", dst_h, 0);
    av_opt_set_int(img_convert_ctx, "dst_format", dst_pixfmt, 0);
    av_opt_set_int(img_convert_ctx, "dst_range", 1, 0);
    sws_init_context(img_convert_ctx, NULL, NULL);//对SwsContext中的各种变量进行赋值


    while (1) {
        if (fread(temp_buffer, 1, src_w * src_h * src_bpp / 8, src_file) !=
            src_w * src_h * src_bpp / 8) {
            break;
        }

        switch (src_pixfmt) {
            case AV_PIX_FMT_GRAY8: {
                memcpy(src_data[0], temp_buffer, src_w * src_h);
                break;
            }
            case AV_PIX_FMT_YUV420P: {
                memcpy(src_data[0], temp_buffer, src_w * src_h);                    //Y
                memcpy(src_data[1], temp_buffer + src_w * src_h, src_w * src_h / 4);      //U
                memcpy(src_data[2], temp_buffer + src_w * src_h * 5 / 4, src_w * src_h / 4);  //V
                break;
            }
            case AV_PIX_FMT_YUV422P: {
                memcpy(src_data[0], temp_buffer, src_w * src_h);                    //Y
                memcpy(src_data[1], temp_buffer + src_w * src_h, src_w * src_h / 2);      //U
                memcpy(src_data[2], temp_buffer + src_w * src_h * 3 / 2, src_w * src_h / 2);  //V
                break;
            }
            case AV_PIX_FMT_YUV444P: {
                memcpy(src_data[0], temp_buffer, src_w * src_h);                    //Y
                memcpy(src_data[1], temp_buffer + src_w * src_h, src_w * src_h);        //U
                memcpy(src_data[2], temp_buffer + src_w * src_h * 2, src_w * src_h);      //V
                break;
            }
            case AV_PIX_FMT_YUYV422: {
                memcpy(src_data[0], temp_buffer, src_w * src_h * 2);                  //Packed
                break;
            }
            case AV_PIX_FMT_RGB24: {
                memcpy(src_data[0], temp_buffer, src_w * src_h * 3);                  //Packed
                break;
            }
            default: {
                LOGE("Not Support Input Pixel Format.\n");
                break;
            }
        }

        sws_scale(img_convert_ctx, src_data, src_linesize, 0, src_h, dst_data, dst_linesize);//转换像素
        LOGE("Finish process frame %5d\n", frame_idx);
        frame_idx++;

        switch (dst_pixfmt) {
            case AV_PIX_FMT_GRAY8: {
                fwrite(dst_data[0], 1, dst_w * dst_h, dst_file);
                break;
            }
            case AV_PIX_FMT_YUV420P: {
                fwrite(dst_data[0], 1, dst_w * dst_h, dst_file);                 //Y
                fwrite(dst_data[1], 1, dst_w * dst_h / 4, dst_file);               //U
                fwrite(dst_data[2], 1, dst_w * dst_h / 4, dst_file);               //V
                break;
            }
            case AV_PIX_FMT_YUV422P: {
                fwrite(dst_data[0], 1, dst_w * dst_h, dst_file);                 //Y
                fwrite(dst_data[1], 1, dst_w * dst_h / 2, dst_file);               //U
                fwrite(dst_data[2], 1, dst_w * dst_h / 2, dst_file);               //V
                break;
            }
            case AV_PIX_FMT_YUV444P: {
                fwrite(dst_data[0], 1, dst_w * dst_h, dst_file);                 //Y
                fwrite(dst_data[1], 1, dst_w * dst_h, dst_file);                 //U
                fwrite(dst_data[2], 1, dst_w * dst_h, dst_file);                 //V
                break;
            }
            case AV_PIX_FMT_YUYV422: {
                fwrite(dst_data[0], 1, dst_w * dst_h * 2, dst_file);               //Packed
                break;
            }
            case AV_PIX_FMT_RGB24: {
                fwrite(dst_data[0], 1, dst_w * dst_h * 3, dst_file);               //Packed
                break;
            }
            default: {
                LOGE("Not Support Output Pixel Format.\n");
                break;
            }
        }
    }

    sws_freeContext(img_convert_ctx);

    free(temp_buffer);
    fclose(dst_file);
    av_freep(&src_data[0]);
    av_freep(&dst_data[0]);

    return 0;
}




/**
 * 刷出编码器最后的几帧数据
 * @param fmt_ctx
 * @param stream_index
 * @return
 */
static int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index) {
    int ret;
    int got_frame;
    AVPacket enc_pkt;
    if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities & CODEC_CAP_DELAY))
        return 0;
    while (1) {
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        av_init_packet(&enc_pkt);
        ret = avcodec_encode_audio2(fmt_ctx->streams[stream_index]->codec, &enc_pkt,
                                    NULL, &got_frame);
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame) {
            ret = 0;
            break;
        }
        LOGE("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n", enc_pkt.size);
        /* mux encoded frame */
        ret = av_write_frame(fmt_ctx, &enc_pkt);
        if (ret < 0)
            break;
    }
    return ret;
}



/**
 * 音频PCM采样数据编码为压缩码流（MP3，WMA，AAC等)
 * MPEG2-PS主要应用于存储的具有固定时长的节目，如DVD电影，而MPEG-TS则主要应用于实时传送的节目，比如实时广播的电视节目。
 * 区别: 你将DVD上的VOB文件的前面一截cut掉（或者干脆就是数据损坏），那么就会导致整个文件无法解码了，
 * 而电视节目是你任何时候打开电视机都能解码（收看）的，所以，MPEG2-TS格式的特点就是要求从视频流的任一片段开始都是可以独立解码的
 * @param in_filepath
 * @param out_file
 * @return
 */
int encodePCM(const char *in_filepath, const char *out_file) {
//    const char *out_file = "ws.aac";          //Output URL
    AVFormatContext *pFormatCtx;
    AVOutputFormat *fmt;
    AVStream *audio_st;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;

    uint8_t *frame_buf;
    AVFrame *pFrame;
    AVPacket pkt;

    int got_frame = 0;
    int ret = 0;
    int size = 0;

    FILE *in_file = NULL;                         //Raw PCM data
    int framenum = 1000;                          //Audio frame number
    int i;

    in_file = fopen(in_filepath /*"ws.pcm"*/, "rb");

    //注册FFmpeg所有编解码器。
    av_register_all();

    //初始化输出码流的AVFormatContext。
    pFormatCtx = avformat_alloc_context();
    //得到最合适的AVOutputFormat并且返回给avformat_alloc_output_context2()，赋值给刚刚新建的AVFormatContext
    fmt = av_guess_format(NULL, out_file, NULL);
    pFormatCtx->oformat = fmt;


    //Open output URL 创建的AVIOContext结构体 打开输出文件
    if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
        LOGE("Failed to open output file!\n");
        return -1;
    }
    //初始化AVFrame,创建输出码流的AVStream
    audio_st = avformat_new_stream(pFormatCtx, 0);
    if (audio_st == NULL) {
        return -1;
    }
    pCodecCtx = audio_st->codec;
    pCodecCtx->codec_id = fmt->audio_codec;
    pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    pCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    pCodecCtx->sample_rate = 44100;
    pCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
    pCodecCtx->channels = av_get_channel_layout_nb_channels(pCodecCtx->channel_layout);
    pCodecCtx->bit_rate = 64000;

    /*av_dump_format()是一个手工调试的函数，能使我们看到pFormatCtx->streams里面有什么内容。
        一般接下来我们使用av_find_stream_info()函数，它的作用是为pFormatCtx->streams填充上正确的信息。*/
    av_dump_format(pFormatCtx, 0, out_file, 1);

    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);//查找编码器
    if (!pCodec) {
        LOGE("Can not find encoder!\n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {//初始化一个编解码器的AVCodecContext
        LOGE("Failed to open encoder!\n");
        return -1;
    }
    pFrame = av_frame_alloc();
    pFrame->nb_samples = pCodecCtx->frame_size;
    pFrame->format = pCodecCtx->sample_fmt;

    size = av_samples_get_buffer_size(NULL, pCodecCtx->channels, pCodecCtx->frame_size,
                                      pCodecCtx->sample_fmt, 1);
    frame_buf = (uint8_t *) av_malloc(size);
    avcodec_fill_audio_frame(pFrame, pCodecCtx->channels, pCodecCtx->sample_fmt,
                             (const uint8_t *) frame_buf, size, 1);

    //写文件头（对于某些没有文件头的封装格式，不需要此函数。比如说MPEG2TS）
    avformat_write_header(pFormatCtx, NULL);

    av_new_packet(&pkt, size);

    for (i = 0; i < framenum; i++) {
        //Read PCM
        if (fread(frame_buf, 1, size, in_file) <= 0) {
            LOGE("Failed to read raw data! \n");
            return -1;
        } else if (feof(in_file)) {
            break;
        }
        pFrame->data[0] = frame_buf;  //PCM Data

        pFrame->pts = i * 100;
        got_frame = 0;

        //编码音频。即将AVFrame（存储PCM采样数据）编码为AVPacket（存储AAC，MP3等格式的码流数据）
        ret = avcodec_encode_audio2(pCodecCtx, &pkt, pFrame, &got_frame);
        if (ret < 0) {
            LOGE("Failed to encode!\n");
            return -1;
        }
        if (got_frame == 1) {
            LOGE("Succeed to encode 1 frame! \tsize:%5d\n", pkt.size);
            pkt.stream_index = audio_st->index;
            ret = av_write_frame(pFormatCtx, &pkt); //编码后的视频码流写入文件
            av_free_packet(&pkt);
        }
    }

    //Flush Encoder
    ret = flush_encoder(pFormatCtx, 0);
    if (ret < 0) {
        LOGE("Flushing encoder failed\n");
        return -1;
    }

    //写文件尾（对于某些没有文件头的封装格式，不需要此函数。比如说MPEG2TS）
    av_write_trailer(pFormatCtx);

    //Clean
    if (audio_st) {
        avcodec_close(audio_st->codec);
        av_free(pFrame);
        av_free(frame_buf);
    }
    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);

    fclose(in_file);

    return 0;
}



/**
 * 使用libavfilter为视音频添加特效功能
 *
    // 滤镜的内容  官网 http://www.ffmpeg.org/ffmpeg-filters.html
    视频filter
    //const char *filter_descr = "lutyuv='u=128:v=128'";                    // 变成灰色
    //const char *filter_descr = "boxblur";                                 // 模糊处理
    //const char *filter_descr = "hflip";                                   // 水平翻转  vflip  上下镜像   hflip 左右镜像
    //const char *filter_descr = "hue='h=60:s=-3'";                         // 色相饱和度  h 色度角度  s 饱和度 -10 和 10 之间，默认1
    //const char *filter_descr = "crop=2/3*in_w:1/2*in_h";                  // 裁剪后尺寸 宽*2/3 高*1/2
    //const char *filter_descr = "crop=320:240:100:200";                    // 裁剪后尺寸 320 * 240， 裁剪区域的左上角坐标 100，200
    //const char *filter_descr = "scale=iw*2:ih*3";                         // 缩放为 宽 * 2  高 * 3
    //const char *filter_descr = "scale=320:240";                           // 缩放为 320 * 240
    //const char *filter_descr = "transpose=dir=1:passthrough=portrait";    // 旋转  顺时针旋转 1 * 90°， 保持竖直方向   passthrough： 默认 none， 可选 portrait， landscape
    //const char *filter_descr = "drawbox=x=100:y=100:w=200:h=100:color=pink@0.5:t=2";      // 矩形水印，矩形左上角坐标 100，100 ，宽高 200*100，颜色 pink, 不透明度0.5, 线宽 2
    //const char *filter_descr = "drawgrid=width=100:height=100:thickness=2:color=red@0.5";      // 网格水印，width 和 height 表示添加网格的宽和高，thickness表示网格的线宽，color表示颜色
    //const char *filter_descr = "movie=/storage/emulated/0/my_logo.png[wm];[in][wm]overlay=5:5[out]";  // 图片水印, 位置坐标为 5，5
    const char *filter_descr = "drawtext=fontfile=arial.ttf:fontcolor=red:fontsize=50:text='huyu'";  // 文字水印  fontfile=/home/code/simhei.ttf 可以写绝对路径
    //const char *filter_descr = "subtitles=filename=huyu.srt";     // 硬字幕，将 huyu.srt 字幕文件 合成到视频流
    //const char *filter_descr = "subtitles=filename=huyu.srt:force_style='FontName=DejaVu Serif,PrimaryColour=&HAA00FF00'";     // 硬字幕，将 huyu.srt 字幕文件 合成到视频流，字幕字体 DejaVu Serif， 颜色 &HAA00FF00
    //const char *filter_descr = "subtitles=video1.mkv";            // 硬字幕，将输入文件 video1 的第一个字幕流合成到视频流中
    //const char *filter_descr = "subtitles=video1.mkv:si=1";       // 硬字幕，将输入文件 video1 的第二个字幕流合成到另一个容器的视频流中输出：

    音频filter
    adelay=1500|0|500   实现不同声道的延时处理。使用参数如下adelay=1500|0|500，这个例子中实现第一个声道的延迟1.5s，第三个声道延迟0.5s，第二个声道不做调整
    aecho               实现回声效果，具体参考http://ffmpeg.org/ffmpeg-filters.html#aecho。
    amerge              将多个音频流合并成一个多声道音频流。具体参考http://ffmpeg.org/ffmpeg-filters.html#amerge-1。
    ashowinfo           显示每一个audio frame的信息，比如时间戳、位置、采样格式、采样率、采样点数等。具体参考http://ffmpeg.org/ffmpeg-filters.html#ashowinfo。
    pan                 特定声道处理，比如立体声变为单声道，或者通过特定参数修改声道或交换声道。
        主要有两大类：
            混音处理，比如下面的例子pan=1c|c0=0.9*c0+0.1*c1，实现立体声到单声道的变换；
            声道变换，比如5.1声道顺序调整，pan="5.1| c0=c1 | c1=c0 | c2=c2 | c3=c3 | c4=c4 | c5=c5"。
    silencedetect 和 silenceremove         根据特定参数检测静音和移除静音
    volume 和 volumedetect                 这两个filter分别实现音量调整和音量检测

    audio source filter
        aevalsrc filter             按照特定表达式生成音频信号。
        anullsrc filter             生成特定的原始音频数据，用于模板或测试。
        anoisesrc filter            生成噪声音频信号。
        sine filter                 生成正弦波音频信号。

    audio sink filter
        abuffersink  和 anullsink ，这些filter只是用于特定情况下结束filter chain


多个 filter 组合时，中间用 , 隔开
1、命令中同一个滤镜链中的滤镜之间通过逗号分隔开，如上例中[crop]和[vflip]处于同一个滤镜链，而知以逗号分隔开。
2、不同的滤镜链之间以分号隔开，如上例中[crop]和[vflip]组成的滤镜链与[split]、[overlay]之间。
3、滤镜链的输入输出通过方括号标记命名，若示例中[split]滤镜产生了两路输出，分别被标记为[main]和[tmp]

    AVFilter:       代表一个filter.
    AVFilterPad:    代表一个filter的输入或输出口.只有输出pad的filter叫source,只有输入pad的tilter叫sink.
    AVFilterGraph:  代表一串连接起来的filter们.
    AVFilterLink:   代表两个连接的fitler之间的粘合物.若一个filter的输出pad和另一个filter的输入pad名字相同，即认为两个filter之间建立了link
    AVFilterChain:  代表一串相互连接的filters，除了source和sink外，要求每个filter的输入输出pad都有对应的输出和输入pad
 *
 * @param srcyuvpath
 * @param destyuvpath
 * @return
 */
int addFilters(char *filter_descr, const char *srcyuvpath, const char *destyuvpath) {
    int ret;
    AVFrame *frame_in;
    AVFrame *frame_out;
    unsigned char *frame_buffer_in;
    unsigned char *frame_buffer_out;

    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;

    //Input YUV
    FILE *fp_in = fopen(srcyuvpath /*"ws.yuv"*/, "rb+");
    if (fp_in == NULL) {
        LOGE("Error open input file.\n");
        return -1;
    }

    //Output YUV
    FILE *fp_out = fopen(destyuvpath /*"ws_output.yuv"*/, "wb+");
    if (fp_out == NULL) {
        LOGE("Error open output file.\n");
        return -1;
    }

    av_register_all();
    avfilter_register_all();//注册所有AVFilter。

    AVFormatContext *pFormatCtx = avformat_alloc_context();
    // Open video file
    if (avformat_open_input(&pFormatCtx, srcyuvpath, NULL, NULL) != 0) {
        LOGE("Couldn't open file:%s\n", srcyuvpath);
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


    char args[512];
    // 一定要的两个过滤器是buffer过滤器和buffersink过滤器
    AVFilter *buffersrc = avfilter_get_by_name("buffer");           //作用是将解码后的画面加载到过滤器链中
    AVFilter *buffersink = avfilter_get_by_name("ffbuffersink");    //作用是将处理好的画面从过滤器链中读取出来
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};
    AVBufferSinkParams *buffersink_params;

    //创建AVFilterGraph
    filter_graph = avfilter_graph_alloc();

    int in_width = pCodecCtx->width/*1920*/;
    int in_height = pCodecCtx->height/*1080*/;
    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    // 拷贝内容到 args
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             in_width, in_height,
            // AV_PIX_FMT_YUV420P, 1, 25, 1, 1);
             pCodecCtx->pix_fmt, pCodecCtx->time_base.num, pCodecCtx->time_base.den,
             pCodecCtx->sample_aspect_ratio.num, pCodecCtx->sample_aspect_ratio.den);

    //创建并向FilterGraph中添加一个Filter 创建source。
    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in", args, NULL, filter_graph);
    if (ret < 0) {
        LOGE("Cannot create buffer source\n");
        return ret;
    }

    /* buffer video sink: to terminate the filter chain. */
    buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;
    // 创建sink
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out", NULL, buffersink_params,
                                       filter_graph);
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

    //将一串通过字符串描述的Graph添加到FilterGraph中。
    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_descr,
                                        &inputs, &outputs, NULL)) < 0) {
        LOGE("Cannot avfilter_graph_parse_ptr\n");
        return ret;
    }
    //检查FilterGraph的配置。
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
        LOGE("Cannot avfilter_graph_config\n");
        return ret;
    }

    frame_in = av_frame_alloc();
    frame_buffer_in = (unsigned char *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, in_width, in_height, 1));
    av_image_fill_arrays(frame_in->data, frame_in->linesize, frame_buffer_in,
                         AV_PIX_FMT_YUV420P, in_width, in_height, 1);

    frame_out = av_frame_alloc();
    frame_buffer_out = (unsigned char *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, in_width, in_height, 1));
    av_image_fill_arrays(frame_out->data, frame_out->linesize, frame_buffer_out,
                         AV_PIX_FMT_YUV420P, in_width, in_height, 1);

    frame_in->width = in_width;
    frame_in->height = in_height;
    frame_in->format = AV_PIX_FMT_YUV420P;

    while (1) {

        if (fread(frame_buffer_in, 1, in_width * in_height * 3 / 2, fp_in) !=
            in_width * in_height * 3 / 2) {
            break;
        }
        //input Y,U,V
        frame_in->data[0] = frame_buffer_in;
        frame_in->data[1] = frame_buffer_in + in_width * in_height;
        frame_in->data[2] = frame_buffer_in + in_width * in_height * 5 / 4;
        //向FilterGraph中加入一个AVFrame。
        if (av_buffersrc_add_frame(buffersrc_ctx, frame_in) < 0) {
            LOGE("Error while add frame.\n");
            break;
        }

        /* pull filtered pictures from the filtergraph  从FilterGraph中取出一个AVFrame。*/
        ret = av_buffersink_get_frame(buffersink_ctx, frame_out);
        if (ret < 0)
            break;

        //output Y,U,V
        if (frame_out->format == AV_PIX_FMT_YUV420P) {
            for (int i = 0; i < frame_out->height; i++) {
                fwrite(frame_out->data[0] + frame_out->linesize[0] * i, 1, frame_out->width,
                       fp_out);
            }
            for (int i = 0; i < frame_out->height / 2; i++) {
                fwrite(frame_out->data[1] + frame_out->linesize[1] * i, 1, frame_out->width / 2,
                       fp_out);
            }
            for (int i = 0; i < frame_out->height / 2; i++) {
                fwrite(frame_out->data[2] + frame_out->linesize[2] * i, 1, frame_out->width / 2,
                       fp_out);
            }
        }
        LOGE("Process 1 frame!\n");
    }

    fclose(fp_in);
    fclose(fp_out);

    av_frame_free(&frame_in);
    av_frame_free(&frame_out);
    avfilter_graph_free(&filter_graph);

    return 0;
}









