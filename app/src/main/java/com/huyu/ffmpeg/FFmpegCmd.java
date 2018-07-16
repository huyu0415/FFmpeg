package com.huyu.ffmpeg;

import android.os.Environment;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Locale;
import java.util.Scanner;

/**
 * ___                      _
 * / __|   ___   ___   ___   | |  ___
 * | (_--- / _ \ / _ \ / _ \  | | /___)
 * \____| \___/ \___/ \___/| |_| \___
 * /
 * \___/
 * Description:
 * Date: 2018/4/27
 * Author: Huyu
 * Success is getting what you want; happiness is wanting what you get.
 */
public class FFmpegCmd {

	private static OnExecListener listener;

	static
	{
//		System.loadLibrary("avcodec-57");
//		System.loadLibrary("avfilter-6");
//		System.loadLibrary("avformat-57");
//		System.loadLibrary("avutil-55");
//		System.loadLibrary("swresample-2");
//		System.loadLibrary("swscale-4");

		System.loadLibrary("ffmpeg_jni");
	}

	private static native int exec(String[] cmd);

	public static native void setDebug(boolean writeLogcat, boolean writeLogFile);

	public native String printInfo();

	/**将其他封装格式 解码成 yuv420 格式*/
	public native int decode2YUV420(String input, String output_yuv);

	public native int encodeYuv2H264(String input, String output);

	public native void startPlay(String input, Object surface);

	public native void resumePlay();

	public native void pausePlay();

	public native void stopPlay();

	public native void playFilterVideo(String input, String filters, Object surface);

	/******直播流*******************************/
	/**初始化*/
	public native void cameraLiveInit(int width, int height, String url);

	/**开始推流，参数传入相机的NV21数据*/
	public native void cameraLiveStart(byte[] cameraData);

	/**停止推流*/
	public native void cameraLiveStop();







	public static int exec(String cmd, OnExecListener listener) {
		FFmpegCmd.listener = listener;
		final String[] split = cmd.split("[ \\t]+");
		return exec(split);
	}

	/**
	 * 执行ffmoeg命令
	 */
	public static void exec(String[] cmds, OnExecListener listener) {
		FFmpegCmd.listener = listener;
		exec(cmds);
	}

	/**
	 * 等待 jni 反射回调
	 */
	public static void onExecuted(int ret) {
		if (listener != null) {
			if(ret == 0)
				listener.onExecutedSuccess();
			else
				listener.onExecutedError(ret);
		}
	}

	/**
	 * 等待 jni 反射回调
	 */
	public static void onProgress(float ret) {
		if (listener != null) {
			listener.onExecutedProgress(ret);
		}
	}


	/**
	 * 遍历 "system/etc/vold.fstab” 文件，获取全部的Android的挂载点信息
	 */
	private static ArrayList<String> getDevMountList() {
		ArrayList<String> out = new ArrayList<>();
		out.add("/mnt/sdcard");
		try {
			File file = new File("/etc/vold.fstab");
			if(file.exists()){
				Scanner scanner = new Scanner(file);
				while (scanner.hasNext()){
					String line = scanner.nextLine();
					if(line.startsWith("dev_mount")){
						String element = line.split(" ")[2];
						if(element.contains(":"))
							element = element.substring(0, element.indexOf(":"));
						if(!element.equals("/mnt/sdcard"))
							out.add(element);
					}
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		return out;
	}

	/**
	 * 获取扩展SD卡存储目录
	 * 如果有外接的SD卡，并且已挂载，则返回这个外置SD卡目录
	 * 否则：返回内置SD卡目录
	 */
	public static String getExternalSdCardPath() {

		if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
			return Environment.getExternalStorageDirectory().getPath();
		}

		String path;
		ArrayList<String> devMountList = getDevMountList();

		for (String devMount : devMountList) {
			File file = new File(devMount);

			if (file.isDirectory() && file.canWrite()) {
				path = file.getAbsolutePath();

				String timeStamp = new SimpleDateFormat("ddMMyyyy_HHmmss", Locale.CHINA).format(new Date());
				File testWritable = new File(path, "test_" + timeStamp);

				if (testWritable.mkdirs()) {
					testWritable.delete();
					return path;
				}
			}
		}
		return null;
	}

}
