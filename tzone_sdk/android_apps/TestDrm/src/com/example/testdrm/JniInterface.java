package com.example.testdrm;

public class JniInterface {
	private static native String testDrm(String parameter);
	private static native String initDrmEngine();
	
	public static String testNativeDrm(String parameter) {
		return(testDrm(parameter));
	}
	
	public static String initNativeDrmEngine() {
		return(initDrmEngine());
	}
}
