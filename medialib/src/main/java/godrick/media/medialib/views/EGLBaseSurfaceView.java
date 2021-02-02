package godrick.media.medialib.views;

import android.content.Context;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.lang.ref.WeakReference;

public abstract class EGLBaseSurfaceView extends SurfaceView implements SurfaceHolder.Callback {


    private Surface surface;

    public EGLBaseSurfaceView(Context context) {
        this(context, null);
    }

    public EGLBaseSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public EGLBaseSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }


    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (surface == null) {
            surface = holder.getSurface();
        }
        //start glthread
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    static class GLThread extends Thread {

        private WeakReference<EGLBaseSurfaceView> reference;

        public GLThread(WeakReference<EGLBaseSurfaceView> reference) {
            this.reference = reference;
        }

        @Override
        public void run() {
            super.run();


        }
    }

}
