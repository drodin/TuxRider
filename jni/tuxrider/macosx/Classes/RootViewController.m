//
//  RootViewController.m
//  test
//
//  Created by emmanuel de roux on 04/11/08.
//  Copyright école Centrale de Lyon 2008. All rights reserved.
//

#import "RootViewController.h"

#import "FlipsideViewController.h"


@implementation RootViewController

@synthesize infoButton;
@synthesize flipsideNavigationBar;
@synthesize eaglViewController;
@synthesize flipsideViewController;


- (void)viewDidLoad {
    
    [super viewDidLoad];
	EAGLViewController *viewController = [[EAGLViewController alloc] initWithNibName:@"EAGLView" bundle:nil];
    self.eaglViewController = viewController;
    [viewController release];
    
    [self.view insertSubview:eaglViewController.view belowSubview:infoButton];
}


- (void)loadFlipsideViewController {
    
    FlipsideViewController *viewController = [[FlipsideViewController alloc] initWithNibName:@"prefsWindows" bundle:nil];
    self.flipsideViewController = viewController;
    [viewController release];
    
    // Set up the navigation bar
    UINavigationBar *aNavigationBar = [[UINavigationBar alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 44.0)];
    aNavigationBar.barStyle = UIBarStyleBlackOpaque;
    self.flipsideNavigationBar = aNavigationBar;
    [aNavigationBar release];
    
    UIBarButtonItem *buttonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(toggleView)];
    UINavigationItem *navigationItem = [[UINavigationItem alloc] initWithTitle:@"test"];
    navigationItem.rightBarButtonItem = buttonItem;
    [flipsideNavigationBar pushNavigationItem:navigationItem animated:YES];
    [navigationItem release];
    [buttonItem release];
}


- (IBAction)toggleView {    
    /*
     This method is called when the info or Done button is pressed.
     It flips the displayed view from the main view to the flipside view and vice-versa.
     */
    if (flipsideViewController == nil) {
        [self loadFlipsideViewController];
    }
    
    UIView *eaglView = eaglViewController.view;
    UIView *flipsideView = flipsideViewController.view;
    
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:0.5];
    [UIView setAnimationTransition:([eaglView superview] ? UIViewAnimationTransitionFlipFromRight : UIViewAnimationTransitionFlipFromLeft) forView:self.view cache:YES];
    
    if ([eaglView superview] != nil) {
        [flipsideViewController viewWillAppear:YES];
        [eaglViewController viewWillDisappear:YES];
        [eaglView removeFromSuperview];
        [infoButton removeFromSuperview];
        [self.view addSubview:flipsideView];
        [self.view insertSubview:flipsideNavigationBar aboveSubview:flipsideView];
        [eaglViewController viewDidDisappear:YES];
        [flipsideViewController viewDidAppear:YES];

    } else {
        [eaglViewController viewWillAppear:YES];
        [flipsideViewController viewWillDisappear:YES];
        [flipsideView removeFromSuperview];
        [flipsideNavigationBar removeFromSuperview];
        [self.view addSubview:eaglView];
        [self.view insertSubview:infoButton aboveSubview:eaglViewController.view];
        [flipsideViewController viewDidDisappear:YES];
        [eaglViewController viewDidAppear:YES];
    }
    [UIView commitAnimations];
}



- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


- (void)dealloc {
    [infoButton release];
    [flipsideNavigationBar release];
    [eaglViewController release];
    [flipsideViewController release];
    [super dealloc];
}


@end
