//
//  TRAppDelegate.m
//  tuxracer
//
//  Created by emmanuel de roux on 22/10/08.
//  Copyright école Centrale de Lyon 2008. All rights reserved.
//

#include "sharedGeneralFunctions.h"

const char* getRessourcePath() {
	return DATA_DIR;
}

const char* getConfigPath() {
	return DATA_DIR;
}

void turnScreenToLandscape() {
    orientation = 1;
}

void turnScreenToPortrait() {
    orientation = 0;
}

void showRegister() {
    //[[TRAppDelegate sharedAppDelegate] showRegister];
}

void showHowToPlay() {
    //[[TRAppDelegate sharedAppDelegate] showHowToPlay:nil];
}

void showHowToPlayAtBegining() {
    //Affiche le register Pannel si on est pas registered
    /*
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    if (![prefs boolForKey:@"registered"])
    {
        // Sometimes, this is called too soon (glView is not yet created). Wait for a run loop.
        [[TRAppDelegate sharedAppDelegate] performSelector:@selector(showHowToPlay:) withObject:nil afterDelay:0.];
    }
    */
}

int playerRegistered() {
    /*
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    if ([prefs boolForKey:@"registered"])
        return YES;
    else
        return NO;
    */
    return 1;
}

void alertRegisterNeeded() {
    /*
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Registered users only" message:@"You need to be a registered candidate to participate to the World Challenge. If you are already registered, click on 'Login'." delegate:[TRAppDelegate sharedAppDelegate] cancelButtonTitle:@"Cancel" otherButtonTitles:@"Register",@"Login",nil];
    [alert show];
    */
}

void vibration()
{
    //AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}
