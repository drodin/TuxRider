//
//  TRAppDelegate.h
//  tuxracer
//
//  Created by emmanuel de roux on 22/10/08.
//  Copyright école Centrale de Lyon 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "TransitionView.h"
#import "winsys.h"
#import "TRAccelerometerDelegate.h"
#import "prefsController.h"

@class EAGLView;

@interface TRAppDelegate : NSObject <UIApplicationDelegate> {
    IBOutlet UIWindow *window;
	TransitionView* transitionView;
    EAGLView *glView;
    IBOutlet UIView *licenseView;
    IBOutlet UIView *howToPlayView;
    IBOutlet UIWebView* licenseWebView;
    IBOutlet UIWebView* howToPlayWebView;
	NSThread * thread;
	UIView *prefsWindow;
	UIView *registerWindow;
    TRAccelerometerDelegate * accelerometre;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) IBOutlet EAGLView *glView;
@property (nonatomic, retain) IBOutlet TransitionView* transitionView;
@property (nonatomic, retain) IBOutlet UIView *prefsWindow;
@property (nonatomic, retain) IBOutlet UIView *registerWindow;
@property (nonatomic, retain) TRAccelerometerDelegate * accelerometre;


- (IBAction)nextTransition:(id)sender;
- (IBAction)showLicense:(id)sender;
- (IBAction)openWebSite:(id)sender;
- (void)showPreferences:(id)sender;
- (void)showRegister;
- (IBAction)showHowToPlay:(id)sender;
+ (id)sharedAppDelegate;

@end

