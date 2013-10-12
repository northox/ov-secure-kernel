package com.example.testov;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;

public class displayResultActivity extends Activity {
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        Intent intent = getIntent();
        String input_param = intent.getStringExtra(TestOVActivity.EXTRA_MESSAGE);
        // Create the text view
        
        TextView textView = new TextView(this);
        textView.setText(input_param);
        setContentView(textView);
    }
	
	@Override
    protected void onPause() {
        super.onPause();
        // Another activity is taking focus (this activity is about to be "paused").
    }

}
