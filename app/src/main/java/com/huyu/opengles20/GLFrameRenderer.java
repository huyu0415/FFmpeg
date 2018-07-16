package com.huyu.opengles20;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.util.DisplayMetrics;
import android.util.Log;

import java.nio.ByteBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;


/**
 * 把数据贴到GLSurfaceVIew上

 1.Surface create的时候，我初始化了一些需要用到的Program/Shader，因为马上就要用到它们了；

 2.Surface change的时候，重置一下画面；

 3.onDrawFrame()时，把数据真正地“画”上去；

 4.至于两个update方法，是用来把图像的宽高/数据传过来的。
 */
public class GLFrameRenderer implements Renderer {

    private ISimplePlayer mParentAct;
    private GLSurfaceView mTargetSurface;
    private GLProgram prog = new GLProgram(0);
    private int mScreenWidth, mScreenHeight;
    private int mVideoWidth, mVideoHeight;
    private ByteBuffer y;
    private ByteBuffer u;
    private ByteBuffer v;

    public GLFrameRenderer(ISimplePlayer callback, GLSurfaceView surface, DisplayMetrics dm) {
        mParentAct = callback;
        mTargetSurface = surface;
        mScreenWidth = dm.widthPixels;
        mScreenHeight = dm.heightPixels;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.d("huyu","GLFrameRenderer :: onSurfaceCreated");
        if (!prog.isProgramBuilt()) {
            prog.buildProgram();
			Log.d("huyu","GLFrameRenderer :: buildProgram done");
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
		Log.d("huyu","GLFrameRenderer :: onSurfaceChanged");
        GLES20.glViewport(0, 0, width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        synchronized (this) {
            if (y != null) {
                // reset position, have to be done
                y.position(0);
                u.position(0);
                v.position(0);
                prog.buildTextures(y, u, v, mVideoWidth, mVideoHeight);
                GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
                prog.drawFrame();
            }
        }
    }

    /**
     * this method will be called from native code, it happens when the video is about to play or
     * the video size changes.
     */
    public void update(int w, int h) {
		Log.d("huyu","INIT E from native code w * h = " + w + " * " + h);
        if (w > 0 && h > 0) {
            // 调整比例
            if (mScreenWidth > 0 && mScreenHeight > 0) {
                float f1 = 1f * mScreenHeight / mScreenWidth;
                float f2 = 1f * h / w;
                if (f1 == f2) {
                    prog.createBuffers(GLProgram.squareVertices);
                } else if (f1 < f2) {
                    float widScale = f1 / f2;
                    prog.createBuffers(new float[] { -widScale, -1.0f, widScale, -1.0f, -widScale, 1.0f, widScale,
                            1.0f, });
                } else {
                    float heightScale = f2 / f1;
                    prog.createBuffers(new float[] { -1.0f, -heightScale, 1.0f, -heightScale, -1.0f, heightScale, 1.0f,
                            heightScale, });
                }
            }
            // 初始化容器
            if (w != mVideoWidth && h != mVideoHeight) {
                this.mVideoWidth = w;
                this.mVideoHeight = h;
                int yarraySize = w * h;
                int uvarraySize = yarraySize / 4;
                synchronized (this) {
                    y = ByteBuffer.allocate(yarraySize);
                    u = ByteBuffer.allocate(uvarraySize);
                    v = ByteBuffer.allocate(uvarraySize);
                }
            }
        }

        mParentAct.onPlayStart();
		Log.d("huyu","INIT X from native code");
    }

    /**
     * this method will be called from native code, it's used for passing yuv data to me.
     */
    public void update(byte[] ydata, byte[] udata, byte[] vdata) {
        synchronized (this) {
            y.clear();
            u.clear();
            v.clear();
            y.put(ydata, 0, ydata.length);
            u.put(udata, 0, udata.length);
            v.put(vdata, 0, vdata.length);
        }

        // request to render
        mTargetSurface.requestRender();
    }

    /**
     * this method will be called from native code, it's used for passing play state to activity.
     */
    public void updateState(int state) {
		Log.d("huyu","updateState E = " + state);
        if (mParentAct != null) {
            mParentAct.onReceiveState(state);
        }
		Log.d("huyu","updateState X");
    }
}
