package godrick.media.medialib.itfs;

public interface IMediaPlayerListener {

    void onInitError(int code, String msg);

    void onPrepare();

    void onComplete();

    void onLoading(boolean loading);

    void onProgress(long current, long duration);

}
