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

import android.content.Context;


public class NativeLib {
	
	static {
		System.loadLibrary("tuxrider");
	}

	public static String DATA_DIR = "/sdcard/com.drodin.tuxrider";

	public static final String [] sound_names = new String[] {
		"rock_sound",
		"snow_sound",
		"hit_tree",
		"ice_sound",
		"item_collect",
		"flying_sound",
	};

	public static final int WSK_UP = 1;
	public static final int WSK_DOWN = 2;
	public static final int WSK_RIGHT = 3;
	public static final int WSK_LEFT = 4;
	public static final int WSK_JUMP = 'e';
	public static final int WSK_QUIT = 'q';
	public static final int WSK_VIEW = '0';

	public static final boolean WSK_PRESSED = false;
	public static final boolean WSK_RELEASED = true;

	public static final boolean WSK_NONSPECIAL = false;
	public static final boolean WSK_SPECIAL = true;

	public static final int NO_MODE = 		   -1;
	public static final int SPLASH = 			0;
	public static final int GAME_TYPE_SELECT =	1;
	public static final int EVENT_SELECT = 		2;
	public static final int RACE_SELECT = 		3;
	public static final int LOADING = 			4;
	public static final int INTRO = 			5;
	public static final int RACING = 			6;
	public static final int GAME_OVER = 		7;
	public static final int PAUSED = 			8;
	public static final int RESET = 			9;
	public static final int CREDITS = 			10;
	public static final int HELP=				11;
	public static final int PREFERENCE=			12;

	public static native void resize(int width, int height);

	public static native int render(
			int touchX, int touchY, double turnFact, int keyCode, boolean keySpecial, boolean keyReleased);

	public static int soundEnabled;
	public static int videoQuality;
	public static int viewMode;
	
	private static MainAudioManager mAudioMgr;
	public NativeLib(Context context) {
		mAudioMgr = MainAudioManager.getInstance(context);
		try {
			if ( mAudioMgr != null)
				mAudioMgr.preloadSounds();
		} catch (Exception e) { }
	}
	
	public static void UnloadSounds() {
		try {
			if ( mAudioMgr != null)
				mAudioMgr.unloadSounds();
		} catch (Exception e) { }
	}

	public static void OnStartSound(String name, int loop) {
		try {
			if ( mAudioMgr != null)
				mAudioMgr.startSound( name, loop);
		} catch (Exception e) { }
	}

	public static void OnStopSound(String name) {
		try {
			if ( mAudioMgr != null)
				mAudioMgr.stopSound(name);
		} catch (Exception e) { }
	}

	public static void OnVolumeSound(String name, int vol) {
		try {
			if ( mAudioMgr != null)
				mAudioMgr.setSoundVolume(name, vol);
		} catch (Exception e) { }
	}

	public static void OnStartMusic(String name, int loop) {
		try {
			if ( mAudioMgr != null)
				mAudioMgr.startMusic(name, loop);
		} catch (Exception e) { }

	}
	
	public static void OnStopMusic() {
		try {
			if ( mAudioMgr != null)
				mAudioMgr.stopMusic();
		} catch (Exception e) { }

	}

}
