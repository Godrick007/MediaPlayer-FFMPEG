package godrick.media.medialib.natives;

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

    public void prepare(String url) {
        nPrepare(url);
    }

    //======================java methods end =================


    //======================ndk methods ======================


    private native void nPrepare(String url);


    //======================ndk methods end ==================


}
