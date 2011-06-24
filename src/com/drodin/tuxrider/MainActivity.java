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
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.widget.FrameLayout;

import com.google.ads.AdRequest;
import com.google.ads.AdSize;
import com.google.ads.AdView;

public class MainActivity extends Activity {

	public static MainActivity currentInstance = null;

	public static NativeLib mNativeLib;

	private static SharedPreferences settings = null;

	private int dWidth, dHeight;

	private boolean dRotate = false;

	private static FrameLayout mFrameLayout = null;

	private static MainView mMainView = null;
	private static AdView mAdView = null;

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

		dWidth = getWindowManager().getDefaultDisplay().getWidth();
		dHeight = getWindowManager().getDefaultDisplay().getHeight();
		if (dHeight>dWidth) {
			dRotate = true;
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		}

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

		mFrameLayout = new FrameLayout(getApplicationContext());

		mMainView = new MainView(getApplicationContext());

		mAdView = new AdView(this, AdSize.BANNER, "a14d3678cfc9fb7");
		
		AdRequest adRequest = new AdRequest();
		adRequest.addTestDevice(AdRequest.TEST_EMULATOR);

		mAdView.loadAd(adRequest);
		mAdView.setGravity(Gravity.BOTTOM|Gravity.CENTER_HORIZONTAL);

		setOverlayView(mMainView.gameMode);

		mFrameLayout.addView(mMainView,
				new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
		mFrameLayout.addView(mAdView,
				new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

		setContentView(mFrameLayout,
				new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

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
				if (dRotate)
					updateTV(-accValueY/accSamples, accValueX/accSamples);
				else
					updateTV(accValueX/accSamples, accValueY/accSamples);
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
			if (mAdView != null) {
				mAdView.setVisibility(View.VISIBLE);
			}
			break;
		default:
			if (mAdView != null) {
				mAdView.setVisibility(View.INVISIBLE);
			}
			break;
		}
	}
	
}
