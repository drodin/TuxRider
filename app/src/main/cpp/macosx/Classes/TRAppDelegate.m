//
//  TRAppDelegate.m
//  tuxracer
//
//  Created by emmanuel de roux on 22/10/08.
//  Copyright école Centrale de Lyon 2008. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import "TRAppDelegate.h"
#import "EAGLView.h"
#import "sharedGeneralFunctions.h"
#import<AudioToolbox/AudioToolbox.h>


#define kAccelerometerFrequency    30

int libtuxracer_main( int argc, char **argv );

const char* getRessourcePath() {
	return [[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"TRWC-data"] UTF8String];
}

const char* getConfigPath() {
    NSArray * array = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);;
	return [[array objectAtIndex:0] UTF8String];
}

void preventScreenDiming(void)
{
    //Prevents screen dimming
    [UIApplication sharedApplication].idleTimerDisabled = YES;
}

void allowScreenDiming(void)
{
    //Prevents screen dimming
    [UIApplication sharedApplication].idleTimerDisabled = NO;
}

void turnScreenToLandscape() {
    EAGLView *view = [EAGLView sharedView];
    
    //set up te status bar into portrait mode (usefull for alerts, because here the status bar is hidden)
    [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
    
    // Rotates the view.
    CGAffineTransform transform = CGAffineTransformMakeRotation(3.14159/2);
    view.transform = transform;
    
    // Repositions and resizes the view.
    CGRect contentRect = CGRectMake(-80, 80, 480, 320);
    view.bounds = contentRect;
}

void turnScreenToPortrait() {
    EAGLView *view = [EAGLView sharedView];
    
    //set up te status bar into portrait mode (usefull for alerts, because here the status bar is hidden)
    [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationPortrait;
    
    // Rotates the view.
    CGAffineTransform transform = CGAffineTransformMakeRotation(0);
    view.transform = transform;
    
    // Repositions and resizes the view.
    CGRect contentRect = CGRectMake(0, 0, 320, 480);
    view.bounds = contentRect;
}

void showRegister() {
    [[TRAppDelegate sharedAppDelegate] showRegister];
}

void showHowToPlay() {
    [[TRAppDelegate sharedAppDelegate] showHowToPlay:nil];
}

void showHowToPlayAtBegining() {
    //Affiche le register Pannel si on est pas registered
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    if (![prefs boolForKey:@"registered"])
    {
        // Sometimes, this is called too soon (glView is not yet created). Wait for a run loop.
        [[TRAppDelegate sharedAppDelegate] performSelector:@selector(showHowToPlay:) withObject:nil afterDelay:0.];
    }
}

bool playerRegistered() {
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    if ([prefs boolForKey:@"registered"])
        return YES;
    else
        return NO;
}

void alertRegisterNeeded() {
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Registered users only" message:@"You need to be a registered candidate to participate to the World Challenge. If you are already registered, click on 'Login'." delegate:[TRAppDelegate sharedAppDelegate] cancelButtonTitle:@"Cancel" otherButtonTitles:@"Register",@"Login",nil];
    [alert show];
}

void vibration()
{
    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}

@implementation TRAppDelegate

@synthesize window;
@synthesize transitionView;
@synthesize glView;
@synthesize prefsWindow;
@synthesize registerWindow;
@synthesize accelerometre;

static id sharedAppDelegate = nil;
+ (id) sharedAppDelegate{
    return sharedAppDelegate;
}

- (id)init
{
    self = [super init];
    sharedAppDelegate = self;
    return self;
}

- (void)dealloc {
    sharedAppDelegate = nil;
	[window release];
	[glView release];
	[prefsWindow release];
	[registerWindow release];
	[super dealloc];
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {

    //Launch tuxRacer
    [glView setupTuxRacer];
    
    //enable multitouch
    [glView setMultipleTouchEnabled:YES];
    
    //Starts animation
	glView.animationInterval = 1.0 / 60.0;
	[glView startAnimation];
    
    //hiddes status bar
	[application setStatusBarHidden:TRUE animated:TRUE];
	
    //places glView into the transitionView
	CGRect glViewFrame = glView.frame;
    [transitionView setBackgroundColor:[UIColor blackColor]];
    glViewFrame.size.height = transitionView.frame.size.height;
	[window setBackgroundColor:[UIColor blackColor]];
	[transitionView addSubview:glView];
	[window makeKeyAndVisible];
    
    //Sets up the accelerometer
    accelerometre = [[[TRAccelerometerDelegate alloc] init] retain];
    [[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];
    [[UIAccelerometer sharedAccelerometer] setDelegate:accelerometre];
    
    //Loads License Text
    NSString* licensePath = [[[NSBundle mainBundle] pathForResource:@"license" ofType:@"html"] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
    [licenseWebView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:licensePath]]];
    
    //Loads How to Play web View
    NSString* howToPlayPath = [[[NSBundle mainBundle] pathForResource:@"how_to_play" ofType:@"html"] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
    [howToPlayWebView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:howToPlayPath]]];    
}


- (void)applicationWillResignActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 5.0;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 60.0;
}


// TransitionViewDelegate methods. 
- (void)transitionViewDidStart:(TransitionView *)view {
}
- (void)transitionViewDidFinish:(TransitionView *)view {
}
- (void)transitionViewDidCancel:(TransitionView *)view {
}

- (void)performTransition {
	if([transitionView isTransitioning]) {
		// Don't interrupt an ongoing transition
		return;
	}
    
	// If view 1 is already a subview of the transition view replace it with view 2, and vice-versa.
	if([glView superview]) {
		//Affiche les preferences
		[glView stopAnimation];
		[transitionView replaceSubview:glView withSubview:prefsWindow transition:kCATransitionPush direction:kCATransitionFromBottom duration:0.75];
	} else {
		//Affiche le jeu
        [glView startAnimation];
		[transitionView replaceSubview:prefsWindow withSubview:glView transition:kCATransitionPush direction:kCATransitionFromTop duration:0.75];
    }
}


- (IBAction)nextTransition:(id)sender {
    [self performTransition];
}

- (IBAction)showLicense:(id)sender {
    if([transitionView isTransitioning]) {
		// Don't interrupt an ongoing transition
		return;
	}
    
	// If view 1 is already a subview of the transition view replace it with view 2, and vice-versa.
	if([glView superview]) {
		//Affiche la license
		[glView stopAnimation];
		[transitionView replaceSubview:glView withSubview:licenseView transition:kCATransitionPush direction:kCATransitionFromBottom duration:0.75];
	} else {
		//Affiche le jeu
        [glView startAnimation];
		[transitionView replaceSubview:licenseView withSubview:glView transition:kCATransitionPush direction:kCATransitionFromTop duration:0.75];
    }
}

- (void)showRegister {
    if([transitionView isTransitioning]) {
		// Don't interrupt an ongoing transition
		return;
	}
    
	if([glView superview]) {
        [glView stopAnimation];
        [transitionView replaceSubview:glView withSubview:registerWindow transition:kCATransitionPush direction:kCATransitionFromBottom duration:0.75];
    } 
}

- (IBAction)showHowToPlay:(id)sender {

    if([transitionView isTransitioning]) {
		// Don't interrupt an ongoing transition
		return;
	}

	// If view 1 is already a subview of the transition view replace it with view 2, and vice-versa.
	if(![howToPlayView superview]) {
        //Affiche How To Play
		[glView stopAnimation];
        [transitionView replaceSubview:glView withSubview:howToPlayView transition:kCATransitionPush direction:kCATransitionFromBottom duration:0.75];
	} else {
		//Affiche le jeu
        [glView startAnimation];
		[transitionView replaceSubview:howToPlayView withSubview:glView transition:kCATransitionPush direction:kCATransitionFromTop duration:0.75];
    }

}

- (IBAction)openWebSite:(id)sender {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"http://www.barlow-server.com/tuxriderworldchallenge"]];
}

- (void)showPreferences:(id)sender {
    [self performTransition];
}

#pragma mark alertView delegate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
    switch (buttonIndex) {
        //cancel
        case 0:
            break;
        //Register
        case 1:
            [self showRegister];
            break;
        //Login
        case 2:
            [self showPreferences:nil];
            break;
        default:
            break;
    }
}

@end
