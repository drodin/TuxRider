//
//  TRAccelerometerDelegate.m
//  tuxracer
//
//  Created by emmanuel de Roux on 10/11/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import "TRAccelerometerDelegate.h"

//Horizontal parameters
#define kYMax					    0.45 //between 0 and 1.0
#define kYSensibility               0.1 //between 0 and kYMax
//shake paramaters, for tricks
#define kAccelerationThreshold       1.50

static enum {
    left,
    right,
    center
}  y_state ;

static enum {
    up,
    down,
    plane
}  x_state ;

static enum {
    shaking,
    idle
}  shake_state ;

static TRAccelerometerDelegate *sharedAccelerationDelegate=nil;

scalar_t accelerometerTurnFact() {
    return [sharedAccelerationDelegate turnFact];
}

@implementation TRAccelerometerDelegate

@synthesize glView;
@synthesize turnFact;

- (id) init
{
    self = [super init];
    if (self != nil) {
        glView = [EAGLView sharedView];
        sharedAccelerationDelegate = self;
    }
    return self;
}

- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration {
    //On n'utilise l'acceleromètre que si on est en mode intro ou en mode racing
    if (g_game.mode==RACING)
    {
        //printf("x : %f ; y : %f ; z : %f \n",acceleration.x,acceleration.y,acceleration.z);
        self.turnFact=[self calculateTurnFact:acceleration];
        
        //touche droite
        if (acceleration.y<-kYSensibility) {
            //Si la touche gauche était enfoncée, on lui dit d'abord de la relacher avant d'enfoncer la touche droite
            //C au cas ou on passe tellement vite de gauche à droite que ca ait pas le temps de passer par center
            if (y_state==left) {
                glView.keyboardFunction(WSK_LEFT,1,1,1,1);
                y_state=center;
            }
            //Si aller vers la droite est djà activé, pas besoin de le refaire
            if (y_state != right) {
                glView.keyboardFunction(WSK_RIGHT,1,0,1,1);
            }
            y_state=right;
        }
        else
            //touche gauche 
            if (acceleration.y>kYSensibility) {
                //ibidem
                if (y_state==right) {
                    glView.keyboardFunction(WSK_RIGHT,1,1,1,1);
                    y_state=center;
                }
                //ibidem
                if (y_state != left) {
                    glView.keyboardFunction(WSK_LEFT,1,0,1,1);
                }
                y_state=left;
            } 
        //ni touche gauche ni touche droite
            else { 
                if (y_state==right) {
                    glView.keyboardFunction(WSK_RIGHT,1,1,1,1);
                }
                else if (y_state==left) {
                    glView.keyboardFunction(WSK_LEFT,1,1,1,1);
                }
                
                y_state=center;
            }
          
        //Detects shakes and do some tricks while jumping
        if ((fabsf(acceleration.z) > kAccelerationThreshold || fabsf(acceleration.y) > kAccelerationThreshold || fabsf(acceleration.x) > kAccelerationThreshold) && shake_state!=shaking) {
            shake_state=shaking;
            glView.keyboardFunction(100,0,0,1,1);  //touche D = 100
        }
        if ((fabsf(acceleration.z) < kAccelerationThreshold || fabsf(acceleration.y) > kAccelerationThreshold || fabsf(acceleration.x) > kAccelerationThreshold) && shake_state==shaking) {
            shake_state=idle;
            glView.keyboardFunction(100,0,1,1,1);  //touche D = 100
        }
    }
}

-(scalar_t)calculateTurnFact:(UIAcceleration*)acceleration
{
    if ((acceleration.y)>kYMax) return -1.0;
    if ((acceleration.y)<-kYMax) return 1.0;
    if ((acceleration.y)>kYSensibility)  return -(kYSensibility+(1.0-kYSensibility)*(acceleration.y-kYSensibility)/(kYMax-kYSensibility));
    if ((acceleration.y)<-kYSensibility) return -(-kYSensibility+(-1.0+kYSensibility)*(acceleration.y+kYSensibility)/(-kYMax+kYSensibility));
    return 0.0;
}

@end
