/*
 *  sharedFunctions.h
 *  tuxracer
 *
 *  Created by emmanuel de roux on 06/11/08.
 *  Copyright 2008 école Centrale de Lyon. All rights reserved.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif 
    /* implémentées dans TRAppDelegate */
	const char* getRessourcePath(void);
	const char* getConfigPath(void);

    void turnScreenToLandscape(void);
    void turnScreenToPortrait(void);
    void vibration(void);
    void showRegister(void); 
    int playerRegistered(void);
    void showHowToPlay(void);
    void showHowToPlayAtBegining(void);
    void alertRegisterNeeded(void);
     
    /* implémentées dans scoresController */
    void saveScoreOnlineAfterRace(char* raceName,int score,int herring,int minutes,int seconds,int hundredths);
    void displayRankingsAfterRace(char* raceName,int score,int herring,int minutes,int seconds,int hundredths);
    void displaySlopes(void);
    void dirtyScores (void);
    
    /* //implémentées dans TRAccelerometterDelegate */
    double accelerometerTurnFact(void);
    
    /* implementée dans prefsController */
    int plyrWantsToSaveOrDisplayRankingsAfterRace();
#ifdef __cplusplus
}
#endif 
