#include "../codectrl/codectrl.h"
#include "../codectrl/liveplayer.h"
#include "../thread/ffmpeg_thread.h"

JNIEXPORT jstring  JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_printInfo(JNIEnv *env, jobject instance) {

    char info[10000] = {0};
    sprintf(info, "%s\n", avcodec_configuration());
    return (*env)->NewStringUTF(env,info);

}

JNIEXPORT jint JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_decode2YUV420(JNIEnv *env, jobject instance, jstring input_,
                                       jstring output_yuv) {
    const char *input = (*env)->GetStringUTFChars(env, input_, 0);
    const char *poutput_yuv = (*env)->GetStringUTFChars(env, output_yuv, 0);


    thread_args* args = malloc(sizeof(thread_args));
    args->filepath1 = input;
    args->filepath2 = poutput_yuv;
    ffmpeg_thread_run(decode2Yuv420, args);
//    jint result = decode2Yuv420(args);

    (*env)->ReleaseStringUTFChars(env, input_, input);
    (*env)->ReleaseStringUTFChars(env, output_yuv, poutput_yuv);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_encodeYuv2H264(JNIEnv *env, jobject instance, jstring input_,
                                       jstring output_) {
    const char *input = (*env)->GetStringUTFChars(env, input_, 0);
    const char *output = (*env)->GetStringUTFChars(env, output_, 0);

    jint result = encodeYuv2H264(input, output);

    (*env)->ReleaseStringUTFChars(env, input_, input);
    (*env)->ReleaseStringUTFChars(env, output_, output);

    return result;
}

JNIEXPORT void JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_startPlay(JNIEnv *env, jobject instance, jstring input_,
                                     jobject surface) {
    const char *input = (*env)->GetStringUTFChars(env, input_, 0);

    playVideo(env, surface, input);

    (*env)->ReleaseStringUTFChars(env, input_, input);

}

JNIEXPORT void JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_resumePlay(JNIEnv *env, jobject instance) {
    resumePlayVideo();
}

JNIEXPORT void JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_pausePlay(JNIEnv *env, jobject instance) {
    pauseVideo();
}

JNIEXPORT void JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_stopPlay(JNIEnv *env, jobject instance) {
    stopPlayVideo();
}

JNIEXPORT void JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_playFilterVideo(JNIEnv *env, jobject instance, jstring input_,
                                                jstring filters_, jobject surface) {
    const char *input = (*env)->GetStringUTFChars(env, input_, 0);
    const char *filters = (*env)->GetStringUTFChars(env, filters_, 0);

    playFilterVideo(env, surface, input, filters);

    (*env)->ReleaseStringUTFChars(env, input_, input);
    (*env)->ReleaseStringUTFChars(env, filters_, filters);
}



JNIEXPORT void JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_cameraLiveInit(JNIEnv *env, jobject instance, jint width,
                                               jint height, jstring url_) {
    const char *url = (*env)->GetStringUTFChars(env, url_, 0);

    cameraLivePlayerInit(width, height, url);

    (*env)->ReleaseStringUTFChars(env, url_, url);
}

JNIEXPORT void JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_cameraLiveStart(JNIEnv *env, jobject instance,
                                                jbyteArray cameraData_) {
    jbyte *cameraData = (*env)->GetByteArrayElements(env, cameraData_, NULL);

    cameraLivePlayerStart(cameraData);

    (*env)->ReleaseByteArrayElements(env, cameraData_, cameraData, 0);
}

JNIEXPORT void JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_cameraLiveStop(JNIEnv *env, jobject instance) {

    cameraLivePlayerStop();
    cameraLivePlayerClose();
}
