//
//  RootViewController.h
//  test
//
//  Created by emmanuel de roux on 04/11/08.
//  Copyright école Centrale de Lyon 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EAGLViewController.h"

@class EAGLViewController;
@class FlipsideViewController;

@interface RootViewController : UIViewController {

    UIButton *infoButton;
    EAGLViewController *eaglViewController;
    FlipsideViewController *flipsideViewController;
    UINavigationBar *flipsideNavigationBar;
}

@property (nonatomic, retain) IBOutlet UIButton *infoButton;
@property (nonatomic, retain) EAGLViewController *eaglViewController;
@property (nonatomic, retain) UINavigationBar *flipsideNavigationBar;
@property (nonatomic, retain) FlipsideViewController *flipsideViewController;

- (IBAction)toggleView;

@end
