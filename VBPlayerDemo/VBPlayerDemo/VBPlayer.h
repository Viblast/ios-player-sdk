//
//  VBPlayer.h
//  VBPlayer
//
//  Created by Ivan Ermolaev on 4/21/15.
//  Copyright (c) 2015 Viblast. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <AVFoundation/AVFoundation.h>

#import "VBDisplayLayer.h"

#if defined(__cplusplus)
#define VIBLAST_EXPORT extern "C"
#else
#define VIBLAST_EXPORT extern
#endif

typedef NS_ENUM(NSInteger, VBPlayerStatus) {
  VBPlayerStatusUnknown,
  VBPlayerStatusReadyToPlay,
  VBPlayerStatusFailed
};

VIBLAST_EXPORT NSString *const VBErrorDomain;

/* 
 VBPlayer settings dictionary keys
 
 */

// Enables p2p media delivery
VIBLAST_EXPORT NSString *const VBEnablePDNKey; // NSNumber (BOOL)


/*
 VBPlayerDelegate
 
 All of the methods are sent on the main thread.
 */
@class VBPlayer;

@protocol VBPlayerDelegate <NSObject>

@optional

- (void)playerDidEnterStall:(VBPlayer *)player;
- (void)playerDidExitStall:(VBPlayer *)player;

- (void)playerDidFinish:(VBPlayer *)player;
@end


/*
 VBPlayer
 
 */
@interface VBPlayer : NSObject

// Contains the error when player's status is set to VBPlayerStatusFailed.
@property (nonatomic, readonly) NSError *error;

// Use KVO to observe the status of the player so you know when to call play.
// All status notifications during the playback are serialized on the main queue.
// If the state happens to be VBPlayerStatusFailed you must reinitalize the player.
@property (nonatomic, readonly) VBPlayerStatus status;

// Currently supported values are 0.0 for stopped playback and 1.0 for playing at normal rate.
@property (nonatomic) float rate;

// Can't use KVO
@property (readonly) CMTime duration;

// Use periodic time notifications to poll this property.
@property (readonly) CMTime currentTime;

@property (nonatomic, weak) id<VBPlayerDelegate> delegate;

- (instancetype)initWithCDN:(NSString *)cdn;
- (instancetype)initWithCDN:(NSString *)cdn settings:(NSDictionary *)settings;

- (instancetype)initWithArgs:(NSString *)args;

- (void)play;
- (void)pause;

- (void)seekToTime:(CMTime)time;

// Use to receive a callback when seek is completed. Only the completion of the last seek will be
// called, i.e. if there is a seek in progress its completion will be ignored.
- (void)seekToTime:(CMTime)time completion:(void(^)(void))completion;

// Observe periodic time changes.
// Must retain the result in order the |block| to be called.
// If |queue| is NULL the updates will be deliverd on the main queue.
// Use |interval| to specify how often the block should be called.
// Inside the observer you can inspect the current time.
// Pair with -removeTimeObserver:.
- (id)addPeriodicTimeObserverForInterval:(CMTime)interval
                                   queue:(dispatch_queue_t)queue
                              usingBlock:(void (^)(void))block;
- (void)removeTimeObserver:(id)observer;

// Set a layer to render the player's output
- (void)setDisplayLayer:(VBDisplayLayer *)layer;

@end


/*
 VBDataPlayer
 
 A subclass of VBPlayer which exposes a buffer-like interface to let you feed the player with media 
 segments. Current supported media coding is only MP4.

 Things to keep in mind:
 * You can manipulate the current time using -seekToTime:. By default it is set to 0.
 * The player DOESN'T CACHE any data back in time, so:
   1) whenever you seek, all the data prior to the seeked moment will be discarded. 
   2) it's the client's responsibility to cache or acquire the media data again and feed it when 
   appropriate.
 * Calling |duration| on this player will result in kCMTimeIndefinite.
 * You should again observer the player's |status| to know when the player is ready to play.
 
 */

typedef NS_ENUM(NSInteger, VBDataCoding) {
  VBDataCodingMP4,
  VBDataCodingMP4Audio,
  VBDataCodingMP4Video,
  VBDataCodingMP4Metadata,
};

@interface VBDataPlayer : VBPlayer

+ (instancetype)createDataPlayer;

// Enqueues media |data| to be played at some point ahead of time.
// NOTE: Each |data| MUST be the continuation of the data previously appended! This does NOT hold
// if the append is made after a seek or if this is the first append.
- (BOOL)appendData:(NSData *)data coding:(VBDataCoding)coding error:(NSError **)outError;

// Returns the buffered media time ahead. This is the minimum between the audio and the video.
- (CMTime)bufferredDuration;

// Tells the player that there is no more data to be played and that it should finish when it has
// played its last piece of data.
// NOTE: Whenever -seekToTime: is issued this information is lost and you have to call this method
// again when appropriate.
- (void)endOfStream;
@end
