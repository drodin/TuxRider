//
//  main.m
//  tuxracer
//
//  Created by emmanuel de roux on 22/10/08.
//  Copyright école Centrale de Lyon 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

int main(int argc, char *argv[]) {
    close(stderr);
	dup(stdout);
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, nil);
    [pool release];
    return retVal;
}
