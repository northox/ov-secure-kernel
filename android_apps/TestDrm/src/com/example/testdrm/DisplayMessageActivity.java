package com.example.testdrm;

import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.widget.TextView;
import android.widget.VideoView;
//import android.support.v4.app.NavUtils;
//import android.support.v4;

public class DisplayMessageActivity extends Activity implements 
									OnCompletionListener, OnPreparedListener{

	private VideoView videoPlayer;
	
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_message);
        startVideo();
    }
    
    public void startVideo() {
    	videoPlayer = (VideoView)findViewById(R.id.videoPlayer);   
	    videoPlayer.setOnPreparedListener(this);
	    videoPlayer.setOnCompletionListener(this);
	    videoPlayer.setKeepScreenOn(true);    
	    videoPlayer.setVideoPath(TestDrm.videoLocation);
    }
    
    
    public void onPrepared(MediaPlayer vp) {
        
        // Don't start until ready to play.  The arg of seekTo(arg) is the start point in
        // milliseconds from the beginning. In this example we start playing 1/5 of
        // the way through the video if the player can do forward seeks on the video.
        
        if(videoPlayer.canSeekForward()) videoPlayer.seekTo(videoPlayer.getDuration()/5);
        videoPlayer.start();
     }
     
     /** This callback will be invoked when the file is finished playing */
     @Override
     public void onCompletion(MediaPlayer  mp) {
    	 setContentView(R.layout.activity_display_message);
    	 startVideo();
     }
     
     /**  Use screen touches to toggle the video between playing and paused. */
     @Override
     public boolean onTouchEvent (MotionEvent ev){	
        if(ev.getAction() == MotionEvent.ACTION_DOWN){
           if(videoPlayer.isPlaying()){
                    videoPlayer.pause();
           } else {
                    videoPlayer.start();
           }
           return true;		
        } else {
           return false;
        }
     }
}
