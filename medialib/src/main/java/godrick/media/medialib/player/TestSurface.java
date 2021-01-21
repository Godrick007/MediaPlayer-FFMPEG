package godrick.media.medialib.player;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import godrick.media.medialib.natives.NativeMediaEnter;

public class TestSurface extends SurfaceView implements SurfaceHolder.Callback {


    public TestSurface(Context context) {
        this(context, null);
    }

    public TestSurface(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public TestSurface(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        SurfaceHolder holder = getHolder();
        holder.addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        NativeMediaEnter.getInstance().eglStart(holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }
}
