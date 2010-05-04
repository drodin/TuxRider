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
import java.util.HashMap;

import android.content.Context;
import android.net.Uri;


public class MainAudioManager {

	static private MainAudioManager am ;

	private static Context mContext;

	private volatile HashMap<String, AudioClip> mSounds = new HashMap<String, AudioClip>();

	private AudioClip music;

	static public MainAudioManager getInstance(Context ctx) {
		if ( am == null) return new MainAudioManager(ctx);
		return am;
	}

	private MainAudioManager(Context ctx) {
		mContext = ctx;
	}

	public void preloadSounds() {

		try {

			File folder = new File(NativeLib.DATA_DIR + "/sounds");

			if ( !folder.exists()) {
				return;
			}

			File[] files =  new File[NativeLib.sound_names.length]; 

			for (int i = 0; i < files.length; i++ ) { 
				files[i] = new File(folder +  File.separator +  NativeLib.sound_names[i] + ".ogg");

				if ( files[i].exists()) {
					mSounds.put(NativeLib.sound_names[i], new AudioClip(mContext, Uri.fromFile(files[i])));
				}

			}

		} catch (Exception e) { }
	}

	public void unloadSounds() {
		for(int i=0; i<NativeLib.sound_names.length; i++) {
			String key = NativeLib.sound_names[i];
			if (mSounds.containsKey(key)) {
				if (mSounds.get(key) != null) {
					mSounds.get(key).stop();
					mSounds.get(key).release();
				}
			}
		}
	}

	public synchronized void startSound( String key, int loop ) { 
		if ( mSounds.containsKey(key)) {
			if (loop != 0)
				mSounds.get(key).loop();
			else
				mSounds.get(key).play();
		}
	}

	public synchronized void stopSound( String key ) { 
		if ( mSounds.containsKey(key)) {
			mSounds.get(key).stop();
		}
	}

	public void setSoundVolume ( String key, int vol ) {
		if ( mSounds.containsKey(key)) {
			mSounds.get(key).setVolume(vol);
		}
	}

	public void startMusic (String key, int loop) {
		File folder = new File(NativeLib.DATA_DIR + "/music");
		File sound = new File(folder +  File.separator + key + ".mp3");

		if ( !sound.exists()) {
			if ( music != null) {
				music.stop();
				music.release();
				music = null;
			}
			return;
		}

		if ( music != null) {
			if (music.getName().equals((Uri.fromFile( sound )).toString()))
				return;
			music.stop();
			music.release();
		}

		music = new AudioClip(mContext, Uri.fromFile( sound ));

		music.setVolume(128);
		if ( loop != 0 ) 
			music.loop();
		else
			music.play();
	}

	public void stopMusic () {
		if ( music != null  ) {
			music.stop();
			music.release();
			music = null;
		}
	}

	public void setMusicVolume (int vol) {
		if ( music != null)
			music.setVolume(vol);
	}

}
