package godrick.media.medialib.player;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.SeekBar;


import androidx.annotation.NonNull;

import godrick.media.medialib.itfs.IMediaPlayerListener;
import godrick.media.medialib.natives.NativeMediaEnter;

public class NativeGLSurfaceView extends SurfaceView implements SurfaceHolder.Callback, IMediaPlayerListener {

    private static long progress;

    NativeMediaEnter engine;
    SeekBar sb;

    long duration;

    Handler handler = new Handler() {


        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);


        }
    };


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
        Log.e("_tag", String.format("current is %d, duration is %d", current, duration));
        Message m = Message.obtain();
        this.duration = duration;
        int c = (int) (current * 1000000 / duration);
        m.arg1 = c;
        m.what = 10086;
        Log.e("_tag", String.format("current is %d, duration is %d, c is %d", current, duration,c));
        handler.sendMessage(m);

        sb.setProgress(m.arg1);

    }

    public void setSeekBar(SeekBar sb) {
        this.sb = sb;
        this.sb.setMax(1000000);
        this.sb.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                long p = seekBar.getProgress() * duration / 1000000;
                if (engine != null) {
                    engine.seek(p);
                }
            }
        });
    }
}
