//
//  TRAccelerometerDelegate.h
//  tuxracer
//
//  Created by emmanuel de Roux on 10/11/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EAGLView.h"
#import "tuxracer.h"


@interface TRAccelerometerDelegate : NSObject {
    EAGLView* glView;
    scalar_t turnFact;
}
@property(nonatomic,retain)  EAGLView* glView;
@property(nonatomic)  scalar_t turnFact;

//Gestion de l'acceleromètre :
-(void)updateXCenter:(float) x_acc;
- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration;
-(scalar_t)calculateTurnFact:(UIAcceleration*)acceleration;
@end
