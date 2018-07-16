#include <jni.h>
#include "../syslog/syslog.h"
#include "../cmdctrl/ffmpeg.h"
#include "../thread/ffmpeg_thread.h"

static JavaVM *jvm = NULL;
//java虚拟机
static jclass m_clazz = NULL;//当前类(面向java)

/**
 * 回调执行Java方法
 * 参看 Jni反射+Java反射
 */
void callJavaMethodOnExcuted(JNIEnv *env, jclass clazz, int ret) {
    if (clazz == NULL) {
        LOGE("---------------clazz isNULL---------------");
        return;
    }
    //获取方法ID (I)V指的是方法签名 通过javap -s -public FFmpegCmd 命令生成
    jmethodID methodID = (*env)->GetStaticMethodID(env, clazz, "onExecuted", "(I)V");
    if (methodID == NULL) {
        LOGE("---------------methodID isNULL---------------");
        return;
    }
    //调用该java方法
    (*env)->CallStaticVoidMethod(env, clazz, methodID, ret);
}

/**
 * 回调执行Java方法
 * 参看 Jni反射+Java反射
 */
void callJavaMethodonProgress(JNIEnv *env, jclass clazz, float ret) {
    if (clazz == NULL) {
        LOGE("---------------clazz isNULL---------------");
        return;
    }
    //获取方法ID (I)V指的是方法签名 通过javap -s -public FFmpegCmd 命令生成
    jmethodID methodID = (*env)->GetStaticMethodID(env, clazz, "onProgress", "(F)V");
    if (methodID == NULL) {
        LOGE("---------------methodID isNULL---------------");
        return;
    }
    //调用该java方法
    (*env)->CallStaticVoidMethod(env, clazz, methodID, ret);
}

/**
 * c语言-线程回调
 * 如果是 C 主线程回调 java 函数，不必从 JVM 取出当前线程的 JNIEnv; 直接 callJavaMethod 即可
 */
static void ffmpeg_callback(int ret) {
    JNIEnv *env;
    //附加到当前线程从JVM中取出JNIEnv, C/C++从子线程中直接回到Java里的方法时  必须经过这个步骤
    (*jvm)->AttachCurrentThread(jvm, (void **) &env, NULL);

    callJavaMethodOnExcuted(env, m_clazz, ret);

    //完毕-脱离当前线程
    (*jvm)->DetachCurrentThread(jvm);
}

/**
 * c语言-线程回调
 * 如果是 C 主线程回调 java 函数，不必从 JVM 取出当前线程的 JNIEnv; 直接 callJavaMethod 即可
 */
static void log_callback(float ret) {
    JNIEnv *env;
    //附加到当前线程从JVM中取出JNIEnv, C/C++从子线程中直接回到Java里的方法时  必须经过这个步骤
    (*jvm)->AttachCurrentThread(jvm, (void **) &env, NULL);

    callJavaMethodonProgress(env, m_clazz, ret);

    //完毕-脱离当前线程
    (*jvm)->DetachCurrentThread(jvm);
}

JNIEXPORT jint JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_exec(JNIEnv *env, jclass clazz, jobjectArray commands) {

    //---------------------------------C语言 反射Java 相关----------------------------------------
    //在jni的c线程中不允许使用共用的env环境变量 但JavaVM在整个jvm中是共用的 可通过保存JavaVM指针,到时候再通过JavaVM指针取出JNIEnv *env;
    //ICS之前(你可把NDK sdk版本改成低于11) 可以直接写m_clazz = clazz;直接赋值,  然而ICS(sdk11) 后便改变了这一机制,在线程中回调java时 不能直接共用变量 必须使用NewGlobalRef创建全局对象
    //官方文档正在拼命的解释这一原因,参看:http://android-developers.blogspot.jp/2011/11/jni-local-reference-changes-in-ics.html
    /*
    参数:jobject obj的解释:
     https://blog.csdn.net/likuan0214/article/details/52671546
    如果 native方法不是static的话，这个obj就代表 native方法所在类 的一个实例对象 的引用，要获取 jclass 需要通过 jclass clazz = (*env)->GetObjectClass(env, instance);
    如果 native方法 是 static的话，这个obj就代表 native方法所在类 的class对象实例 jclass (static方法不需要类实例的，所以就代表这个类的class对象)
     */

    (*env)->GetJavaVM(env, &jvm);

//    jclass clazz = (*env)->GetObjectClass(env, instance);
    m_clazz = (*env)->NewGlobalRef(env, clazz);
    //---------------------------------C语言 反射Java 相关----------------------------------------


    int i, result = 0;
    int argc = 0;
    char **argv = NULL;
    if (commands != NULL) {
    //---------------------------------java 数组转C语言数组----------------------------------------
        argc = (*env)->GetArrayLength(env, commands);
        argv = (char **) malloc(sizeof(char *) * argc);

        for (i = 0; i < argc; i++) {
            jstring js = (jstring) (*env)->GetObjectArrayElement(env, commands, i);
            argv[i] = (char *) (*env)->GetStringUTFChars(env, js, 0);
        }

    //---------------------------------java 数组转C语言数组----------------------------------------


    //---------------------------------执行FFmpeg命令相关----------------------------------------
        //设置进度回调
        syslog_callback(log_callback);
        //注册ffmpeg命令执行完毕时的回调
        ffmpeg_thread_callback(ffmpeg_callback);
        //新建线程 执行ffmpeg 命令
        result = ffmpeg_thread_run_cmd(argc, argv);
    //---------------------------------执行FFmpeg命令相关----------------------------------------

        return result;
    }

    return 0;
}



JNIEXPORT void JNICALL
Java_com_huyu_ffmpeg_FFmpegCmd_setDebug(JNIEnv *env, jclass clazz, jboolean writeLogcat,
                                        jboolean writeLogFile) {

    WRITE_LOGCAT = writeLogcat;
    WRITE_LOGFILE = writeLogFile;

}