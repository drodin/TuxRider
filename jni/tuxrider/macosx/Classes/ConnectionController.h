#import <UIKit/UIKit.h>
#import <CoreGraphics/CoreGraphics.h>
#import "prefsController.h"

extern NSString * tuxRiderRootServer;

@interface ConnectionController : NSObject {
    NSMutableData *_responseData;
    UIAlertView* _alertMessage;
    NSURLConnection* _connection;
	NSURL *_baseURL;
    id _responseDelegate;
    SEL _responseSelector;
}
@property(assign) SEL _responseSelector;

- (NSURLConnection*) postRequest:(NSString*)body atURL:(NSString*)url withWaitMessage:(NSString*)message sendResponseTo:(id)target withMethod:(SEL)selector;

@end

