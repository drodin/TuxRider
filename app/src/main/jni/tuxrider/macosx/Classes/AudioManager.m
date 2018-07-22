//
//  AudioManager.m
//  tuxracer
//
//  Created by emmanuel de roux on 26/10/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import "AudioManager.h"
#import "AudioToolbox/AudioServices.h"
#import "AudioToolbox/AudioQueue.h"
#import "prefsController.h"

static AudioManager* sharedAudioManager=nil;

void playIphoneMusic(const char* context, int mustLoop){
	[sharedAudioManager playMusicForContext:[NSString stringWithUTF8String:context] andLoop:mustLoop];
}

void playIphoneSound(const char* context, int isSystemSound){
	[sharedAudioManager playSoundForContext:[NSString stringWithUTF8String:context] isSystemSound:isSystemSound];
}

void adjustSoundGain(const char* context, int volume){
	[sharedAudioManager setVolume:volume forContext:[NSString stringWithUTF8String:context]];
}

void haltSound(const char* context){
	[sharedAudioManager haltSoundForContext:[NSString stringWithUTF8String:context]];
}

#ifdef DEBUG_AUDIO
# define DBG_LOG() printf("%s %s\n", __func__, [context UTF8String])
#else
# define DBG_LOG()
#endif

@implementation AudioManager
- (id)init
{
    self = [super init];
    if(!self) return nil;

	//PAS sur de ca
	AudioSessionInitialize( CFRunLoopGetCurrent(), 
						   NULL, 
						   NULL, 
						   NULL);
	sharedAudioManager=self;

    _playingSoundContextStack = [[NSMutableArray array] retain];
    _audioPlayersForContext = [[NSMutableDictionary dictionary] retain];
    _musicAudioPlayers = [[NSMutableArray array] retain];
    _soundAudioPlayers = [[NSMutableArray array] retain];

	NSString* dataDir = [[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"TRWC-data"] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	NSString* soundsDir = [dataDir stringByAppendingPathComponent:@"iPhone_sounds"];
	NSString* musicsDir = [dataDir stringByAppendingPathComponent:@"iPhone_music"];
    
    //sets defaults if no prefs exists
    [prefsController setDefaults];
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

    NSString * soundsName[] = {        @"tux_on_snow1.aif", @"tux_on_ice1.aif", @"tux_on_rock1.aif", @"tux_in_air1.aif" };
    NSString * soundContexts[] = { @"snow_sound",       @"ice_sound",       @"rock_sound",       @"flying_sound" };
    int i;
    for(i = 0; i < sizeof(soundsName)/sizeof(*soundsName); i++) {
        AudioPlayer * audioPlayer = [[AudioPlayer alloc] initWithURL:[NSURL URLWithString:[soundsDir stringByAppendingPathComponent:soundsName[i]]]];
        [audioPlayer setGainFactor: [prefs floatForKey:@"soundsVolume"]];
        [_audioPlayersForContext setObject:audioPlayer forKey:soundContexts[i]];
        [_soundAudioPlayers addObject:audioPlayer];
        [audioPlayer release];
	}

    NSString * musicName[] = {        @"start1_loop.aif", @"options1-jt_loop.aif", @"race1-rlb.aif", @"race2-jt.aif", @"wonrace1-jt.aif" };
    NSString * musicContexts[] = { @"start_screen",    @"credits_screen",       @"racing", @"racing2",            @"game_over" };
    for(i = 0; i < sizeof(musicName)/sizeof(*musicName); i++) {
        AudioPlayer * audioPlayer = [[AudioPlayer alloc] initWithURL:[NSURL URLWithString:[musicsDir stringByAppendingPathComponent:musicName[i]]]];
        [audioPlayer setGainFactor: [prefs floatForKey:@"musicVolume"]];
        [_audioPlayersForContext setObject:audioPlayer forKey:musicContexts[i]];
        [_musicAudioPlayers addObject:audioPlayer];
        [audioPlayer release];
	}
	_systemSoundIDForContext = [[NSMutableDictionary dictionary] retain];
    NSURL* soundUrl;
    SystemSoundID soundID;

    soundUrl = [NSURL URLWithString:[soundsDir stringByAppendingPathComponent:@"fish_pickup1.aif"]];
    AudioServicesCreateSystemSoundID ((CFURLRef)soundUrl, &soundID);
	[_systemSoundIDForContext setObject:[NSNumber numberWithInt:soundID] forKey:@"item_collect"];

    soundUrl = [NSURL URLWithString:[soundsDir stringByAppendingPathComponent:@"tux_hit_tree1.aif"]];
    AudioServicesCreateSystemSoundID ((CFURLRef)soundUrl, &soundID);
	[_systemSoundIDForContext setObject:[NSNumber numberWithInt:soundID] forKey:@"hit_tree"];
		
	return self;
}

- (void)dealloc
{
    [_soundAudioPlayers release];
    [_musicAudioPlayers release];
    for(NSNumber * num in _systemSoundIDForContext) {
        SystemSoundID soundID = [num intValue];
        AudioServicesDisposeSystemSoundID (soundID);
    }
    [_systemSoundIDForContext release];
    [_audioPlayersForContext release];
    [super dealloc];
}

- (void)playSoundForContext:(NSString*)context isSystemSound:(BOOL)isSS
{
    DBG_LOG();
	if (isSS)
	{
		SystemSoundID soundID = [[_systemSoundIDForContext objectForKey:context] intValue];
		AudioServicesPlaySystemSound(soundID);
	}
	else
	{
        AudioPlayer * audioPlayer = [_audioPlayersForContext objectForKey:context];
        assert(audioPlayer);
        [audioPlayer play];
	}
}

- (void)haltSoundForContext:(NSString*)context
{
    DBG_LOG();
    AudioPlayer * audioPlayer = [_audioPlayersForContext objectForKey:context];
    assert(audioPlayer);
    if([_musicAudioPlayers containsObject:audioPlayer])
        [audioPlayer stop];
    else
        [audioPlayer pause];
}

- (void)playMusicForContext:(NSString*)context andLoop:(BOOL)mustLoop
{
    DBG_LOG();
    
    // Change the racing music randomly
    if([context isEqualToString:@"racing"])
    {
        if(rand() % 2 == 0)
            context = @"racing2";
    }

    AudioPlayer * audioPlayer = [_audioPlayersForContext objectForKey:context];
    assert(audioPlayer);

    for(AudioPlayer * anAudioPlayer in _musicAudioPlayers)
    {
        if(anAudioPlayer != audioPlayer) {
            if([_musicAudioPlayers containsObject:anAudioPlayer])
                [anAudioPlayer stop];
            else
                [anAudioPlayer pause];
        }
    }
	[audioPlayer setVolume:45];
    [audioPlayer play];
}

- (void)setVolume:(int)volume forContext:(NSString*)context
{
    DBG_LOG();
    AudioPlayer * audioPlayer = [_audioPlayersForContext objectForKey:context];
    assert(audioPlayer);
	[audioPlayer setVolume:volume];
}

- (void)setSoundGainFactor:(Float32)gain
{
    for(AudioPlayer * audioPlayer in _soundAudioPlayers)
    {
        [audioPlayer setGainFactor:gain];
        [audioPlayer updateVolume];
    }
}

- (void)setMusicGainFactor:(Float32)gain
{
    for(AudioPlayer * audioPlayer in _musicAudioPlayers)
    {
        [audioPlayer setGainFactor:gain];
        [audioPlayer updateVolume];
    }
}
@end
