package godrick.media.player;

import android.Manifest;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import java.io.File;

import godrick.media.medialib.player.NativeGLSurfaceView;


public class MainActivity extends AppCompatActivity implements View.OnClickListener {


    private NativeGLSurfaceView mp;

    @RequiresApi(api = Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
//        tv.setText(NativeMediaEnter.getInstance().nTest() + " = " + NativeMediaEnter.getInstance().nVersionTest());


//        mp = findViewById(R.id.video);

//        NativeMediaEnter.getInstance().setRequestRenderListener(new IRequestRenderListener() {
//            @Override
//            public void requestRenderer() {
//                mp.requestRender();
//            }
//        });

        mp = findViewById(R.id.video);

        SeekBar sb = findViewById(R.id.sb);

        mp.setSeekBar(sb);

        Button btnPrepare = findViewById(R.id.btn_prepare);
        Button btnPause = findViewById(R.id.btn_pause);
        Button btnResume = findViewById(R.id.btn_resume);
        Button btnStart = findViewById(R.id.btn_start);
        Button btnStop = findViewById(R.id.btn_stop);
        Button btnSpeed20 = findViewById(R.id.btn_speed20);


        btnPause.setOnClickListener(this);
        btnResume.setOnClickListener(this);
        btnPrepare.setOnClickListener(this);
        btnStart.setOnClickListener(this);
        btnStop.setOnClickListener(this);
        btnSpeed20.setOnClickListener(this);

        requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 0);

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_prepare:
                final File file = new File("sdcard/tree.mp4");
//                final File file = new File("sdcard/aaaa.wmv");
//                final File file = new File("sdcard/bbbb.mp4");
//                final File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/aaaa.mp4");
                if (!file.exists()) {
                    Log.e("MediaPlayer", "file is not exists");
                    return;
                }

//                String[] ss = MediaUtil.getMediaCodecSupport(file.getAbsolutePath());
//
//                Log.e("", "");

                mp.prepare(file.getAbsolutePath());

//                NativeMediaEnter.getInstance().prepare(file.getAbsolutePath(), false);
//                mp.setRenderer(new GLESRender());
//                new Handler(Looper.getMainLooper()).postDelayed(new Runnable() {
//                    @Override
//                    public void run() {
//
//                    }
//                }, 1000);

                break;
            case R.id.btn_start:
//                NativeMediaEnter.getInstance().start();
                mp.start();
                break;
            case R.id.btn_stop:
//                NativeMediaEnter.getInstance().stop();
                mp.stop();
                break;
            case R.id.btn_speed20:
//                NativeMediaEnter.getInstance().setPlaySpeed(2.0f);
//                NativeMediaEnter.isSupportCodec("");
                break;

            case R.id.btn_pause:
//                NativeMediaEnter.getInstance().pause();
                mp.pause();
                break;
            case R.id.btn_resume:
//                NativeMediaEnter.getInstance().resume();
                mp.resume();
                break;
        }
    }
}