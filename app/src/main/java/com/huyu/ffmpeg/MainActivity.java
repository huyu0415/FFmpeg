package com.huyu.ffmpeg;

import android.app.ProgressDialog;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.yanzhenjie.permission.Action;
import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.Permission;

import java.io.File;
import java.util.List;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, SurfaceHolder.Callback, TextureView.SurfaceTextureListener {

	private FFmpegCmd mFFmpeg;

	private String videoFile, yuvFile, h264File;
	private ProgressDialog pDialog;
	private SurfaceView surfaceView;
	private TextureView textureView;
	private final String RTSP_URL = "rtsp://192.168.2.155:554/Streaming/Channels/101?transportmode=unicast";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		initViews();
		FFmpegCmd.setDebug(true, false);
		pDialog = new ProgressDialog(this);
		pDialog.setMessage("loading...");

		AndPermission.with(this).runtime().permission(Permission.Group.STORAGE).start();
	}

	private void initViews() {
		Button play = findViewById(R.id.play);
		Button pause = findViewById(R.id.pause);
		Button resume = findViewById(R.id.resume);
		Button stop = findViewById(R.id.stop);
		Button filters = findViewById(R.id.filters);
		Button cmd = findViewById(R.id.cmd);
		Button translate2yuv = findViewById(R.id.translate2yuv);

		String root = FFmpegCmd.getExternalSdCardPath();

		if (null == root)
			root = "/storage/emulated/0";

		videoFile = root + "/Movies/test.mp4";
		h264File = root + "/Movies/test.264";
		yuvFile = root + "/Movies/test.yuv";

		mFFmpeg = new FFmpegCmd();

		/* surfaceview */
		surfaceView = findViewById(R.id.surfaceView);
		SurfaceHolder surfaceHolder = surfaceView.getHolder();
		surfaceHolder.addCallback(this);

		/*  textureView  */
		textureView = findViewById(R.id.textureView);
		textureView.setSurfaceTextureListener(this);

		play.setOnClickListener(this);
		pause.setOnClickListener(this);
		resume.setOnClickListener(this);
		stop.setOnClickListener(this);
		filters.setOnClickListener(this);
		cmd.setOnClickListener(this);
		translate2yuv.setOnClickListener(this);
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {

			case R.id.play:
//				textureView.setRotation(45);
				new Thread() {
					@Override
					public void run() {
						// surfaceView
						mFFmpeg.startPlay(h264File, surfaceView.getHolder().getSurface());
						// textureView
						// ffmpeg.startPlay(h264File, new Surface(textureView.getSurfaceTexture()));
					}
				}.start();
				break;

			case R.id.pause:
				mFFmpeg.pausePlay();
				break;

			case R.id.resume:
				mFFmpeg.resumePlay();
				break;

			case R.id.stop:
				mFFmpeg.stopPlay();
				break;

			case R.id.filters:
				//const char *filter_descr = "boxblur=2:1:cr=0:ar=0";                   // 模糊处理 参数依次为亮度 2:1  色度 0， 透明度 0

				//const char *filter_descr = "lutyuv='u=128:v=128'";                    // 变成灰色
				//const char *filter_descr = "hflip";                                   // 水平翻转  vflip  上下镜像   hflip 左右镜像
				//const char *filter_descr = "hue='h=60:s=-3'";                         // 色相饱和度  h 色度角度  s 饱和度 -10 和 10 之间，默认1
				//const char *filter_descr = "crop=320:240:100:200";                    // 裁剪后尺寸 320 * 240， 裁剪区域的左上角坐标 100，200
				//const char *filter_descr = "scale=320:240";                           // 缩放为 320 * 240
				//const char *filter_descr = "transpose=dir=1:passthrough=none";    // 旋转  顺时针旋转 1 * 90°， 保持竖直方向   passthrough： 默认 none， 可选 portrait， landscape

				//const char *filter_descr = "drawbox=x=100:y=100:w=200:h=100:color=pink@0.5:t=2";      // 矩形水印，矩形左上角坐标 100，100 ，宽高 200*100，颜色 pink, 不透明度0.5, 线宽 2
				//const char *filter_descr = "drawgrid=width=100:height=100:thickness=2:color=red@0.5";      // 网格水印，width 和 height 表示添加网格的宽和高，thickness表示网格的线宽，color表示颜色
				//const char *filter_descr = "movie=/storage/emulated/0/ic_launcher.png[wm];[in][wm]overlay=5:5:1,scale=1920:1080[out]";
					//图片水印, 位置坐标为 5，5  第三个参数 为1，表示支持透明水印
					//	main_w		视频单帧图像宽度		左上角	10:10
					//	main_h		视频单帧图像高度		右上角	main_w-overlay_w-10:10
					//	overlay_w	水印图片的宽度		左下角	10:main_h-overlay_h-10
					//	overlay_h	水印图片的高度		右下角	main_w-overlay_w-10:main_h-overlay_h-10

				//const char *filter_descr = "[in]drawtext=fontfile=/storage/emulated/0/Movies/arial.ttf:fontcolor=red:fontsize=50:x=100:y=10:text='huyu':shadowy=2[out]";  // 文字水印 x:y是显示位置  fontfile=/home/code/simhei.ttf 可以写绝对路径

				//const char *filter_descr = "subtitles=filename=huyu.srt";     // 硬字幕，将 huyu.srt 字幕文件 合成到视频流
				//const char *filter_descr = "subtitles=filename=huyu.srt:force_style='FontName=DejaVu Serif,PrimaryColour=&HAA00FF00'";     // 硬字幕，将 huyu.srt 字幕文件 合成到视频流，字幕字体 DejaVu Serif， 颜色 &HAA00FF00
				//const char *filter_descr = "subtitles=video1.mkv";            // 硬字幕，将输入文件 video1 的第一个字幕流合成到视频流中
				//const char *filter_descr = "subtitles=video1.mkv:si=1";       // 硬字幕，将输入文件 video1 的第二个字幕流合成到另一个容器的视频流中输出：

//				mFFmpeg.playFilterVideo(videoFile, "movie=/storage/emulated/0/Movies/ic_launcher.png[wm];[in][wm]overlay=5:5:1[out]", surfaceView.getHolder().getSurface());
				mFFmpeg.playFilterVideo(videoFile, "[in]drawtext=fontfile=/storage/emulated/0/Movies/arial.ttf:fontcolor=red:fontsize=50:x=100:y=10:shadowy=2:text='huyu'[out]", surfaceView.getHolder().getSurface());
				break;

			case R.id.translate2yuv:
//				new Thread() {
//					@Override
//					public void run() {
						if (new File(videoFile).exists()) {
							File file = new File(yuvFile);
							if (file.exists())
								file.delete();

							int result;
							result = mFFmpeg.decode2YUV420(videoFile, yuvFile);
//							result = mFFmpeg.encodeYuv2H264(yuvFile, yuvh264);
							Log.i("huyu_MainActivity_run", result >= 0 ? "封装格式 转换 yuv 完成" : "封装格式 转换 yuv 失败");

						} else
							Log.e("huyu_MainActivity_run", videoFile + " 不存在");
//					}
//				}.start();
				break;

			case R.id.cmd:
				AndPermission.with(this)
						.runtime()
						.permission(Permission.Group.STORAGE)
						.onGranted(new Action<List<String>>() {
							@Override
							public void onAction(List<String> data) {
								File file = new File(h264File);
								if (file.exists())
									file.delete();

								Log.i("huyu_initViews", videoFile + (new File(videoFile).exists() ? " 存在" : " 不存在"));
								String command =
										String.format("ffmpeg -i %s -vcodec h264 -s 720x1280 -an -f mp4 %s", videoFile, h264File)
										;
								pDialog.show();
								final long time = System.currentTimeMillis();
								FFmpegCmd.exec(command, new OnExecListener() {
									@Override
									public void onExecutedProgress(final float result) {
										runOnUiThread(new Runnable() {
											@Override
											public void run() {
												pDialog.setMessage("Progress : " + result);
											}
										});
									}

									@Override
									public void onExecutedError(int result) {
										Log.e("MainActivity_onExecuted", "error = " + result);
									}

									@Override
									public void onExecutedSuccess() {
										runOnUiThread(new Runnable() {
											@Override
											public void run() {
												Log.e("MainActivity_onExecuted", "success--耗时(s)：" + ((System.currentTimeMillis() - time)/1.0f/1000));
												pDialog.dismiss();
												Toast.makeText(MainActivity.this, "Execute Successfully", Toast.LENGTH_SHORT).show();
											}
										});
									}
								});
							}
						})
						.onDenied(new Action<List<String>>() {
							@Override
							public void onAction(List<String> data) {
								Toast.makeText(MainActivity.this, "权限拒绝", Toast.LENGTH_SHORT).show();
							}
						})
						.start();
				break;

			default:
				break;
		}
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {

	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {

	}


	@Override
	public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int i, int i1) {

	}

	@Override
	public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int i, int i1) {

	}

	@Override
	public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
		return false;
	}

	@Override
	public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) {

	}
}
