package godrick.media.medialib.natives;

import android.util.Log;
import android.view.Surface;

public class NativeMediaEnter {

    static {
        System.loadLibrary("MediaPlayer-FFMPEG");
        System.loadLibrary("avcodec-58");
        System.loadLibrary("avdevice-58");
        System.loadLibrary("avfilter-7");
        System.loadLibrary("avformat-58");
        System.loadLibrary("avutil-56");
        System.loadLibrary("postproc-55");
        System.loadLibrary("swresample-3");
        System.loadLibrary("swscale-5");
        System.loadLibrary("x264");
    }

    private static NativeMediaEnter instance;

    private NativeMediaEnter() {
    }

    public synchronized static NativeMediaEnter getInstance() {
        if (instance == null) {
            instance = new NativeMediaEnter();
        }
        return instance;
    }


    //======================java methods =====================

    public void setSurface(Surface surface) {
        nSetSurface(surface);
    }

    public void prepare(String url) {
        nPrepare(url);
    }

    public void start() {
        nStart();
    }

    public void pause() {
        nPause();
    }

    public void resume() {
        nResume();
    }

    public void stop() {
        nStop();
    }

    public void release() {
        nRelease();
    }

    public void setPlaySpeed(float speed) {
        nSetPlaySpeed(speed);
    }

    //======================java methods end =================


    //======================ndk methods ======================


    private native void nSetSurface(Surface surface);

    private native void nPrepare(String url);

    private native void nStart();

    private native void nPause();

    private native void nResume();

    private native void nStop();

    private native void nRelease();

    private native void nSetPlaySpeed(float speed);

    //======================ndk methods end ==================


    //======================ndk callback 2 java ===================

    public void cb_OnJniLoadFailed() {
        Log.e("MediaPlayer", "cb_OnJniLoadFailed");
    }

    public void cb_JniMethodRegisterError() {
        Log.e("MediaPlayer", "cb_JniMethodRegisterError");
    }

    public void cb_MediaPlayerInitError(int errorCode, String msg) {
        Log.e("MediaPlayer", "cb_MediaPlayerInitError code = " + errorCode + " ------ msg = " + msg);
    }

    public void cb_MediaPlayerPrepared() {
        Log.e("MediaPlayer", "cb_MediaPlayerPrepared");
    }

    public void cb_MediaPlayerComplete() {
        Log.e("MediaPlayer", "cb_MediaPlayerComplete");
    }

    public void cb_MediaPlayerLoading(boolean isLoading) {
        Log.e("MediaPlayer", "cb_MediaPlayerLoading = " + isLoading);
    }

    public void cb_MediaPlayerProgress(long current, long duration) {
        Log.e("MediaPlayer", "cb_MediaPlayerProgress current is " + current + "   duration is " + duration);
    }

    public void cb_MediaPlayerDBValue(double dbValue) {
        Log.e("MediaPlayer", "cb_MediaPlayerDBValue = " + dbValue);
    }

    public void cb_MediaPlayerSLInitError(int errorCode, String msg) {
        Log.e("MediaPlayer", "cb_MediaPlayerSLInitError code = " + errorCode + " ------ msg = " + msg);
    }

    //======================ndk callback 2 java end ===============

}
