package godrick.media.medialib.player;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import godrick.media.medialib.itfs.IMediaPlayerListener;
import godrick.media.medialib.natives.NativeMediaEnter;

public class NativeGLSurfaceView extends SurfaceView implements SurfaceHolder.Callback, IMediaPlayerListener {

    NativeMediaEnter engine;


    public NativeGLSurfaceView(Context context) {
        this(context, null);
    }

    public NativeGLSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public NativeGLSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        engine = NativeMediaEnter.getInstance();
        engine.setiMediaPlayerListener(this);
        getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

        Log.i("java_tag", "surfaceCreated");

        if (engine == null) {
            engine = NativeMediaEnter.getInstance();
        }
        engine.onSurfaceCreate(holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.i("java_tag", "surfaceChanged");
        if (engine != null) {
            engine.onSurfaceChanged(width, height);
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    public void prepare(String url) {
        if (engine != null) {
            engine.prepare(url, false);
        }
    }

    public void start() {
        if (engine != null) {
            engine.start();
        }
    }

    public void stop() {
        if (engine != null) {
            engine.stop();
        }
    }

    public void pause() {
        if (engine != null) {
            engine.pause();
        }
    }

    public void resume() {
        if (engine != null) {
            engine.resume();
        }
    }


    @Override
    public void onInitError(int code, String msg) {

    }

    @Override
    public void onPrepare() {

    }

    @Override
    public void onComplete() {

    }

    @Override
    public void onLoading(boolean loading) {

    }

    @Override
    public void onProgress(long current, long duration) {

    }
}
