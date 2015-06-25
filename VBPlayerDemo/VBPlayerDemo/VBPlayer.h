//
//  VBPlayer.h
//  VBPlayer
//
//  Created by Ivan Ermolaev on 4/21/15.
//  Copyright (c) 2015 Viblast. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

#if defined(__cplusplus)
#define VIBLAST_EXPORT extern "C"
#else
#define VIBLAST_EXPORT extern
#endif

VIBLAST_EXPORT NSString *const VBErrorDomain;

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

- (instancetype)initWithCDN:(NSString *)cdn licenseKey:(NSString *)licenseKey;
- (instancetype)initWithArgs:(NSString *)args;

- (void)play;
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
