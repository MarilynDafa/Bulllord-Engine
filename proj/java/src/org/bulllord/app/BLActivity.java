package org.bulllord.app;
import android.app.NativeActivity;
public class BLActivity extends NativeActivity {
	static {
        System.loadLibrary("bulllord");
    }
}
