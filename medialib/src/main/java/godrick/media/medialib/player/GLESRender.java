package godrick.media.medialib.player;

import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.Surface;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import godrick.media.medialib.natives.NativeMediaEnter;

public class GLESRender implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {

    private long count = 0;
    private OnRenderListener onRenderListener;
    private OnSurfaceCreatedListener onSurfaceCreatedListener;

    public void setOnRenderListener(OnRenderListener onRenderListener) {
        this.onRenderListener = onRenderListener;
    }

    public void setOnSurfaceCreatedListener(OnSurfaceCreatedListener onSurfaceCreatedListener) {
        this.onSurfaceCreatedListener = onSurfaceCreatedListener;
    }

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
//        Log.e("MediaPlayer", "onDrawFrame");
//        Log.e("java_count", "On draw frame " + count++);

        NativeMediaEnter.getInstance().rendererDrawFrame();
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        if (onRenderListener != null) {
            onRenderListener.onRender();
        }
    }

    public interface OnSurfaceCreatedListener {
        void onSurfaceCreated(Surface surface);
    }

    public interface OnRenderListener {
        void onRender();
    }
}
