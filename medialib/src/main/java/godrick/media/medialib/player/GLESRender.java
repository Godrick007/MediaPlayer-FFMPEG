package godrick.media.medialib.player;

import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import godrick.media.medialib.natives.NativeMediaEnter;

public class GLESRender implements GLSurfaceView.Renderer {


    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.e("MediaPlayer", "onSurfaceCreated");
        NativeMediaEnter.getInstance().rendererInit();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.e("MediaPlayer", String.format("width = $d, height = %d", width, height));
        NativeMediaEnter.getInstance().rendererResize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        Log.e("MediaPlayer", "onDrawFrame");
        NativeMediaEnter.getInstance().rendererDrawFrame();
    }
}
