//
//  TRFriendsManagerDelegate.m
//  tuxracer
//
//  Created by emmanuel de Roux on 06/12/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import "TRFriendsManagerDelegate.h"
#import "prefsController.h"
#import "ConnectionController.h"


@implementation TRFriendsManagerDelegate

- (id) init
{
    self = [super init];
    if (self != nil) {
        [prefsController setDefaults];
         NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
        _friendsList = [[NSMutableArray arrayWithArray:[prefs objectForKey:@"friendsList"]] retain];
    }
    return self;
}

- (void) dealloc
{
    [_loginBeingAdded release];
    [_friendsList release];
    [super dealloc];
}


- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1; 
}

#pragma mark tableView delegate
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if ([_friendsList count]==0) return 1;
    return [_friendsList count];
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    switch (editingStyle) {
        case UITableViewCellEditingStyleDelete:
            [_friendsList removeObjectAtIndex:indexPath.row];
            [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObjects:indexPath,nil] withRowAnimation:UITableViewRowAnimationTop];
            break;
        case UITableViewCellEditingStyleInsert:
            [insertTextField resignFirstResponder];
            [self addFriend:[insertTextField text]];
            break;
        default:
            break;
    }
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    return TRUE;
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath {
    if (indexPath.row>0)
        return UITableViewCellEditingStyleDelete;
    else return UITableViewCellEditingStyleInsert;
    
}

- (UITableViewCellAccessoryType)tableView:(UITableView *)tableView accessoryTypeForRowWithIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellSeparatorStyleNone;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [insertTextField resignFirstResponder];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell;
    if (![[_friendsList objectAtIndex:indexPath.row] isEqualToString:@"*addCell*"]) {
        cell = [tableView dequeueReusableCellWithIdentifier:@"cellID"];
        if (cell == nil)
        {
            cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"cellID"] autorelease];
            cell.selectionStyle = UITableViewCellSelectionStyleBlue;
        }
        if ([[_friendsList objectAtIndex:0] isEqualToString:@"*empty*"]) cell.text=@"Tap \"Edit\" to Add friends.";
        else cell.text = [_friendsList objectAtIndex:indexPath.row];
        
    } else cell = insertCell;
    
    return cell;
}

#pragma mark Friends Manager Functions


- (IBAction) edit:(id)sender {
    [sender setTitle:@"Ok"];
    //Go into edit mode
    if (![friendsTableView isEditing]) {
        [sender setTitle:@"Save"];
        
        //cas vide
        if ([[_friendsList objectAtIndex:0] isEqualToString:@"*empty*"]) {
            [_friendsList removeObjectAtIndex:0];
            //[friendsTableView deleteRowsAtIndexPaths:[NSArray arrayWithObjects:[NSIndexPath indexPathForRow:1 inSection:0],nil] withRowAnimation:UITableViewRowAnimationTop];
        }
        
        [_friendsList insertObject:@"*addCell*" atIndex:0];
        [friendsTableView insertRowsAtIndexPaths:[NSArray arrayWithObjects:[NSIndexPath indexPathForRow:0 inSection:0],nil] withRowAnimation:UITableViewRowAnimationTop];
        [insertTextField setText:@""];
        [insertTextField resignFirstResponder];
        [friendsTableView setEditing:TRUE animated:TRUE];
    }
    //Save modifications
    else {
        [sender setTitle:@"Edit"];
        
        //cas vide (Tous les amis ont été suprimés)
        if ([[_friendsList objectAtIndex:([_friendsList count]-1)] isEqualToString:@"*addCell*"]) {
            [_friendsList insertObject:@"*empty*" atIndex:0];
            [friendsTableView insertRowsAtIndexPaths:[NSArray arrayWithObjects:[NSIndexPath indexPathForRow:0 inSection:0],nil] withRowAnimation:UITableViewRowAnimationTop];
            [_friendsList removeObjectAtIndex:1];
            [friendsTableView deleteRowsAtIndexPaths:[NSArray arrayWithObjects:[NSIndexPath indexPathForRow:1 inSection:0],nil] withRowAnimation:UITableViewRowAnimationTop];
        }
        else {
            [_friendsList removeObjectAtIndex:0];
            [friendsTableView deleteRowsAtIndexPaths:[NSArray arrayWithObjects:[NSIndexPath indexPathForRow:0 inSection:0],nil] withRowAnimation:UITableViewRowAnimationTop];
        }
        NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
        [prefs setObject:_friendsList forKey:@"friendsList"];
        [friendsTableView setEditing:FALSE animated:TRUE];
    }
}

