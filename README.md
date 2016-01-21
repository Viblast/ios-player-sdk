### Overview
Viblast Player iOS SDK plays HLS and MPEG DASH streams using the iOS devices hardware decoders for video and audio. The API is similar to Apple's AVPlayer, but much simpler and can be easily integrated into existing projects using the AVPlayer. The Player can be easily configured to use Viblast PDN and enable P2P delivery.

Visit [Viblast's commercial web page](http://viblast.com/)

This repository contains an example XCode project to demonstrate the basic usage of the player.

### XCode
If you want to start using [Viblast](http://viblast.com/player/)'s player in your XCode project here is what you need to do:  

Drag and drop the library and all of its headers (you can find them inside the example project - all VB\*.\* files) 
in your project. When XCode asks you to choose options for added files, don't forget to put a mark on '*Copy items if needed*' option.  
The remarkable size of `VBPlayer.a` is due to the fact the library is *fat*. It has 4 different builds inside of it - 2 for  device and 2 for simulator (one for 32bit and one for 64bit architectures).  

Go to the project's build target.   
**(We've encountered some issues migrating to XCode7 so until we resolve them please set the _Enable Bitcode_ option to "_No_"!)**   
Select **Build Phases** and under the **Link Binary With Libraries** section make sure the  
following are present:  

* Foundation
* AVFoundation
* VideoToolbox
* CoreGraphics
* CoreMedia    

*NOTE: If you've followed the previous steps, the `VBPlayer.a` lib should 
already be there.*  
Now select **Build Settings** and add the following in **Other Linker Flags**:

* -lstdc++
* -ObjC

### Brief documentation

`VBPlayer.h` is pretty self-explanatory on its own and it can be used as a good source of documentation.
It defines two major interfaces:

* `VBPlayer` is a player which can play a *HLS* or *DASH* streams provided from a *CDN* and its successor
* `VBDataPlayer` can play separate media segments (or chunks) delivered by the client.

##### VBPlayer
The API is reminiscent of Apple's AVPlayer, but much simpler.  
You can initialize the player with a single URL of the CDN hosting the stream:
```
VBPlayer *player = [[VBPlayer alloc] initWithCDN:@"www.mycdn.com/manifest.mpd"];
```
Immediately after its initialization the player will try to start downloading the stream. The `status` property is *KVO-compliant* and as soon as the player has buffered enough
data it will be set to `VBPlayerStatusReadyToPlay`.   
Having this property set means you can assume the following:
* the playback will start simultaneously if you issue `-play`
* `duration` and `currentTime` will be resolved.

If the player fails for some reason the status will be set to `VBPlayerStatusFailed` and the `error` property will contain
more information about the failure.  
The player will stop automatically when it is deallocated. If you need to change the *CDN* source of the player you'll just have to reinitialize it.

You can use `-seekToTime:` methods to seek the media timeline if the stream is not a live broadcast. To let you help draw
adequate seek bar you can subscribe as a timeline observer using `-addPeriodicTimeObserverForInterval:queue:usingBlock:`

Setting a `VBPlayerDelegate` will get you notified about a certain playback events, like stalls and when the playback has finished.

##### VBDataPlayer
This API lets you feed the player with media segments much like a queue or a buffer:
````
VBDataPlayer *player = [VBDataPlayer createDataPlayer];
...
NSError *error = nil;
BOOL succ;
NSData *segmentData = ... ;
// NOTE: It will be a good idea not to call this from the main thread.
succ = [player appendData:segmentData coding:(VBDataCodingMP4) error:&error];
if (!succ) {
    // inspect error
}
````
Current supported media codings are all variety of the MP4.  

As soon as the player had been supplied with enough data (both audio and video) it will change its `status` property.  
You can inspect the buffered duration ahead of time using `-bufferredDuration` method.  
This implementation doesn't know when the stream is going to an end so it is the client's responsibility to call `-endOfStream` whenever there is no more data to append. If there is a delegate, `-playerDidFinish` will be called immediately after the last piece of data had been played.

A thing to **keep in mind** about this player is that **it doesn't cache any data back in time**. This means that anything that
had been played *must* be supplied again if a *seek* is issued.

##### VBDisplayLayer
After the player is ready to play (or even if it is not) you can supply it with a render or a display layer using VBPlayer's `-setDisplayLayer` to capture its video output.  
A recommended usage of `VBDisplayLayer` is to define your own `UIView` and set it as layer of this view:
```
// PlayerView.m
#import "VBDisplayLayer.h"
@implementation PlayerView
+ (Class)layerClass {
  return [VBDisplayLayer class];
}
@end
```
It has some useful properties like the *KVO-Compliant* `displayRect` which you should observer to position your UI controls.
