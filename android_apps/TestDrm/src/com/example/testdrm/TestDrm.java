package com.example.testdrm;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;

import android.content.ContentValues;
import android.content.Intent;
import java.lang.reflect.Method;

import android.view.MotionEvent;
import android.view.View;
import android.widget.EditText;
import android.widget.VideoView;

import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.os.Bundle;
import android.app.Activity;
import android.drm.DrmInfo;
import android.drm.DrmInfoRequest;
import android.drm.DrmInfoStatus;
import android.drm.DrmManagerClient;
import android.drm.DrmRights;
import android.drm.DrmStore;
import android.drm.ProcessedData;
import android.view.Menu;
import android.widget.TextView;


public class TestDrm extends Activity  {

	public static final String EXTRA_MESSAGE = "com.example.testdrm.MESSAGE";
    String drmEngines = "";
    public static final String videoLocation = "/system/media/Amazing-Spiderman.mp4";
    public static final String rightsPath = "/system/lib/drm/drm.rights";
      /*public static final String videoLocation = "/data/Amazing-Spiderman.mp4";
      public static final String rightsPath = "/data/drm.rights";*/
   
 public static final String mimeType = "application/video.*.*";
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_drm);
        DrmManagerClient drm_manager_client = new DrmManagerClient(this);
        String[] engines = drm_manager_client.getAvailableDrmEngines();
        for (String engine : engines) {
            drmEngines += engine + "\n";
        }
        String message = "";
        if(drmEngines.equals("")) {
        	message = "No Drm Engines available \n";
        } else {
        	message = "Available Drm Engines are -- \n";
        	message += drmEngines;
        	message += "\n\n";
        	int retVal =  checkDrmInfo(drm_manager_client);
        	if(retVal != 0) {
        		message += "Video content not authorized successfully \n";
        		drmEngines = "";
        	} else {
        		message += "Video content authorized successfully \n";
            	message += "Click on the " +"Start Video !" +
            							 "button to start video playback \n";
        	}
        }
        TextView textBox = (TextView)findViewById(R.id.textView1);
        textBox.setText(message);
    }
    
    public void sendMessage(View view) {
    	if(drmEngines.equals("")) {
    		return;
    	}
        Intent intent = new Intent(this, DisplayMessageActivity.class);
        startActivity(intent);
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_test_drm, menu);
        return true;
    }
    
    public int checkDrmInfo(DrmManagerClient drm_manager_client) {
        DrmInfoRequest drmInfoRequest;
        int infoType = DrmInfoRequest.TYPE_RIGHTS_ACQUISITION_INFO; 
        drmInfoRequest = new DrmInfoRequest(infoType,TestDrm.mimeType);
        DrmInfo drmInfo = drm_manager_client.acquireDrmInfo(drmInfoRequest);
        drm_manager_client.processDrmInfo(drmInfo);
        DrmRights content_rights = new DrmRights(TestDrm.rightsPath,TestDrm.mimeType);
        try {
        	drm_manager_client.saveRights(content_rights,
        							TestDrm.rightsPath,TestDrm.videoLocation);
        }
        catch(IOException err) {
        	
        }
        return(0);
    }
}
