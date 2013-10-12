package com.example.testov;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

public class JniInterface extends Activity {
	public final static String EXTRA_MESSAGE = "com.example.testov.MESSAGE";
	private static native String testOV(String parameter);
	public static String outputfromOV;
	@Override
	public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Intent intent2 = getIntent();
        String input_param = intent2.getStringExtra(TestOVActivity.EXTRA_MESSAGE2);
        outputfromOV = testNativeOV(input_param);
        intent2.putExtra(EXTRA_MESSAGE, outputfromOV);
        setResult(RESULT_OK,intent2);
        super.finish();
         }
	
	public static String testNativeOV(String parameter) {
		System.loadLibrary("test");
		return(testOV(parameter));
	}    
}
