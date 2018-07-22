//
//  prefsDelegate.h
//  tuxracer
//
//  Created by emmanuel de Roux on 19/11/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "TransitionView.h"
#import "TRAppDelegate.h"

@interface prefsController : NSObject {
    //prefs View
    IBOutlet UISlider* musicVolume;
    IBOutlet UISlider* soundsVolume;
    IBOutlet UIButton* pays;
    IBOutlet UITextField* login;
    IBOutlet UITextField* mdp;
    IBOutlet UIView *prefsWindow;
	IBOutlet UIView *registerWindow;
    IBOutlet TransitionView *transitionWindow;
    IBOutlet UIView *listCountriesWindow;
    IBOutlet UIView* friendsManagerWindow;
    IBOutlet UISwitch* saveScoresOnline;
    IBOutlet UISwitch* displayRankings;

    IBOutlet UIButton* friendsListButton;
    
    //register view
    IBOutlet UITextField* RegLogin;
    IBOutlet UITextField* RegMdp1;
    IBOutlet UITextField* RegMdp2;
    IBOutlet UIButton* RegPays;
    IBOutlet UIView* registerView;

    IBOutlet UISegmentedControl * viewMode;

    IBOutlet UIScrollView * scrollView;
    IBOutlet UIView * contentView;

    //countries View
    IBOutlet UITableView		*myTableView;
	IBOutlet UISearchBar		*mySearchBar;
	NSArray						*listContent;			// the master content
	NSMutableArray				*filteredListContent;	// the filtered content as a result of the search
	NSMutableArray				*savedContent;			// the saved content in case the user cancels a search
}

//prefs View
@property(nonatomic,retain) UISlider* musicVolume;
@property(nonatomic,retain) UISlider* soundsVolume;
//countries View
@property (nonatomic, retain) UITableView *myTableView;
@property (nonatomic, retain) UISearchBar *mySearchBar;
@property (nonatomic, retain) NSArray *listContent;
@property (nonatomic, retain) NSMutableArray *filteredListContent;
@property (nonatomic, retain) NSMutableArray *savedContent;

//prefs View
- (IBAction) updatePrefs:(id)sender;
- (IBAction) listCountriesTransition:(id)sender;
- (IBAction) switchSaveScores:(id)sender;
- (IBAction) switchDisplayRankings:(id)sender;
- (IBAction) switchViewMode:(id)sender;
- (IBAction) displayFriendsManager:(id)sender;
- (void) loadPrefs;
- (IBAction)nextRegisterTransition:(id)sender;
- (void)performRegisterTransition;
+ (void) setDefaults;
+ (id)sharedPrefsController;

//Register View
-(int)firstVerifForm;
-(IBAction)suscribe:(id)sender;
- (void)login;
-(void) treatError:(NSString*)erreur;

@end
