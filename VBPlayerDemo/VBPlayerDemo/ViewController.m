//
//  ViewController.m
//  VBPlayerDemo
//
//  Created by Ivan Ermolaev on 6/18/15.
//  Copyright (c) 2015 Viblast. All rights reserved.
//

#import "ViewController.h"
#import "PlayerView.h"

static NSUInteger kToolbarPlaybackButtonItemIndex = 1;
static void *kPlayerStatusObserveContext = &kPlayerStatusObserveContext;

@interface ViewController ()

@property (strong, nonatomic) VBPlayer *player;
@property (weak, nonatomic) IBOutlet PlayerView *playerView;

@property (weak, nonatomic) IBOutlet UIToolbar *toolbar;
@property (strong, nonatomic) UIBarButtonItem *playButton;
@property (strong, nonatomic) UIBarButtonItem *stopButton;

@property (nonatomic, strong) UIActivityIndicatorView *loadingView;
@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  // Set toolbar items
  NSMutableArray *items = [NSMutableArray new];
  [items addObject:[[UIBarButtonItem alloc]
                    initWithBarButtonSystemItem:(UIBarButtonSystemItemFlexibleSpace)
                    target:nil action:nil] ];
  self.playButton = [[UIBarButtonItem alloc]
                     initWithBarButtonSystemItem:(UIBarButtonSystemItemPlay)
                     target:self action:@selector(togglePlayback:)];
  self.stopButton = [[UIBarButtonItem alloc]
                      initWithBarButtonSystemItem:(UIBarButtonSystemItemPause)
                      target:self action:@selector(togglePlayback:)];
  [items addObject:self.playButton];
  [items addObject:[[UIBarButtonItem alloc]
                    initWithBarButtonSystemItem:(UIBarButtonSystemItemFlexibleSpace)
                    target:nil action:nil]];
  self.toolbar.items = items;
  
  // Init loading view
  self.loadingView = [[UIActivityIndicatorView alloc]
                      initWithActivityIndicatorStyle:(UIActivityIndicatorViewStyleWhiteLarge)];
  self.loadingView.autoresizingMask = (UIViewAutoresizingFlexibleLeftMargin
                                       | UIViewAutoresizingFlexibleRightMargin
                                       | UIViewAutoresizingFlexibleBottomMargin
                                       | UIViewAutoresizingFlexibleTopMargin);
}

- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
  // Dispose of any resources that can be recreated.
}

- (void)togglePlayback:(UIBarButtonItem *)sender {
  NSMutableArray *items = [self.toolbar.items mutableCopy];
  if (sender == self.playButton) {
    [self start];
    [items replaceObjectAtIndex:kToolbarPlaybackButtonItemIndex withObject:self.stopButton];
  } else {
    [self stop];
    [items replaceObjectAtIndex:kToolbarPlaybackButtonItemIndex withObject:self.playButton];
  }
  self.toolbar.items = items;
}

- (void)start {
  [self setLoadingVisible:YES];
  
  self.player = [[VBPlayer alloc]
                 // Viblast: You can also try with your favourite HLS or DASH stream :)
                 initWithCDN:@"http://responsive-tv-shared.s3.amazonaws.com/sample-video/mpeg-dash-ondemand/full.mpd"
                 enabledPDN:NO
                 licenseKey:nil];
  [self.player addObserver:self
                forKeyPath:@"status"
                   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionInitial)
                   context:kPlayerStatusObserveContext];
}

- (void)stop {
  [self setLoadingVisible:NO];
  [self.player removeObserver:self forKeyPath:@"status"];
  
  self.player = nil;
  self.playerView.player = nil;
}

- (void)setLoadingVisible:(BOOL)visible {
  if (!visible) {
    [self.loadingView stopAnimating];
    [self.loadingView removeFromSuperview];
  } else {
    self.loadingView.center = CGPointMake(CGRectGetWidth(self.playerView.bounds)/2.0,
                                          CGRectGetHeight(self.playerView.bounds)/2.0);
    [self.playerView addSubview:self.loadingView];
    [self.playerView bringSubviewToFront:self.loadingView];
    [self.loadingView startAnimating];
  }
}

- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context {
  if (context == kPlayerStatusObserveContext) {
    switch (self.player.status) {
      case VBPlayerStatusUnknown: {
        break;
      }
      case VBPlayerStatusReadyToPlay: {
        [self setLoadingVisible:NO];
        
        [self.playerView setPlayer:self.player];
        [self.player play];
        break;
      }
      case VBPlayerStatusFailed: {
        UIAlertView *alert = [[UIAlertView alloc]
                              initWithTitle:@"Error"
                              message:[self.player.error localizedDescription]
                              delegate:nil
                              cancelButtonTitle:@"OK"
                              otherButtonTitles: nil];
        [alert show];
        
        // Restore playback interface
        NSMutableArray *items = [self.toolbar.items mutableCopy];
        [items replaceObjectAtIndex:kToolbarPlaybackButtonItemIndex withObject:self.playButton];
        self.toolbar.items = items;
        break;
      }
    }
  }
}

@end
