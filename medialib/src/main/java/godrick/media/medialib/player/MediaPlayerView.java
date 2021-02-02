package godrick.media.medialib.player;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import godrick.media.medialib.natives.NativeMediaEnter;
import godrick.media.medialib.utils.MediaUtil;

public class MediaPlayerView extends GLSurfaceView {

    private String playUrl;

    private GLESRender render;

    public MediaPlayerView(Context context) {
        this(context, null);
    }

    public MediaPlayerView(Context context, AttributeSet attrs) {
        super(context, attrs);

        init();

    }


    private void init() {
        setEGLContextClientVersion(2);

        render = new GLESRender();

        setRenderer(render);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        render.setOnRenderListener(this::requestRender);
    }

    public void setMediaUrl(String url, boolean isLiving) {

        this.playUrl = url;
        String mimes[] = MediaUtil.getMediaCodecSupport(url);

        if (mimes.length >= 2) {//hw


        } else {                //sw
//            NativeMediaEnter.getInstance().prepare(url, isLiving);
        }


    }

}
