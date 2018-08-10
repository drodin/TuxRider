//
//  EAGLView.h
//  tuxracer
//
//  Created by emmanuel de roux on 22/10/08.
//  Copyright école Centrale de Lyon 2008. All rights reserved.
//


#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#include "winsys.h"
#include "sharedAudioFunctions.h"
#include "tuxracer.h"
#include "loop.h"//pour pouvoir utiliser set_game_mode

/*
This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
The view content is basically an EAGL surface you render your OpenGL scene into.
Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
*/
@interface EAGLView : UIView {
    
@private
    /* The pixel dimensions of the backbuffer */
    GLint backingWidth;
    GLint backingHeight;
    
    EAGLContext *context;
    
    /* OpenGL names for the renderbuffer and framebuffers used to render to this view */
    GLuint viewRenderbuffer, viewFramebuffer;
    
    /* OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist) */
    GLuint depthRenderbuffer;
    
    NSTimer *animationTimer;
    NSTimeInterval animationInterval;
	
	winsys_display_func_t displayFunction;
	winsys_display_func_t idleFunction;
	winsys_keyboard_func_t keyboardFunction;
    winsys_mouse_func_t mouseFunction;
    
    BOOL _tuxracerLoaded;
}

@property NSTimeInterval animationInterval;
@property winsys_display_func_t displayFunction;
@property winsys_idle_func_t idleFunction;
@property winsys_keyboard_func_t keyboardFunction;
@property winsys_mouse_func_t mouseFunction;

@property BOOL tuxracerLoaded;

+(id)sharedView;
+ (id)sharedAudioManager;

- (void)setupTuxRacer;

- (void)startAnimation;
- (void)stopAnimation;
- (void)drawView;

- (BOOL)jumpTouch:(CGPoint)pt andPush:(BOOL)push;
- (BOOL)quitIfNeeded:(CGPoint)pt1 to:(CGPoint)pt2;

@end
