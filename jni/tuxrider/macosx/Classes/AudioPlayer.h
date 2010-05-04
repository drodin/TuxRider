#import <UIKit/UIKit.h>
#include <AudioToolbox/AudioToolbox.h>


#define kNumberAudioDataBuffers	5
#define kSecondsPerBuffer	0.1

@interface AudioPlayer : NSObject {
	NSURL *     _url;
	AudioQueueBufferRef				_buffers[kNumberAudioDataBuffers];	// the audio queue buffers for the audio queue
	
	UInt32							_bufferByteSize;						// the number of bytes to use in each audio queue buffer
	UInt32							_startingPacketNumber;						// the number of bytes to use in each audio queue buffer
	UInt32							_numPacketsToRead;					// the number of audio data packets to read into each audio queue buffer
		
	Float32							_gain;								// the gain (relative audio level) for the playback audio queue
    Float32                         _gainFactor;
	AudioFileID                     _audioFileID;
    AudioQueueRef					_audioQueue;
	AudioStreamPacketDescription	_packetDescription;
    AudioStreamBasicDescription     _format;
    BOOL 							_isPlaying;
}

@property (readonly,copy) NSURL * url;

@property (readonly,assign) AudioFileID audioFileID;

@property (readwrite) UInt32						startingPacketNumber;
@property (readwrite) UInt32						bufferByteSize;
@property (readwrite) UInt32						numPacketsToRead;
@property (readwrite) Float32						gainFactor;
@property (readonly) AudioStreamPacketDescription *	packetDescription;
@property (readonly) BOOL	isPlaying;

- (id) initWithURL:(NSURL *)url;

- (void) setVolume:(int)volume;
- (void) updateVolume;

- (void) incrementStartingPacketNumberBy: (UInt32) inNumPackets;

- (BOOL) isRunning;

- (void) play;
- (void) pause;
- (void) stop;

@end
