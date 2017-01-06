/**
 * Copyright (c) 2015-2016 VisionStar Information Technology (Shanghai) Co., Ltd. All Rights Reserved.
 * EasyAR is the registered trademark or trademark of VisionStar Information Technology (Shanghai) Co., Ltd in China
 * and other countries for the augmented reality technology developed by VisionStar Information Technology (Shanghai) Co., Ltd.
 */

package cn.easyar.samples.helloarvideo;

import android.content.res.Configuration;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;

import java.text.SimpleDateFormat;
import java.util.Timer;
import java.util.TimerTask;

import cn.easyar.engine.EasyAR;


public class MainActivity extends ActionBarActivity {

	/*
	* Steps to create the key for this sample:
	*  1. login www.easyar.com
	*  2. create app with
	*      Name: HelloARVideo
	*      Package Name: cn.easyar.samples.helloarvideo
	*  3. find the created item in the list and show key
	*  4. set key string bellow
	*/
	static String key = "8T0e2nqQBYnQihPmQPrCLqg54xQNlrllfSXpSKdV0TZebsZ31uAyfhxGPebKDEWSmyGlH7amg70y1QQH3hZXSs9Yp6LVVG8beQwa5ac4ff6dd47b25de4a12217a44fca84bY2Of1SvpRXlZ5BXuPSyNm9Jz9DFU0itmIuNC160dknZsFoqRnB8zsNDe4yojz2asGoTZ";

	static {
		System.loadLibrary("EasyAR");
		System.loadLibrary("HelloARVideoNative");
	}

	private FrameLayout mPreviewFrameLayout;
	private ImageView mVideoPlayImageView;
	private ImageView mVideoRestartImageView;
	private SeekBar mVideoSeekbarSeekBar;
	private TextView mVideoCurrentTextView;

	public static native void nativeInitGL();

	public static native void nativeResizeGL(int w, int h);

	public static native void nativeRender();

	private native boolean nativeInit();

	private native void nativeVideoSeek(int pos);

	private native void nativeVideoPlay();

	private native void nativeVideoPause();

	private native int nativeGetDuration();

	private native int nativeGetCurrentPosition();

	private native void nativeDestory();

	private native void nativeRotationChange(boolean portrait);

	private TimerTask task = new TimerTask() {
		@Override
		public void run() {
			Log.d("AR", "1秒钟获取");
			Log.d("AR", "总时长：" + nativeGetDuration());
			Log.d("AR", "当前时长：" + nativeGetCurrentPosition());
			if (nativeGetDuration() != 0) {
				if (nativeGetDuration() != mVideoSeekbarSeekBar.getMax()) {
					mVideoSeekbarSeekBar.setMax(nativeGetDuration());
				}
				if (nativeGetCurrentPosition() != mVideoSeekbarSeekBar.getProgress()) {
					runOnUiThread(new Runnable() {
						@Override
						public void run() {
							mVideoSeekbarSeekBar.setProgress(nativeGetCurrentPosition());
							mVideoCurrentTextView.setText(getTime(nativeGetCurrentPosition()) + ":" + getTime(nativeGetDuration()));
						}
					});
				}
			}
		}
	};

	private String getTime(int ms) {
		SimpleDateFormat formatter = new SimpleDateFormat("mm:ss");//初始化Formatter的转换格式。
		String hms = formatter.format(ms);
		return hms;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		mPreviewFrameLayout = (FrameLayout) findViewById(R.id.preview);
		mVideoPlayImageView = (ImageView) findViewById(R.id.iv_video_play);
		mVideoRestartImageView = (ImageView) findViewById(R.id.iv_video_restart);
		mVideoSeekbarSeekBar = (SeekBar) findViewById(R.id.sk_video_seekbar);
		mVideoCurrentTextView = (TextView) findViewById(R.id.tv_video_current);
		mVideoSeekbarSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				if (fromUser) {
					mVideoCurrentTextView.setText(getTime(progress) + ":" + getTime(nativeGetDuration()));
					nativeVideoSeek(progress);
				}
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {

			}

			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {

			}
		});


		getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		EasyAR.initialize(this, key);
		nativeInit();
		Timer timer = new Timer();
		timer.schedule(task, 0, 1000);


		GLView glView = new GLView(this);
		glView.setRenderer(new Renderer());
		glView.setZOrderMediaOverlay(true);

		((ViewGroup) findViewById(R.id.preview)).addView(glView, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
		nativeRotationChange(getWindowManager().getDefaultDisplay().getRotation() == android.view.Surface.ROTATION_0);
	}


	public void play(View v) {
		//播放
		if("play".equals(v.getTag())){
			//当前是播放状态
			nativeVideoPause();
			mVideoPlayImageView.setTag("pause");
			mVideoPlayImageView.setImageResource(R.mipmap.iconfont_video_pause);
		}else{
			//是暂停状态
			mVideoPlayImageView.setTag("play");
			mVideoPlayImageView.setImageResource(R.mipmap.iconfont_video_play);
			nativeVideoPlay();
		}
	}

	public void restart(View v) {
		//重新播放
		nativeVideoSeek(0);
	}


	@Override
	public void onConfigurationChanged(Configuration config) {
		super.onConfigurationChanged(config);
		nativeRotationChange(getWindowManager().getDefaultDisplay().getRotation() == android.view.Surface.ROTATION_0);
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		nativeDestory();
	}

	@Override
	protected void onResume() {
		super.onResume();
		EasyAR.onResume();
	}

	@Override
	protected void onPause() {
		super.onPause();
		EasyAR.onPause();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.menu_main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		int id = item.getItemId();

		if (id == R.id.action_settings) {
			return true;
		}

		return super.onOptionsItemSelected(item);
	}
}
