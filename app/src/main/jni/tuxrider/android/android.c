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

#include <math.h>

#include <stdio.h>
#include <assert.h>
#include <dlfcn.h>

#include "winsys.h"
#include "touchwinsys.h"

static const char *classPathName = LIB_CLASS;

int initDone = 0;

int prevX=-1;
int prevY=-1;
int oldX=-1;
int oldY=-1;

double turnFact = 0.0;

int forceExit = 0;

int global_event_type = 0;
int global_event_value = 0;

int orientation = 0;

JNIEnv* cenv = NULL;
jclass nclass;
jmethodID OnStartMusic, OnStopMusic, OnStartSound, OnVolumeSound, OnStopSound;
jfieldID soundEnabled, videoQuality, viewMode;

int mWidth = 0;
int mHeight = 0;

extern int libtuxracer_main( int argc, char **argv );

void set_video_quality(int quality) {
    switch (quality) {
        case 0: //low
            setparam_forward_clip_distance(20);
            setparam_track_marks(0);
	        setparam_backward_clip_distance(0);
            break;
        case 1: //med
            setparam_forward_clip_distance(30);
            setparam_track_marks(0);
	        setparam_backward_clip_distance(0);
            break;
        case 2: //high
            setparam_forward_clip_distance(40);
            setparam_track_marks(1);
	        setparam_backward_clip_distance(5);
            break;
    }
}

void saveparamSoundEnabled(int val) {
    (*cenv)->SetStaticIntField(cenv, nclass, soundEnabled, val);    
}

void saveparamVideoQuality(int val) {
    (*cenv)->SetStaticIntField(cenv, nclass, videoQuality, val);        
}

void saveparamViewMode(int val) {
    (*cenv)->SetStaticIntField(cenv, nclass, viewMode, val);        
}

void
resize(JNIEnv *env, jobject thiz, jint width, jint height) {
    cenv = env;

    mWidth = width;
    mHeight = height;

    jclass lclass = (*cenv)->FindClass(cenv, classPathName);
    nclass =  (jclass)((*cenv)->NewGlobalRef(cenv, lclass));

    OnStartMusic = (*cenv)->GetStaticMethodID(cenv, nclass, "OnStartMusic", "(Ljava/lang/String;I)V");
    OnStopMusic = (*cenv)->GetStaticMethodID(cenv, nclass, "OnStopMusic", "()V");
    OnStartSound = (*cenv)->GetStaticMethodID(cenv, nclass, "OnStartSound", "(Ljava/lang/String;I)V");
    OnVolumeSound = (*cenv)->GetStaticMethodID(cenv, nclass, "OnVolumeSound", "(Ljava/lang/String;I)V");
    OnStopSound = (*cenv)->GetStaticMethodID(cenv, nclass, "OnStopSound", "(Ljava/lang/String;)V");

    soundEnabled = (*cenv)->GetStaticFieldID(cenv, nclass, "soundEnabled", "I");
    int snd = (*cenv)->GetStaticIntField(cenv, nclass, soundEnabled);
    
    videoQuality = (*cenv)->GetStaticFieldID(cenv, nclass, "videoQuality", "I");
    int vid = (*cenv)->GetStaticIntField(cenv, nclass, videoQuality);

    viewMode = (*cenv)->GetStaticFieldID(cenv, nclass, "viewMode", "I");
    int vim = (*cenv)->GetStaticIntField(cenv, nclass, viewMode);

    if (!initDone) {
        libtuxracer_main(0, NULL);
        setparam_x_resolution(mWidth);
        setparam_y_resolution(mHeight);
        setparam_music_enabled(snd);
        setparam_sound_enabled(snd);
        setparam_video_quality(vid);
        set_video_quality(vid);
        setparam_view_mode(vim);
        initDone = 1;
    } else {
        setparam_x_resolution(mWidth);
        setparam_y_resolution(mHeight);
    }

}

jint
render(JNIEnv *env, jobject thiz, jint touchX, jint touchY, jdouble turnFactor, jint keyCode, jboolean keySpecial, jboolean keyReleased) {

    turnFact = turnFactor;

    if (initDone) {

        if (mouse_func != NULL && touchX != oldX && touchY != oldY && prevX == -1 && prevY == -1) {
            mouse_func(WS_LEFT_BUTTON, WS_MOUSE_DOWN, touchX, touchY);
            prevX = touchX; prevY = touchY;
        } else if (mouse_func != NULL && touchX != oldX && touchY != oldY) {
            oldX = prevX; oldY = prevY;            
            mouse_func(WS_LEFT_BUTTON, WS_MOUSE_UP, prevX, prevY);
            prevX = -1; prevY = -1;
        }

        if (keyboard_func != NULL && keyCode != -1) {
            keyboard_func(keyCode, keySpecial, keyReleased, 1/*x*/, 1/*y*/);
        }

        if (idle_func != NULL) {
            idle_func();
        }
    
    }

    if (forceExit)
        return(NO_MODE);
    else
        return(g_game.mode);

}

static JNINativeMethod methods[] = {
    {"resize", "(II)V", (void*)resize },
    {"render", "(IIDIZZ)I", (void*)render },
};

typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint version = JNI_VERSION_1_4;

    if ((*vm)->GetEnv(vm, &uenv.venv, version) != JNI_OK)
        return JNI_ERR;

    env = uenv.env;
    assert(env != NULL);

    jclass clazz = (*env)->FindClass(env, classPathName);
    if (clazz == NULL)
        return JNI_ERR;

    if ((*env)->RegisterNatives(env, clazz, methods, sizeof(methods) / sizeof(methods[0])) < 0)
        return JNI_ERR;

    return version;
}

