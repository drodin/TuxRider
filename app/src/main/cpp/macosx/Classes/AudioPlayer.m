#include <AudioToolbox/AudioToolbox.h>
#import "AudioPlayer.h"


static void playbackCallback (
							  void					*inUserData,
							  AudioQueueRef			inAudioQueue,
							  AudioQueueBufferRef	bufferReference
) {
	// This callback, being outside the implementation block, needs a reference to the AudioPlayer object
	AudioPlayer *player = (AudioPlayer *) inUserData;
	
	UInt32 numBytes;
	UInt32 numPackets = [player numPacketsToRead];

    if(![player isPlaying]) return;

	// This callback is called when the playback audio queue object has an audio queue buffer
	// available for filling with more data from the file being played
	AudioFileReadPackets ([player audioFileID], NO, &numBytes, bufferReference->mPacketDescriptions, [player startingPacketNumber], &numPackets, bufferReference->mAudioData);

    if (numPackets <= 0) {
        // End of the file, loop.
        numPackets = [player numPacketsToRead];
        [player setStartingPacketNumber: 0];
        AudioFileReadPackets ([player audioFileID], NO, &numBytes, bufferReference->mPacketDescriptions, [player startingPacketNumber], &numPackets, bufferReference->mAudioData);
    }

	if (numPackets > 0) {

		bufferReference->mAudioDataByteSize = numBytes;		
		
		AudioQueueEnqueueBuffer (inAudioQueue, bufferReference, 1, [player packetDescription]);
		
		[player incrementStartingPacketNumberBy: numPackets];
	}
    else {
        printf("ERROR: cant reset\n");
    }
}

@interface AudioPlayer ()
- (void) _calculateSizesFor: (Float64) seconds;
@end


@implementation AudioPlayer

@synthesize url=_url;
@synthesize audioFileID=_audioFileID;
@synthesize bufferByteSize=_bufferByteSize;
@synthesize numPacketsToRead=_numPacketsToRead;
@synthesize bufferByteSize=_bufferByteSize;
@synthesize startingPacketNumber=_startingPacketNumber;
@synthesize gainFactor=_gainFactor;


//c'est la fonction d'initiations dans le cadre d'un sound Player et non d'un Music Player
- (id) initWithURL:(NSURL *)url {
	
	self = [super init];
	if (self != nil) {
        OSStatus ret;
        ret = AudioFileOpenURL ( (CFURLRef)url, 0x01, kAudioFileCAFType, &_audioFileID);
        if (ret != noErr) {
            [self release];
            return nil;
        }
        _gainFactor = 1.;
        [self _calculateSizesFor:kSecondsPerBuffer];

        UInt32 sizeOfPlaybackFormatASBDStruct = sizeof (AudioStreamBasicDescription);
        
        // get the AudioStreamBasicDescription format for the playback file
        AudioFileGetProperty (_audioFileID, 
                              kAudioFilePropertyDataFormat,
                              &sizeOfPlaybackFormatASBDStruct,
                              &_format
                              );

        // create the playback audio queue object
        AudioQueueNewOutput (&_format, playbackCallback, self, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &_audioQueue);

        //On met le volume a 0 pour le début
        AudioQueueSetParameter (_audioQueue, kAudioQueueParam_Volume, 0.0);

        int i;
        for (i = 0; i < kNumberAudioDataBuffers; i++) {
            AudioQueueAllocateBuffer (_audioQueue, [self bufferByteSize], &_buffers[i]);
        }

	}
	return self;
} 

- (void)dealloc
{
    if([self isRunning]) {
        AudioQueueStop (_audioQueue, YES);
    }
    int i;
	for (i = 0; i < kNumberAudioDataBuffers; i++) {
		AudioQueueFreeBuffer(_audioQueue, _buffers[i]);
	}

	AudioQueueDispose (_audioQueue, YES);
    [super dealloc];
}

- (AudioStreamPacketDescription *) packetDescription
{
    return &_packetDescription;
}

- (void) incrementStartingPacketNumberBy: (UInt32) inNumPackets
{	
	_startingPacketNumber += inNumPackets;
}

- (BOOL) isRunning
{	
	UInt32		isRunning;
	UInt32		propertySize = sizeof (UInt32);
	OSStatus	result;
	
	result =	AudioQueueGetProperty (_audioQueue, kAudioQueueProperty_IsRunning, &isRunning, &propertySize );
	if (result != noErr) {
		return false;
	} else {
		return isRunning;
	}
}

- (void) setVolume:(int)volume {
	//Le volume dans tuxracer varie entre 0 et 128
	Float32 maxVolume = 128.0;
    _gain = (Float32)volume/maxVolume;
    if (_gain > 1.0) _gain = 1.;
    AudioQueueSetParameter (_audioQueue, kAudioQueueParam_Volume, _gain*_gainFactor );
}

-(void) updateVolume {
     AudioQueueSetParameter (_audioQueue, kAudioQueueParam_Volume, _gain*_gainFactor );
}

- (void) play {
    int i;
    if(_isPlaying) return;
    _isPlaying = YES;
    for (i = 0; i < kNumberAudioDataBuffers; i++) {
        playbackCallback (self, _audioQueue, _buffers[i]);
    }
	AudioQueueStart(_audioQueue, NULL);
}

- (BOOL) isPlaying
{
    return _isPlaying;
}

- (void) pause {
	if (_isPlaying) {
        _isPlaying = NO;
        [self setStartingPacketNumber:0];
        AudioQueuePause (_audioQueue);
	}
}

- (void) stop {
	if (_isPlaying) {
        _isPlaying = NO;
        [self setStartingPacketNumber:0];
        AudioQueueStop (_audioQueue, YES);
	}
}

- (void) _calculateSizesFor: (Float64) seconds {
	UInt32 maxPacketSize;
	UInt32 propertySize = sizeof (maxPacketSize);
	
	AudioFileGetProperty (_audioFileID, kAudioFilePropertyPacketSizeUpperBound, &propertySize, &maxPacketSize);
	
	static const int maxBufferSize = 0x10000;	// limit maximum size to 64K
	static const int minBufferSize = 0x4000;	// limit minimum size to 16K
	
	if (_format.mFramesPerPacket) {
		Float64 numPacketsForTime = _format.mSampleRate / _format.mFramesPerPacket * seconds;
		_bufferByteSize = numPacketsForTime * maxPacketSize;
	} else {
		// if frames per packet is zero, then the codec doesn't know the relationship between 
		// packets and time -- so we return a default buffer size
        _bufferByteSize = maxBufferSize > maxPacketSize ? maxBufferSize : maxPacketSize;
	}
	
	// we're going to limit our size to our default
	if (_bufferByteSize > maxBufferSize && _bufferByteSize > maxPacketSize) {
		_bufferByteSize = maxBufferSize;
	} else {
		// also make sure we're not too small - we don't want to go the disk for too small chunks
		if (_bufferByteSize < minBufferSize) {
			_bufferByteSize = minBufferSize;
		}
	}
    _numPacketsToRead = self.bufferByteSize / maxPacketSize;
}

@end
