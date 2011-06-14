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

import java.io.File;
import java.io.FileOutputStream;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.Gravity;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.LinearLayout;

import com.admob.android.ads.AdListener;
import com.admob.android.ads.AdManager;
import com.admob.android.ads.AdView;

public class MainActivity extends Activity implements AdListener {

	public boolean firstRun = true;

	public static MainActivity currentInstance = null;

	public static NativeLib mNativeLib;

	private static SharedPreferences settings = null;

	private float dScale = 1.0f;

	private int dWidth, dHeight;

	private boolean dRotate = false;

	private static FrameLayout mFrameLayout = null;

	private static MainView mMainView = null;
	private static LinearLayout mOverlayView = null;
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

		AdManager.setTestDevices(new String[] {AdManager.TEST_EMULATOR});

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

		dScale = getApplicationContext().getResources().getDisplayMetrics().density;

		mFrameLayout = new FrameLayout(getApplicationContext());

		mMainView = new MainView(getApplicationContext());

		mOverlayView = new LinearLayout(getApplicationContext());
		mOverlayView.setLayoutParams(new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
		mOverlayView.setFocusable(false);
		mOverlayView.setFocusableInTouchMode(false);
		mOverlayView.setGravity(Gravity.BOTTOM|Gravity.CENTER_HORIZONTAL);

		setOverlayView(mMainView.gameMode);

		mFrameLayout.addView(mMainView,
				new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
		mFrameLayout.addView(mOverlayView,
				new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

		setContentView(mFrameLayout,
				new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

		mMainView.requestFocus();

	}

	@Override
	protected void onResume() {
		super.onResume();
		if (mMainView != null)
			mMainView.onResume();
		mSensorManager.registerListener(mSensorListener, mSensor, SensorManager.SENSOR_DELAY_FASTEST);
	}

	@Override
	protected void onPause() {
		super.onPause();
		if (mMainView != null)
			mMainView.onPause();
		this.onDestroy();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();

		SharedPreferences.Editor editor = settings.edit();
		editor.putInt("soundEnabled", NativeLib.soundEnabled);
		editor.putInt("videoQuality", NativeLib.videoQuality);
		editor.putInt("viewMode", NativeLib.viewMode);
		editor.commit();

		NativeLib.OnStopMusic();
		NativeLib.UnloadSounds();

		mSensorManager.unregisterListener(mSensorListener); 

		System.exit(0);
	}

	public void requestExit() {
		this.onDestroy();
	}

	private void updateTV(float x, float y) {

		if (mMainView.gameMode == NativeLib.RACING) {

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
			if (mAdView == null) {
				mAdView = new AdView(this);
				mAdView.setAdListener(this);

				mOverlayView.removeAllViews();

				mOverlayView.addView(mAdView,
						new LayoutParams((int) (320*dScale+0.5f), (int) (48*dScale+0.5f)));
			}
			if (gameMode != NativeLib.SPLASH)
				mOverlayView.setBackgroundDrawable(getResources().getDrawable(R.drawable.transparent));
			else
				mOverlayView.setBackgroundDrawable(getResources().getDrawable(R.drawable.splash));
			break;
		default:
			mOverlayView.removeAllViews();
			if (mAdView != null) {
				mAdView = null;
			}
			mOverlayView.setBackgroundDrawable(getResources().getDrawable(R.drawable.transparent));
			break;
		}
	}

	public void InstallFiles() {

		final AssetManager mAssetManager = getApplication().getResources().getAssets();

		try {
			String dir = NativeLib.DATA_DIR + "/";

			File fdir = new File( dir );
			if (!fdir.exists())
				fdir.mkdirs();

			ZipInputStream zs = new ZipInputStream(mAssetManager.open("files.zip", AssetManager.ACCESS_BUFFER));

			ZipEntry item;
			while( (item = zs.getNextEntry())!=null ) {

				if( item.isDirectory() ) {
					File newdir = new File( dir + item.getName() );
					if (!newdir.exists())
						newdir.mkdir();
				}
				else {
					File newfile = new File( dir + item.getName() );
					long filesize = item.getSize();
					if (newfile.exists() && newfile.length() == filesize)
						continue;
					byte[] tempdata = new byte[(int)filesize];
					int offset = 0;
					while (offset<filesize)
						offset += zs.read(tempdata, offset, (int)filesize-offset);
					zs.closeEntry();
					newfile.createNewFile();
					FileOutputStream fo = new FileOutputStream(newfile);
					fo.write(tempdata);
					fo.close();
				}
			}

			zs.close();   
		}
		catch(Exception e)
		{
			//noop
		}
	}

	public void onFailedToReceiveAd(AdView arg0) {
		// TODO Auto-generated method stub

	}

	public void onFailedToReceiveRefreshedAd(AdView arg0) {
		// TODO Auto-generated method stub

	}

	public void onReceiveAd(AdView arg0) {
		// TODO Auto-generated method stub

	}

	public void onReceiveRefreshedAd(AdView arg0) {
		// TODO Auto-generated method stub

	}

}
