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
  return [VBDisplayLayer class];
}

- (VBDisplayLayer *)displayLayer {
  return (VBDisplayLayer *)self.layer;
}
@end
