package godrick.media.medialib.natives;

import android.annotation.SuppressLint;
import android.media.MediaCodecList;
import android.util.Log;
import android.view.Surface;

import godrick.media.medialib.itfs.IRequestRenderListener;

public class NativeMediaEnter {

    static {
        System.loadLibrary("MediaPlayer-FFMPEG");
        System.loadLibrary("soundtouch");
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

    private IRequestRenderListener iRequestRenderListener;

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

    public void prepare(String url, boolean isLiving) {
        nPrepare(url, isLiving);
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

    public void rendererInit() {
        nRendererInitialize();
    }

    public void rendererResize(int width, int height) {
        nRendererResize(width, height);
    }

    public void rendererDrawFrame() {
        nRendererDrawFrame();
    }

    //======================java methods end =================


    //======================ndk methods ======================


    private native void nSetSurface(Surface surface);

    private native void nPrepare(String url, boolean isLiving);

    private native void nStart();

    private native void nPause();

    private native void nResume();

    private native void nStop();

    private native void nRelease();

    private native void nSetPlaySpeed(float speed);

    private native void nRendererInitialize();

    private native void nRendererResize(int width, int height);

    private native void nRendererDrawFrame();


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
//        nRendererInitialize();
    }

    public void cb_MediaPlayerComplete() {
        Log.e("MediaPlayer", "cb_MediaPlayerComplete");
    }

    public void cb_MediaPlayerLoading(boolean isLoading) {
//        Log.e("MediaPlayer", "cb_MediaPlayerLoading = " + isLoading);
    }

    public void cb_MediaPlayerProgress(long current, long duration) {
//        Log.e("MediaPlayer", "cb_MediaPlayerProgress current is " + current + "   duration is " + duration);
    }

    public void cb_MediaPlayerDBValue(double dbValue) {
        Log.e("MediaPlayer", "cb_MediaPlayerDBValue = " + dbValue);
    }

    public void cb_MediaPlayerSLInitError(int errorCode, String msg) {
        Log.e("MediaPlayer", "cb_MediaPlayerSLInitError code = " + errorCode + " ------ msg = " + msg);
    }

    public void cb_MediaRequestRenderer() {
        if (iRequestRenderListener != null) {
            iRequestRenderListener.requestRenderer();
        }
    }

    //======================ndk callback 2 java end ===============


    @SuppressLint("DefaultLocale")
    public static boolean isSupportCodec(String codecName) {

        boolean supportCode = false;

        int count = MediaCodecList.getCodecCount();

        for (int i = 0; i < count; i++) {

            String[] types = MediaCodecList.getCodecInfoAt(i).getSupportedTypes();

            for (int j = 0; j < types.length; j++) {

//                if(types[j].equals(findVideoCodecName(codecName))){
//                    supportCode = true;
//                    break;
//                }

                Log.e("MediaPlayer", String.format("%s", types[j]));

            }

            if (supportCode) {
                break;
            }

        }


        return supportCode;
    }

    public void setRequestRenderListener(IRequestRenderListener iRequestRenderListener) {
        this.iRequestRenderListener = iRequestRenderListener;
    }
}
