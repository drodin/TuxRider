#import "ConnectionController.h"
#import "myHTTPErrors.h"

static const char* bicId = TUXRIDER_BIC_ID;


@implementation ConnectionController
@synthesize _responseSelector;

- (void)dealloc {
    NSAssert(!_connection, @"There shouldn't be a connection at this point");    
    [_alertMessage release];
    [_responseData release];
    [super dealloc];
}

- (id) init
{
    self = [super init];
    if (self != nil) {
        _responseData = [[NSMutableData data] retain];
        _alertMessage = [[UIAlertView alloc] initWithTitle:@"Please wait..." message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
    }
    return self;
}


- (NSURLConnection*) postRequest:(NSString*)body atURL:(NSString*)url withWaitMessage:(NSString*)message sendResponseTo:(id)target withMethod:(SEL)selector {
    _responseDelegate=target;
    _responseSelector=selector;
    NSMutableURLRequest* query = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
    [query setHTTPMethod:@"POST"];
    const char * varId = TUXRIDER_VAR_ID;

    NSString* queryString = [body stringByAppendingString:[NSString stringWithFormat:@"%s%s%d", varId, bicId, TUXRIDER_TOC]];
    [query setHTTPBody:[queryString dataUsingEncoding:NSUTF8StringEncoding]];
    TRDebugLog("querying %s with : \n%s\n",[url UTF8String],[queryString UTF8String]);
    
    //preparing alert message
    if (message!= NULL) {
        [_alertMessage setMessage:message];
        [_alertMessage show];
    }

    NSAssert(!_connection, @"There is already a connection initiated");    
    _connection = [[NSURLConnection alloc] initWithRequest:query delegate:self startImmediately:TRUE];
    
    return _connection;
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
	[_responseData setLength:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
	[_responseData appendData:data];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
    [_alertMessage dismissWithClickedButtonIndex:0 animated:YES];
    void (*method)(id,SEL,NSString*)= (void (*)(id,SEL,NSString*))[_responseDelegate methodForSelector:_responseSelector];
    (*method)(_responseDelegate,_responseSelector,[NSString stringWithFormat:@"%d",CONNECTION_ERROR]);
    [_connection release];
    _connection = nil;
}

- (void)connectionDidFinishLoading:(NSURLConnection *)aconnection {
    [_alertMessage dismissWithClickedButtonIndex:0 animated:YES];
    NSString* rep = [[NSString alloc] initWithData:_responseData encoding:NSUTF8StringEncoding];
    void (*method)(id,SEL,NSString*)=(void (*)(id,SEL,NSString*))[_responseDelegate methodForSelector:_responseSelector];
    (*method)(_responseDelegate,_responseSelector,rep);
    [_connection release];
    _connection = nil;
}

#pragma mark alertView delegate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
    [_connection cancel];
    [_connection release];
    _connection = nil;
}

#pragma mark TextField delegate function
- (BOOL)textFieldShouldReturn:(UITextField *)theTextField {
    [theTextField resignFirstResponder];
	return YES;
}

@end
