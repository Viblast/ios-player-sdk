### Overview
Viblast Player iOS SDK plays HLS and MPEG DASH streams using the iOS devices’ hardware decoders for video and audio. The API is similar to Apple's AVPlayer, but much simple and can be easily integrated into existing projects using the AVPlayer. Currently, this DASH player SDK is most suitable for live streaming applications as the seek features is not yet enabled. If your main interest is in video on-demand (VoD), let us know. The Player can be easily extended to use Viblast PDN and enable P2P delivery.

Visit [Viblast's commercial web page](http://viblast.com/)

### Project setup
Let's assume you have an existing XCode project or you've just created one.   

Unzip...  
Drag and drop the unzipped content in your project.  
For example, you can make a group in your project, name it 
*VBPlayer*, select the unzipped content (that is the `VBPlayer.a` library and 
all the header files), and drop it inside.
When XCode asks you to choose options for added files, don't forget to mark
the '*Copy items if needed*' option.  
Notice that `VBPlayer.a` is a *fat* library, i.e. it is built for both 32bit 
and 64bit architectures.

Now go to the project's build target. 

Select **Build Phases** and under the **Link Binary With Libraries** section add the 
following frameworks:

* Foundation
* AVFoundation
* VideoToolbox
* CoreGraphics
* CoreMedia  

*NOTE: If you've followed the previous steps, the `VBPlayer.a` lib should 
already be there.*

Select **Build Settings** and add the following in **Other Linker Flags**:

* -lstdc++
* -ObjC

### API

VBPlayer is reminiscent of Apple's AVPlayer, but much simpler.

Currenty, you can initialize Viblast Player just by providing it with an *HLS* or *DASH* 
stream, for example:  
`_player = [[VBPlayer alloc] initWithCDN:@"www.mycdn.com/playlist.m3u8"];`

The player has a status property which you can observe.   
When the player status is changed to `VBPlayerStatusReadyToPlay` you can invoke 
the player's `play` method.

If you want to stop the player, you just have to release it, like this:  
`_player = nil;`

If you want to restart it, you just have to reinitilize it, like this:  
`_player = [[VBPlayer alloc] ... ];`

If the player fails for some reason, i.e when its status becomes 
`VBPlayerStatusFailed`, you can examine its `error` property for more information 
about the failure.

### Usage example

The first thing you need to do is to provide or create a `UIView`, which will 
display the output of the player. The views's layer must be of `VBPlayerLayer` class, i.e it must have its `+layerClass` method overridden:
  
```
// PlayerView.m
#import "VBPlayer.h"
@implementation PlayerView

+ (Class)layerClass {
  return [VBPlayerLayer class];
}
@end
```
Now let's assume all the action will happen in a view controller named 
**PlayerViewController:**

```
// PlayerViewController.m

#import "VBPlayer.h"
#import "PlayerView.h"

// We will use custom observe context for the player's status. 
// You can observe it with or without different context. It's up to you.
static void *PlayerStatusObserveCtx = &PlayerStatusObserveCtx;

@interface PlayerViewController() 

@property (nonatomic, strong) VBPlayer *player;
@property (nonatomic, strong) PlayerView *playerView;
@end

@imlementation PlayerViewController

- (void)viewDidLoad {
  [super viewDidLoad];

  self.playerView = [[PlayerView alloc] initWithFrame:self.view.bounds];
  // An important step is to set the appropriate resize mask so the video 
  // can be properly displayed when the device is rotated.
  self.playerView.autoresizingMask = (UIViewAutoresizingFlexibleWidth 
                                      | UIViewAutoresizingFlexibleHeight);
  [self.view addSubview:self.playerView];

  self.player = [[VBPlayer alloc] initWithCDN:@"www.mycdn.com/playlist.m3u8"];
  [self.player addObserver:self
                forKeyPath:@"status"
                   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionInitial)
                   context:PlayerStatusObserveCtx];
}

- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context {
  if (context = PlayerStatusObserveCtx) {
    // NOTE: The player will dispatch KVOs about its status on the main queue.
    VBPlayerStatus status = (VBPlayerStatus)[change[NSKeyValueChangeNewKey] integerValue];
    switch (status) {
      case VBPlayerStatusUnknown: {
      // Not ready yet
      break;
      }
      case VBPlayerStatusReadyToPlay: {
        // The player is ready to play and we can display its output.
        [(VBPlayerLayer *)[self.playerView layer] setPlayer:self.player];
        [self.player play];
      break;
      }
      case VBPlayerStatusFailed: {
        NSError *error = self.player.error;
        NSLog(@"Player failed: %@", error);
        break;
      }
    }
  }
}
```
