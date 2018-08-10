/*
 *  sharedFunctions.h
 *  tuxracer
 *
 *  Created by emmanuel de roux on 26/10/08.
 *  Copyright 2008 école Centrale de Lyon. All rights reserved.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif 
	void playIphoneMusic(const char * context,int mustLoop);                         //ces deux fonctions sont dimplémentées dans AudioManager.m
	void playIphoneSound(const char* context, int isSystemSound);
	void adjustSoundGain(const char* context, int volume);
    void haltSound(const char* context);
#ifdef __cplusplus
}
#endif 