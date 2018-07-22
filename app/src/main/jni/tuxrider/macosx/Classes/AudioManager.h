//
//  AudioManager.h
//  tuxracer
//
//  Created by emmanuel de roux on 26/10/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AudioPlayer.h"
#import "sharedAudioFunctions.h"

@interface AudioManager : NSObject {
    NSMutableArray * _playingSoundContextStack;
    NSMutableArray * _musicAudioPlayers;
    NSMutableArray * _soundAudioPlayers;
    NSMutableDictionary * _systemSoundIDForContext;
    NSMutableDictionary * _audioPlayersForContext;
}

- (void)setSoundGainFactor:(Float32)gain;
- (void)setMusicGainFactor:(Float32)gain;

- (void)playSoundForContext:(NSString*)context isSystemSound:(BOOL)isSS;
- (void)playMusicForContext:(NSString*)context andLoop:(BOOL)mustLoop;
- (void)haltSoundForContext:(NSString*)context;
- (void)setVolume:(int)volume forContext:(NSString*)context;
@end
