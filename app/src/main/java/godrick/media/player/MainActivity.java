package godrick.media.player;

import android.Manifest;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import java.io.File;

import godrick.media.medialib.natives.NativeMediaEnter;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    @RequiresApi(api = Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
//        tv.setText(NativeMediaEnter.getInstance().nTest() + " = " + NativeMediaEnter.getInstance().nVersionTest());


        Button btnPrepare = findViewById(R.id.btn_prepare);
        Button btnStart = findViewById(R.id.btn_start);
        Button btnStop = findViewById(R.id.btn_stop);

        btnPrepare.setOnClickListener(this);
        btnStart.setOnClickListener(this);
        btnStop.setOnClickListener(this);

        requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 0);

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_prepare:
                final File file = new File("sdcard/aaaa.mp4");
//                final File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/aaaa.mp4");
                if (!file.exists()) {
                    Log.e("MediaPlayer", "file is not exists");
                    return;
                }
                NativeMediaEnter.getInstance().prepare(file.getAbsolutePath());
                break;
            case R.id.btn_start:
                NativeMediaEnter.getInstance().start();
                break;
            case R.id.btn_stop:
                NativeMediaEnter.getInstance().stop();
                break;
        }
    }
}