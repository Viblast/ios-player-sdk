//
//  PlayerView.m
//  VBPlayerDemo
//
//  Created by Ivan Ermolaev on 6/18/15.
//  Copyright (c) 2015 Viblast. All rights reserved.
//

#import "PlayerView.h"

@implementation PlayerView

+ (Class)layerClass {
  return [VBPlayerLayer class];
}

- (void)setPlayer:(VBPlayer *)player {
  [(VBPlayerLayer *)self.layer setPlayer:player];
}

- (VBPlayer *)player {
  return [(VBPlayerLayer *)self.layer player];
}
@end
