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

#if defined(__cplusplus)
#define VIBLAST_EXPORT extern "C"
#else
#define VIBLAST_EXPORT extern
#endif

VIBLAST_EXPORT NSString *const VBErrorDomain;

VIBLAST_EXPORT NSString *const VBPlayerPlaybackStalledNotification;

typedef NS_ENUM(NSInteger, VBPlayerStatus) {
  VBPlayerStatusUnknown,
  VBPlayerStatusReadyToPlay,
  VBPlayerStatusFailed
};

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

- (instancetype)initWithCDN:(NSString *)cdn
                 enabledPDN:(BOOL)enabledPDN
                 licenseKey:(NSString *)licenseKey;

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
@end


// Return VBPlayerLayer class as layerClass of your UIView in order to
// display the output of the player, e.g.
//
//  @implementation MyPlayerView
//  ...
//  + (Class)layerClass {
//    return [VBPlayerLayer class];
//  }
//  ...
//  @end
@interface VBPlayerLayer : CALayer

// The player which output will be displayed in the layer
@property (nonatomic, strong) VBPlayer *player;

// You can use KVO to observe the following two properties
@property (nonatomic, readonly, getter=isReadyForDisplay) BOOL readyForDisplay;
@property (nonatomic, readonly) CGRect displayRect;

+ (instancetype)layerWithPlayer:(VBPlayer *)player;
@end
