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
import android.media.MediaPlayer;
import android.net.Uri;


public class AudioClip {
	
	private MediaPlayer mPlayer;
	private String name;
	
	private boolean mPlaying = false;
	private boolean mLoop = false;

	public AudioClip(Context ctx, Uri uri) {
		name = uri.toString();
		
		mPlayer = MediaPlayer.create(ctx, uri);
		mPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener(){

			public void onCompletion(MediaPlayer mp) {
				mPlaying = false;
				if ( mLoop) {
					mp.start();
				}
			}
			
		});
	}
	
	public synchronized void play () {
		if ( mPlaying) 
			return;
		
		if (mPlayer != null ) {
			mPlaying = true;
			mPlayer.start();
		}
	}
	
	public synchronized void stop() {
		try {
			mLoop = false;
			if ( mPlaying ) { 
				mPlaying = false;
				mPlayer.pause();
			}			
		} catch (Exception e) {
		}
	}
	
	public synchronized void loop () {
		mLoop = true;
		mPlaying = true;
		mPlayer.start();		
	}
	
	public void release () {
		if (mPlayer != null) { 
			mPlayer.release();
			mPlayer = null;
		}
	}
	
	public String getName() {
		return name;
	}
	
	public void setVolume (int vol) {
		if ( mPlayer != null) {
			if (vol>128) vol=128;
			mPlayer.setVolume(((float)vol)/128.0f, ((float)vol)/128.0f);
		}
	}
}
