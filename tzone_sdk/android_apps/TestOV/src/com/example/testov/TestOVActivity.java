package com.example.testov;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class TestOVActivity extends Activity {

	public final static String EXTRA_MESSAGE = "com.example.testov.MESSAGE";

	public final static String EXTRA_MESSAGE2 = "com.example.testov.MESSAGE";
	String str = ""; 
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_ov);
        
}


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_test_ov, menu);
        return true;
    }

    /** Called when the user selects the Send button */
    public void sendParamToOV(View view) {
        // Do something in response to button
    	EditText editText = (EditText) findViewById(R.id.input_param);
    	String paramToOV = editText.getText().toString();
    	Intent intent2 = new Intent(this, JniInterface.class);
    	intent2.putExtra(EXTRA_MESSAGE2, paramToOV);
    	startActivityForResult(intent2,1);
    	//String outputFromOV = JniInterface.testNativeOV(paramToOV);
    	
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
      if (resultCode == RESULT_OK && requestCode == 1) {
    	  
    	  String value = data.getExtras().getString(EXTRA_MESSAGE);
    	  data = new Intent(this, displayResultActivity.class);
    	  data.putExtra(EXTRA_MESSAGE, value);
    	  startActivity(data);
      }
    }
}

