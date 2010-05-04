//
//  EAGLView.m
//  tuxracer
//
//  Created by emmanuel de roux on 22/10/08.
//  Copyright école Centrale de Lyon 2008. All rights reserved.
//


#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>


#import "EAGLView.h"
#import "AudioManager.h"

#import <CoreGraphics/CGAffineTransform.h>
#include <math.h>

#define USE_DEPTH_BUFFER 1

//variables for quiting
static CGPoint initialPoint;
static CGPoint finalPoint;

static AudioManager* manager = nil;
// A class extension to declare private methods
@interface EAGLView ()

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer *animationTimer;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

int libtuxracer_main( int argc, char **argv );

@implementation EAGLView

@synthesize context;
@synthesize animationTimer;
@synthesize animationInterval;
@synthesize displayFunction;
@synthesize idleFunction;
@synthesize keyboardFunction;
@synthesize mouseFunction;
@synthesize tuxracerLoaded=_tuxracerLoaded;



// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}

static id sharedView = nil;
+ (id)sharedView
{
	return sharedView;
}

+ (id)sharedAudioManager
{
	return manager;
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {
    
    if ((self = [super initWithCoder:coder])) {
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if (!context || ![EAGLContext setCurrentContext:context]) {
            [self release];
            return nil;
        }
        
        animationInterval = 1.0 / 60.0;
    }
	sharedView = self;
	manager = [[AudioManager alloc] init];
    
    return self;
}

- (void)setupTuxRacer
{
    [EAGLContext setCurrentContext:context];
	libtuxracer_main(0, NULL);
	if (self.idleFunction) {
		self.idleFunction();
	}
    setparam_view_mode([[NSUserDefaults standardUserDefaults] boolForKey:@"viewModeIsTuxEye"] ? 3 : 1);
    self.tuxracerLoaded = YES;
}

- (void)drawView {
    
    // Replace the implementation of this method to do your own custom drawing
    [EAGLContext setCurrentContext:context];
	
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glViewport(0, 0, backingWidth, backingHeight);
    
	if (self.displayFunction) {
		self.displayFunction();
	}
    
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
}


- (void)layoutSubviews {
    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
    [self drawView];
}


- (BOOL)createFramebuffer {
    
    glGenFramebuffersOES(1, &viewFramebuffer);
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    if (USE_DEPTH_BUFFER) {
        glGenRenderbuffersOES(1, &depthRenderbuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    }
    
    if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
    
    return YES;
}


- (void)destroyFramebuffer {
    
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    
    if(depthRenderbuffer) {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}


- (void)startAnimation {
    self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}


- (void)stopAnimation {
    self.animationTimer = nil;
}


- (void)setAnimationTimer:(NSTimer *)newTimer {
    [animationTimer invalidate];
    animationTimer = newTimer;
}


- (void)setAnimationInterval:(NSTimeInterval)interval {
    
    animationInterval = interval;
    if (animationTimer) {
        [self stopAnimation];
        [self startAnimation];
    }
}


static BOOL isPointInDisc(const CGPoint * point, const CGPoint * discCenter, CGFloat discRadius)
{
    float dx = point->x - discCenter->x;
    float dy = point->y - discCenter->y;
    return dx * dx + dy * dy < discRadius * discRadius;
}

- (BOOL)accelerateTouch:(CGPoint)pt andPush:(BOOL)push
{
    static const CGPoint accelerateCircleCenterPoint = { 345.0, 345.0 };
    static const CGFloat accelerateCircleRay = 100.0;

    if (self.keyboardFunction && (g_game.mode==RACING || g_game.mode==INTRO)) {
        if (isPointInDisc(&pt, &accelerateCircleCenterPoint, accelerateCircleRay))
        {
            self.keyboardFunction(WSK_UP,1,push?0:1,1,1);
            return YES;
        }
    }
    return NO;
}
 
- (BOOL)brakeTouch:(CGPoint)pt andPush:(BOOL)push
{
    static const CGPoint brakeCircleCenterPoint = { -20.0, 345.0 };
    static const CGFloat brakeCircleRay=100.0;

    if (self.keyboardFunction && (g_game.mode==RACING || g_game.mode==INTRO)) {
        if (isPointInDisc(&pt, &brakeCircleCenterPoint, brakeCircleRay))
        {
            self.keyboardFunction(WSK_DOWN,1,push?0:1,1,1);
            return YES;
        }
    }
    return NO;
}


- (BOOL)jumpTouch:(CGPoint)pt andPush:(BOOL)push
{
    static const CGFloat jumpCircleRay=100.0;
    static const CGPoint jumpCircleCenterPoint = { 345.0, 150.0 };

    if (self.keyboardFunction && (g_game.mode==RACING || g_game.mode==INTRO)) {
        if (isPointInDisc(&pt, &jumpCircleCenterPoint, jumpCircleRay))
        {
            self.keyboardFunction(101,0,push?0:1,1,1); //Touche E = 101
            return YES;
        }
    }
    return NO;
}


- (BOOL)quitIfNeeded:(CGPoint)pt1 to:(CGPoint)pt2
{
    if (g_game.mode==RACING) {
        if ((pt1.x-pt2.x)*(pt1.x-pt2.x)+(pt1.y-pt2.y)*(pt1.y-pt2.y)> 400.0 * 400.0)//la vrai diagonale de l'iphone fait 576.8
        {
            g_game.race_aborted = True;
            set_game_mode(GAME_OVER);
            return YES;
        }
        else {
            initialPoint=CGPointZero;
            finalPoint=CGPointZero;
        }
    }
    return NO;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGPoint pt;

	//Get all the touches.
	NSSet *allTouches = [event allTouches];

    BOOL handled = NO;
    for(UITouch *touch in allTouches) {
        if ([touch phase]==UITouchPhaseBegan) {
            pt = [touch locationInView:self];
            
            //set initialPoint for quitting implementation
            initialPoint = pt;
            
            //test if the touchf is an attempt to jump during the race
            handled |= [self jumpTouch:pt andPush:TRUE];
            
            //test if the touchf is an attempt to accelerate during the race
            handled |= [self accelerateTouch:pt andPush:TRUE];
            
            //test if the touchf is an attempt to brake during the race
            handled |= [self brakeTouch:pt andPush:TRUE];
        }
    }
    
    //Considère a touch as a mouse click release
    if (!handled && self.mouseFunction) {
        self.mouseFunction(WS_LEFT_BUTTON, WS_MOUSE_DOWN, pt.x, pt.y);
    }
}


- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event { 
	
    //g_game.race_aborted = True;
    //set_game_mode( GAME_OVER );
    
	CGPoint pt;
	BOOL handled = NO;

	//Get all the touches.
	NSSet *allTouches = [event allTouches];
    
    //On relache d'office les trois touches jump, accelerate et frein, puis on réactivera celles qui le nécessitent
    //C'est pour gérer les cas ou le mec push sur une zone sensible, glisse son doignt, et le relache sur une zone non sensible
    self.keyboardFunction(101,0,1,1,1);
    self.keyboardFunction(WSK_UP,1,1,1,1);
    self.keyboardFunction(WSK_DOWN,1,1,1,1);
	    
    for(UITouch *touch in allTouches) {
        pt = [touch locationInView:self];
        
        if ([allTouches count]==1) {
            //set finalPoint for quitting implementation
            finalPoint = pt;
            [self quitIfNeeded:initialPoint to:finalPoint];
        }
        //test if the touch is an attempt to jump during the race
        handled |= [self jumpTouch:pt andPush:(!([touch phase]==UITouchPhaseEnded))];				//(!([touch phase]==UITouchPhaseEnded)) vaut TRUE A chaque fois sauf
                                                                                     //si on est sur le touch qui vient d'être relaché
        //test if the touchf is an attempt to accelerate during the race
        handled |= [self accelerateTouch:pt andPush:(!([touch phase]==UITouchPhaseEnded))];

        //test if the touchf is an attempt to brake during the race
        handled |= [self brakeTouch:pt andPush:(!([touch phase]==UITouchPhaseEnded))];
    }
    
    //Considère a touch as a mouse click release
    if (!handled && self.mouseFunction) {
        self.mouseFunction(WS_LEFT_BUTTON, WS_MOUSE_UP, pt.x, pt.y);
    }
    
    
    if (!handled && [allTouches count] > 0 && [[[allTouches allObjects] objectAtIndex:0] tapCount]) {
        if ([[[allTouches allObjects] objectAtIndex:0] tapCount] == 2)
        {
            if (g_game.mode==RACING) 
            {
                set_game_mode(PAUSED);
            }
        }
    }
}

- (void)dealloc {
    [self stopAnimation];
    
    if ([EAGLContext currentContext] == context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [context release];  
    [super dealloc];
}

@end
