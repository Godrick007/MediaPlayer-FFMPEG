package godrick.media.player;

import android.os.Bundle;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import godrick.media.medialib.natives.NativeMediaEnter;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView tv = findViewById(R.id.tv);
//        tv.setText(NativeMediaEnter.getInstance().nTest() + " = " + NativeMediaEnter.getInstance().nVersionTest());
        NativeMediaEnter.getInstance().prepare("hahahahaha");
    }
}