/*

File: TransitionView.m
Abstract: A convenience class that allows to replace a subview by another one
using one of the built-in CoreAnimation transitions.

Version: 1.6

Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc.
("Apple") in consideration of your agreement to the following terms, and your
use, installation, modification or redistribution of this Apple software
constitutes acceptance of these terms.  If you do not agree with these terms,
please do not use, install, modify or redistribute this Apple software.

In consideration of your agreement to abide by the following terms, and subject
to these terms, Apple grants you a personal, non-exclusive license, under
Apple's copyrights in this original Apple software (the "Apple Software"), to
use, reproduce, modify and redistribute the Apple Software, with or without
modifications, in source and/or binary forms; provided that if you redistribute
the Apple Software in its entirety and without modifications, you must retain
this notice and the following text and disclaimers in all such redistributions
of the Apple Software.
Neither the name, trademarks, service marks or logos of Apple Inc. may be used
to endorse or promote products derived from the Apple Software without specific
prior written permission from Apple.  Except as expressly stated in this notice,
no other rights or licenses, express or implied, are granted by Apple herein,
including but not limited to any patent rights that may be infringed by your
derivative works or by other works in which the Apple Software may be
incorporated.

The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
COMBINATION WITH YOUR PRODUCTS.

IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR
DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF
CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF
APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Copyright (C) 2008 Apple Inc. All Rights Reserved.

*/

#import <QuartzCore/QuartzCore.h>
#import "TransitionView.h"

#define kAnimationKey @"transitionViewAnimation"

@implementation TransitionView

@synthesize delegate, transitioning;

// Method to replace a given subview with another using a specified transition type, direction, and duration
- (void)replaceSubview:(UIView *)oldView withSubview:(UIView *)newView transition:(NSString *)transition direction:(NSString *)direction duration:(NSTimeInterval)duration {
	
	// If a transition is in progress, do nothing
	if(transitioning) {
		return;
	}
	
	NSArray *subViews = [self subviews];
	NSUInteger index;
	
	if ([oldView superview] == self) {
		// Find the index of oldView so that we can insert newView at the same place
		for(index = 0; [subViews objectAtIndex:index] != oldView; ++index) {}
		[oldView removeFromSuperview];
	}
	
	// If there's a new view and it doesn't already have a superview, insert it where the old view was
	if (newView && ([newView superview] == nil))
		[self insertSubview:newView atIndex:index];
	
	
	// Set up the animation
	CATransition *animation = [CATransition animation];
	[animation setDelegate:self];
	
	// Set the type and if appropriate direction of the transition, 
	if (transition == kCATransitionFade) {
		[animation setType:kCATransitionFade];
	} else {
		[animation setType:transition];
		[animation setSubtype:direction];
	}
	
	// Set the duration and timing function of the transtion -- duration is passed in as a parameter, use ease in/ease out as the timing function
	[animation setDuration:duration];
	[animation setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
	
	[[self layer] addAnimation:animation forKey:kAnimationKey];
}


// Not used in this example, but may be useful in your own project
- (void)cancelTransition {
	// Remove the animation -- cleanup performed in animationDidStop:finished:
	[[self layer] removeAnimationForKey:kAnimationKey];
}


- (void)animationDidStart:(CAAnimation *)animation {
	
	transitioning = YES;
    
	// Record the current value of userInteractionEnabled so it can be reset in animationDidStop:finished:
	wasEnabled = self.userInteractionEnabled;
	
	// If user interaction is not already disabled, disable it for the duration of the animation
	if (wasEnabled) {
		self.userInteractionEnabled = NO;
    }
    
	// Inform the delegate if the delegate implements the corresponding method
	if([delegate respondsToSelector:@selector(transitionViewDidStart:)]) {
		[delegate transitionViewDidStart:self];
    }
}


- (void)animationDidStop:(CAAnimation *)animation finished:(BOOL)finished {
	
	transitioning = NO;
	
	// Reset the original value of userInteractionEnabled
	if (wasEnabled) {
		self.userInteractionEnabled = YES;
	}
    
	// Inform the delegate if it implements the corresponding method
	if (finished) {
		if ([delegate respondsToSelector:@selector(transitionViewDidFinish:)]) {
			[delegate transitionViewDidFinish:self];
        }
	}
	else {
		if ([delegate respondsToSelector:@selector(transitionViewDidCancel:)]) {
			[delegate transitionViewDidCancel:self];
        }
	}
}


@end
