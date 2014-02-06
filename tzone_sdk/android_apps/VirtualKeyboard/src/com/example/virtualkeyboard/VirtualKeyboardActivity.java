package com.example.virtualkeyboard;

import android.app.Activity;
import android.os.Bundle;
import android.widget.ImageButton;
import android.widget.Toast;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.MotionEvent;
import android.content.pm.ActivityInfo;
import java.util.Timer;
import java.util.TimerTask;
import android.os.Handler;

public class VirtualKeyboardActivity extends Activity {
	private static native String VirtualKeyboard();
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_virtualkeyboard);
 		
		this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

		ImageButton imageButton = (ImageButton) findViewById(R.id.imageButton1);

		imageButton.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
			final Handler handler = new Handler(); 
		    Timer t = new Timer(); 
		    t.schedule(new TimerTask() { 
		            public void run() { 
		                    handler.post(new Runnable() { 
		                            public void run() { 
										nativeVirtualKeyboard();
						   				setContentView(R.layout.payment_complete);
		                            } 
		                    }); 
		            } 
		    }, 500); 
			}
		});

		imageButton.setOnTouchListener(new View.OnTouchListener() {
    	    @Override
        	public boolean onTouch(View v, MotionEvent event) {
            	if (event.getAction() == MotionEvent.ACTION_DOWN){
					float x = event.getX(), y = event.getY();
					if((110 <= x) && (780 <= y) && (495 >= x) && (890 >= y)) {
//						nativeVirtualKeyboard();
//        				setContentView(R.layout.payment_complete);
					}
				}

				return false; 
			}
    	}); 
	}

	public static void nativeVirtualKeyboard() {
		System.loadLibrary("virtualkeyboard");
		VirtualKeyboard();
	}

}
