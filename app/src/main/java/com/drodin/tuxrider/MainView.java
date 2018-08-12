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

import java.util.ArrayList;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class MainView extends GLSurfaceView {

	public int gameMode = 0;
	public int prevMode = 0;

	private MainActivity mMainActivity;

	private volatile int touchX = -1;
	private volatile int touchY = -1;

	private ArrayList<KeyState> keyQueue = new ArrayList<>();

	public volatile double turnFact;

	public MainView(Context context) {
		super(context);

		mMainActivity = MainActivity.currentInstance;

		setId((int)(Math.random()*100));

		setFocusable(true);
		setFocusableInTouchMode(true);

		setRenderer(new MainRenderer());
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
	}

	@Override
	public boolean onTouchEvent (MotionEvent event) {
		if (event.getAction() == MotionEvent.ACTION_DOWN) {
			touchX = Math.round(event.getX());
			touchY = Math.round(event.getY());
			if (gameMode == NativeLib.RACING)
				keyboardFunction(NativeLib.WSK_JUMP, NativeLib.WSK_NONSPECIAL, NativeLib.WSK_PRESSED);
		} else if (event.getAction() == MotionEvent.ACTION_UP && gameMode == NativeLib.RACING)
			keyboardFunction(NativeLib.WSK_JUMP, NativeLib.WSK_NONSPECIAL, NativeLib.WSK_RELEASED);
		return true;
	}

	@Override
	public boolean dispatchKeyEventPreIme (KeyEvent event) {
		if (event.getRepeatCount() == 0) {
			int keyCode = event.getKeyCode();
			boolean keyReleased = (event.getAction()==KeyEvent.ACTION_DOWN)?
					NativeLib.WSK_PRESSED:NativeLib.WSK_RELEASED;

			switch (keyCode) {
			case KeyEvent.KEYCODE_BACK:
				keyboardFunction(NativeLib.WSK_QUIT, NativeLib.WSK_NONSPECIAL, keyReleased);
				return true;
			case KeyEvent.KEYCODE_FOCUS:
			case KeyEvent.KEYCODE_CAMERA:
			case KeyEvent.KEYCODE_MENU:
				if (gameMode == NativeLib.RACING)
					keyboardFunction(NativeLib.WSK_VIEW, NativeLib.WSK_NONSPECIAL, keyReleased);
				return true;
			default:
				return false;
			}
		} else
			return true;
	}

	public void keyboardFunction(int key, boolean special, boolean state) {
		keyQueue.add(new KeyState(key, special, state));
	}

	public class MainRenderer implements GLSurfaceView.Renderer {

		public void onDrawFrame(GL10 gl) {
			int keyCode = -1;
			boolean keySpecial = false;
			boolean keyReleased = false;

			if (keyQueue.size() !=0 ) {
				KeyState keyState = keyQueue.remove(0);
				keyCode = keyState.keyCode;
				keySpecial = keyState.keySpecial;
				keyReleased = keyState.keyReleased;
			}

			gameMode = NativeLib.render(touchX, touchY, turnFact, keyCode, keySpecial, keyReleased);

			if (gameMode != NativeLib.NO_MODE && gameMode != prevMode) {
				post(new Runnable() {					
					public void run() {
						mMainActivity.setOverlayView(gameMode);
					}
				});
				prevMode = gameMode;
			}

			if (gameMode != NativeLib.NO_MODE) {
				requestRender();
			} else {
				mMainActivity.requestExit();
			}

		}

		public void onSurfaceChanged(GL10 gl, int width, int height) {
			NativeLib.resize(width, height);
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		}

	}

	public class KeyState {
		int keyCode;
		boolean keySpecial;
		boolean keyReleased;

		KeyState(int keyCode2, boolean keySpecial2, boolean keyReleased2) {
			keyCode = keyCode2;
			keySpecial = keySpecial2;
			keyReleased = keyReleased2;
		}
	}

}