-(void)treatData:(NSString*)data {
    int error = [data intValue];
    if (error == LOGIN_EXISTS) {
        [_friendsList insertObject:_loginBeingAdded atIndex:1];
        [friendsTableView insertRowsAtIndexPaths:[NSArray arrayWithObjects:[NSIndexPath indexPathForRow:1 inSection:0],nil] withRowAnimation:UITableViewRowAnimationTop];
        [insertTextField setText:@""];
    } else {
        [PC treatError:data];
        [insertTextField setText:_loginBeingAdded];
    }
}

- (void)addFriend:(NSString*)login {
    if (![login isEqualToString:@""] && ![self isAlreadyFriend:login]) {
        if(!_loginBeingAdded) [_loginBeingAdded release];
        _loginBeingAdded=[login retain];
        [insertTextField setText:_loginBeingAdded];
        ConnectionController* conn = [[[ConnectionController alloc] init] autorelease];
        NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
        NSString* queryString = [NSString stringWithFormat:@"login=%@",login];
        [conn postRequest:queryString atURL:[tuxRiderRootServer stringByAppendingString:@"checkLogin.php"] withWaitMessage:(NSString*)@"Checking login..." sendResponseTo:self withMethod:@selector(treatData:)];        
    } 
    else 
    {
        [insertTextField setText:@""];
    }
}

- (BOOL) isAlreadyFriend:(NSString*)friend {
    for (NSString* name in _friendsList) {
        if ([name isEqualToString:friend]) 
        {
            UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Error !" message:[NSString stringWithFormat:@"%@ is already in your friends list.",friend] delegate:nil cancelButtonTitle:@"Ok" otherButtonTitles:nil];
            [alert show];
            return YES;
        }
    }
    return NO;
}

#pragma mark transitions Functions

- (IBAction) goBack:(id)sender{
    [editButton setTitle:@"Edit"];
    
    //If user was in editing mode while quitting, ask if he wants to save
    if ([friendsTableView isEditing]) {
        //UIAlertView* alertMessage = [[[UIAlertView alloc] initWithTitle:@"Save changes" message:@"Do you want to save your changes ?" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Save",nil] autorelease];
        UIActionSheet* alert = [[UIActionSheet alloc] initWithTitle:@"Do you want to save your changes ?" delegate:self cancelButtonTitle:@"No" destructiveButtonTitle:@"Yes" otherButtonTitles:nil];
        [alert setActionSheetStyle:UIActionSheetStyleBlackOpaque];
        [alert showInView:friendsTableView];
    }
    else {
        [PC displayFriendsManager:sender];
    }
}

#pragma mark actionSheet delegate

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
    NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
    switch (buttonIndex) {
        //yes
        case 0:
            [self edit:editButton];
            [PC displayFriendsManager:nil];
            break;
        //no
        case 1:
            //suprime la case d'ajout d'un nouvel ami
            [friendsTableView deleteRowsAtIndexPaths:[NSArray arrayWithObjects:[NSIndexPath indexPathForRow:([_friendsList count]-1) inSection:0],nil] withRowAnimation:UITableViewRowAnimationTop];
            [_friendsList release];
            _friendsList = [[NSMutableArray arrayWithArray:[prefs objectForKey:@"friendsList"]] retain];
            [insertTextField setText:@""];
            [friendsTableView setEditing:FALSE animated:TRUE];
            [friendsTableView reloadData];
            [PC displayFriendsManager:nil];
            break;
        default:
            break;
    }
}

#pragma mark TextField delegate function
- (BOOL)textFieldShouldReturn:(UITextField *)theTextField {
    [theTextField resignFirstResponder];
    [self addFriend:[theTextField text]];
    return YES;
}

@end
