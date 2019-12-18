/*

   Copyright (c) 2010 Dmitry Rodin

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

   Author contact information:

   E-mail: rodin.dmitry@gmail.com

 */

package com.drodin.tuxrider;


import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.net.Uri;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.Gravity;
import android.graphics.Color;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageButton;

public class MainActivity extends Activity {

	public static MainActivity currentInstance = null;

	public static NativeLib mNativeLib;

	private static SharedPreferences settings = null;

	private static MainView mMainView = null;
	private ImageButton mAppView = null;

	public SensorManager mSensorManager = null; 
	public List<Sensor> mSensors = null; 
	public Sensor mSensor = null;

	private int accSamples = 4;
	private int accCounter = accSamples;
	private float accValueX = 0;
	private float accValueY = 0; 
	private final float kXMax = 4.5f;
	private final float kXSensibility = 1f;
	private final float kYDown = 8f;
	private final float kYUp = 7f;
	private boolean trackingLeft = false;
	private boolean trackingRight = false;
	private boolean trackingUp = false;
	private boolean trackingDown = false;

	@Override
	protected void onCreate(Bundle icicle) {
		super.onCreate(icicle);

		currentInstance = this;

		getWindow().setFlags(
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		mNativeLib = new NativeLib(getApplicationContext());

		settings = PreferenceManager.getDefaultSharedPreferences(this);
		NativeLib.soundEnabled = settings.getInt("soundEnabled", 1);
		NativeLib.videoQuality = settings.getInt("videoQuality", 1);
		NativeLib.viewMode = settings.getInt("viewMode", 3);

		mSensorManager = (SensorManager)getSystemService(Context.SENSOR_SERVICE); 
		mSensors = mSensorManager.getSensorList(Sensor.TYPE_ACCELEROMETER); 
		if(mSensors !=null && mSensors.size() > 0) { 
			mSensor = mSensors.get(0); 
		}

		FrameLayout mFrameLayout = new FrameLayout(getApplicationContext());

		mMainView = new MainView(getApplicationContext());

		mAppView = new ImageButton(this);
		mAppView.setBackgroundColor(Color.TRANSPARENT);
		mAppView.setImageResource(R.drawable.icon); //TODO: set proper image
		mAppView.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Intent playMarket = new Intent(Intent.ACTION_VIEW, Uri.parse("market://details?id=com.drodin.tuxrider"));
				startActivity(playMarket);
			}
		});

		mFrameLayout.addView(mMainView,
				new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
		mFrameLayout.addView(mAppView,
				new FrameLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT, Gravity.CENTER_HORIZONTAL|Gravity.BOTTOM));

		setContentView(mFrameLayout,
				new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));

		mMainView.requestFocus();
	}

	@Override
	protected void onResume() {
		super.onResume();

		mSensorManager.registerListener(mSensorListener, mSensor, SensorManager.SENSOR_DELAY_FASTEST);

		if (mMainView != null)
			mMainView.onResume();
	}

	@Override
	protected void onPause() {
		super.onPause();

		NativeLib.OnStopMusic();
		NativeLib.OnStopAllSounds();

		if (mMainView.gameMode == NativeLib.RACING)
			mMainView.keyboardFunction(NativeLib.WSK_PAUSE, NativeLib.WSK_NONSPECIAL, NativeLib.WSK_PRESSED);

		mSensorManager.unregisterListener(mSensorListener); 

		if (mMainView != null)
			mMainView.onPause();

		SharedPreferences.Editor editor = settings.edit();
		editor.putInt("soundEnabled", NativeLib.soundEnabled);
		editor.putInt("videoQuality", NativeLib.videoQuality);
		editor.putInt("viewMode", NativeLib.viewMode);
		editor.commit();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();

		NativeLib.UnloadSounds();

		System.exit(0);
	}

	public void requestExit() {
		this.onDestroy();
	}

	private void updateTV(float x, float y) {

		if (mMainView != null && mMainView.gameMode == NativeLib.RACING) {

			if (x>kXSensibility) {
				if (x>kXMax) mMainView.turnFact = -1.0;
				else mMainView.turnFact = -x/kXMax;
				if (trackingRight) {
					mMainView.keyboardFunction(NativeLib.WSK_RIGHT, NativeLib.WSK_SPECIAL, NativeLib.WSK_RELEASED);
					trackingRight = false;
				}
				if (!trackingLeft) {
					mMainView.keyboardFunction(NativeLib.WSK_LEFT, NativeLib.WSK_SPECIAL, NativeLib.WSK_PRESSED);
					trackingLeft = true;
				}
			} else if (x<-kXSensibility) {
				if (x<-kXMax) mMainView.turnFact = +1.0;
				else mMainView.turnFact = -x/kXMax;
				if (trackingLeft) {
					mMainView.keyboardFunction(NativeLib.WSK_LEFT, NativeLib.WSK_SPECIAL, NativeLib.WSK_RELEASED);
					trackingLeft = false;
				}
				if(!trackingRight) {
					mMainView.keyboardFunction(NativeLib.WSK_RIGHT, NativeLib.WSK_SPECIAL, NativeLib.WSK_PRESSED);
					trackingRight = true;
				}
			} else {
				mMainView.turnFact = 0.0;
				if (trackingLeft) {
					mMainView.keyboardFunction(NativeLib.WSK_LEFT, NativeLib.WSK_SPECIAL, NativeLib.WSK_RELEASED);
					trackingLeft = false;
				}
				if (trackingRight) {
					mMainView.keyboardFunction(NativeLib.WSK_RIGHT, NativeLib.WSK_SPECIAL, NativeLib.WSK_RELEASED);
					trackingRight = false;
				}
			}

			if (Math.abs(y)>kYDown) {
				if (trackingUp) {
					mMainView.keyboardFunction(NativeLib.WSK_UP, NativeLib.WSK_SPECIAL, NativeLib.WSK_RELEASED);
					trackingUp = false;
				}
				if (!trackingDown) {
					mMainView.keyboardFunction(NativeLib.WSK_DOWN, NativeLib.WSK_SPECIAL, NativeLib.WSK_PRESSED);
					trackingDown = true;
				}
			} else if (Math.abs(y)<kYUp) {
				if (trackingDown) {
					mMainView.keyboardFunction(NativeLib.WSK_DOWN, NativeLib.WSK_SPECIAL, NativeLib.WSK_RELEASED);
					trackingDown = false;
				}
				if(!trackingUp) {
					mMainView.keyboardFunction(NativeLib.WSK_UP, NativeLib.WSK_SPECIAL, NativeLib.WSK_PRESSED);
					trackingUp = true;
				}
			} else {
				if (trackingDown) {
					mMainView.keyboardFunction(NativeLib.WSK_DOWN, NativeLib.WSK_SPECIAL, NativeLib.WSK_RELEASED);
					trackingDown = false;
				}
				if (trackingUp) {
					mMainView.keyboardFunction(NativeLib.WSK_UP, NativeLib.WSK_SPECIAL, NativeLib.WSK_RELEASED);
					trackingUp = false;
				}
			}

		}

	} 

	private final SensorEventListener mSensorListener = new SensorEventListener() { 
		public void onSensorChanged(SensorEvent event) { 
			accCounter--;
			accValueX += event.values[0];
			accValueY += event.values[1];

			if (accCounter==0) {
				updateTV(-accValueY/accSamples, accValueX/accSamples);
				accValueX = accValueY = 0;
				accCounter = accSamples;
			}
		} 

		public void onAccuracyChanged(Sensor sensor, int accuracy) {} 
	}; 

	public void setOverlayView(int gameMode) {
		switch (gameMode) {
		case NativeLib.SPLASH:
		case NativeLib.GAME_TYPE_SELECT:
		case NativeLib.EVENT_SELECT:
		case NativeLib.RACE_SELECT:
		case NativeLib.PREFERENCE:
		case NativeLib.LOADING:
			//TODO: remove
			/*if (mAppView != null) {
				mAppView.setVisibility(View.VISIBLE);
			}
			break;*/
		default:
			if (mAppView != null) {
				mAppView.setVisibility(View.INVISIBLE);
			}
			break;
		}
	}

}
