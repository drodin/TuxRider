//
//  prefsDelegate.m
//  tuxracer
//
//  Created by emmanuel de Roux on 19/11/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

int plyrWantsToSaveOrDisplayRankingsAfterRace() {
    //NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
    //return ([prefs boolForKey:@"saveScoresAfterRace"]||[prefs boolForKey:@"displayRankingsAfterRace"]);
    return 0;
}

int plyrWantsToDisplayRankingsAfterRace() {
    return 0;
}

/*
@implementation prefsController
@synthesize musicVolume,soundsVolume;
@synthesize listContent, filteredListContent, savedContent, myTableView, mySearchBar;

+ (id)sharedPrefsController {
    return self;
}

- (void)awakeFromNib
{
    [scrollView addSubview:contentView];
    [scrollView setContentSize:contentView.frame.size];

    //load prefs
    [self loadPrefs];
    
    // create the master list
    NSString* allCountries = [NSString stringWithContentsOfFile:[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"countries.txt"] encoding:NSUTF8StringEncoding error:NULL];
    listContent = [[[allCountries componentsSeparatedByString:@"|||"] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)] retain];
    
    // create our filtered list that will be the data source of our table, start its content from the master "listContent"
    filteredListContent = [[NSMutableArray alloc] initWithCapacity: [listContent count]];
    [filteredListContent addObjectsFromArray: listContent];
    
    // this stored the current list in case the user cancels the filtering
    savedContent = [[NSMutableArray alloc] initWithCapacity: [listContent count]]; 
    
    // don't get in the way of user typing
    mySearchBar.autocorrectionType = UITextAutocorrectionTypeNo;
    mySearchBar.autocapitalizationType = UITextAutocapitalizationTypeNone;
    mySearchBar.showsCancelButton = NO;
    
}

- (void)dealloc
{
    [myTableView release];
    [mySearchBar release];
    
    [listContent release];
    [filteredListContent release];
    [savedContent release];
    
    [super dealloc];
}


#pragma mark prefs functions

- (IBAction) updatePrefs:(id)sender {
    NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
    //indicates that prefs have been updated at least once
    [prefs setObject:@"yes" forKey:@"yetUpdatedOnce"];
    
    //Volume prefs
    [prefs setFloat:musicVolume.value forKey:@"musicVolume"];
    [[EAGLView sharedAudioManager] setMusicGainFactor:musicVolume.value];
    [prefs setFloat:soundsVolume.value forKey:@"soundsVolume"];
    [[EAGLView sharedAudioManager] setSoundGainFactor:soundsVolume.value];
}

-(void) loadPrefs {
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    //If prefs are loaded for first time, set defaults
    [prefsController setDefaults];
    //loads prefs
    //username
    
    [login setText:[prefs valueForKey:@"username"]];
    //password
    [mdp setText:[prefs valueForKey:@"password"]];
    //switch "save score online"
    [saveScoresOnline setEnabled:[prefs boolForKey:@"registered"]];
    [saveScoresOnline setOn:[prefs boolForKey:@"saveScoresAfterRace"]];
    //switch "Display Rankings"
    [displayRankings setEnabled:[prefs boolForKey:@"registered"]];
    [displayRankings setOn:[prefs boolForKey:@"displayRankingsAfterRace"]];
    //enable friends list button
    [friendsListButton setEnabled:[prefs boolForKey:@"registered"]];

    [viewMode setSelectedSegmentIndex:[prefs boolForKey:@"viewModeIsTuxEye"] ? 0 : 1 ];

    musicVolume.value = [prefs floatForKey:@"musicVolume"];
    soundsVolume.value = [prefs floatForKey:@"soundsVolume"];
}

static const int kTRPreferencesVersion = 2;

+(void) setDefaults {
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    if ([prefs objectForKey:@"yetUpdatedOnce"]==nil || [[prefs objectForKey:@"version"] intValue] < kTRPreferencesVersion) {
        [prefs setObject:@"yetUpdated" forKey:@"yetUpdatedOnce"];
        [prefs setFloat:0.5 forKey:@"musicVolume"];
        [prefs setFloat:0.5 forKey:@"soundsVolume"];
        [prefs setValue:@"" forKey:@"username"];
        [prefs setValue:@"" forKey:@"password"];
        [prefs setValue:@"" forKey:@"pays"];
        [prefs setBool:NO forKey:@"registered"];
        [prefs setBool:NO forKey:@"saveScoresAfterRace"];
        [prefs setBool:NO forKey:@"displayRankingsAfterRace"];
        NSArray* array = [NSArray arrayWithObjects:@"*empty*",nil];
        [prefs setObject:array forKey:@"friendsList"];
        [prefs setObject:@"" forKey:@"rankingCache"];
        [prefs setObject:[NSNumber numberWithInt:kTRPreferencesVersion] forKey:@"version"];
        //Pour chaque piste dont les rankings auront été consultés, un objet cache sera créé pour la clé "Nom de la piste"
        [prefs setInteger:0 forKey:@"needsSync"];
    }
}

- (IBAction) switchSaveScores:(id)sender{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    [prefs setBool:[saveScoresOnline isOn] forKey:@"saveScoresAfterRace"];
}

- (IBAction) switchViewMode:(id)sender
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    BOOL isTuxEyeMode = [viewMode selectedSegmentIndex] == 0;
    [prefs setBool:isTuxEyeMode forKey:@"viewModeIsTuxEye"];
    if([[EAGLView sharedView] tuxracerLoaded])
        setparam_view_mode(isTuxEyeMode ? 3 : 1);
}

- (IBAction) switchDisplayRankings:(id)sender{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    [prefs setBool:[displayRankings isOn] forKey:@"displayRankingsAfterRace"];
}
#pragma mark transitions

- (IBAction)nextRegisterTransition:(id)sender {
    [self performRegisterTransition];
}

- (void)performRegisterTransition {
    
	[UIView setAnimationDelegate:self];
	[UIView beginAnimations:nil context:nil];
	[UIView setAnimationDuration:0.75];
	
	[UIView setAnimationTransition:([prefsWindow superview] ?
									UIViewAnimationTransitionFlipFromLeft : UIViewAnimationTransitionFlipFromRight)
						   forView:transitionWindow cache:YES];
    
	if ([prefsWindow superview])
	{
        [RegLogin setText:@""];
        [RegMdp1 setText:@""];
        [RegMdp2 setText:@""];
        [RegPays setTitle:@"Choose one" forState:UIControlStateNormal];
		[prefsWindow removeFromSuperview];
		[transitionWindow addSubview:registerWindow];
	}
	else
	{
		[registerWindow removeFromSuperview];
		[transitionWindow addSubview:prefsWindow];
	}
	
	[UIView commitAnimations];
    
}

- (IBAction) listCountriesTransition:(id)sender{
    if([registerWindow superview]) {
        [transitionWindow replaceSubview:registerWindow withSubview:listCountriesWindow transition:kCATransitionPush direction:kCATransitionFromRight duration:0.75];
    } else {
        [transitionWindow replaceSubview:listCountriesWindow withSubview:registerWindow transition:kCATransitionPush direction:kCATransitionFromLeft duration:0.75];
    }
}

- (IBAction) displayFriendsManager:(id)sender {
    if([transitionWindow isTransitioning]) {
		// Don't interrupt an ongoing transition
		return;
	}
	// If view 1 is already a subview of the transition view replace it with view 2, and vice-versa.
	if([prefsWindow superview]) {
		[transitionWindow replaceSubview:prefsWindow withSubview:friendsManagerWindow transition:kCATransitionPush direction:kCATransitionFromRight duration:0.75];
	} else {
		[transitionWindow replaceSubview:friendsManagerWindow withSubview:prefsWindow transition:kCATransitionPush direction:kCATransitionFromLeft duration:0.75];
    }
}

#pragma mark Register Window Functions

//Traite les différents cas d'erreurs possibles suite à une tentative de connection dans le cadre des preférences
-(void) treatError:(NSString*)erreur{
    int err = [erreur intValue];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Error !" message:@"" delegate:nil cancelButtonTitle:@"Ok" otherButtonTitles:nil];
    NSUserDefaults *preferences = [NSUserDefaults standardUserDefaults];
    switch (err) {
        case LOGIN_TOO_LONG:
            [alert setMessage:@"Login must be between 1 and 15 characters!"];
            [alert show];
            [RegLogin becomeFirstResponder];
            break;
        case DIFFERENTS_PASSWORDS:
            [alert setMessage:@"Passwords entered are differents!"];
            [alert show];
            [RegMdp1 becomeFirstResponder];
            break;
        case PASSWORD_TOO_LONG:
            [alert setMessage:@"Password must be less or equal than 10 characters!"];
            [alert show];
            [RegMdp1 becomeFirstResponder];
            break;
        case PASSWORD_TOO_SHORT:
            [alert setMessage:@"Password must be more or equal than 5 characters"];
            [alert show];
            [RegMdp1 becomeFirstResponder];
            break;
        case LOGIN_ALREADY_EXISTS:
            [alert setMessage:@"The login you entered already exists! Please choose another."];
            [alert show];
            [RegLogin becomeFirstResponder];
            break;
        case INVALID_EMAIL:
            [alert setMessage:@"The email you entered has an invalid format. Note that you are not obliged to enter an email!"];
            [alert show];
            break;
        case COUNTRY_EMPTY:
            [alert setMessage:@"Choose your country!"];
            [alert show];
            [self listCountriesTransition:self];
            break;
        case SERVER_ERROR:
            [alert setMessage:@"Internal server error! Please try again Later."];
            [alert show];
            break;
        case CONNECTION_ERROR:
            [alert setMessage:@"Check your network connection and try again."];
            [alert show];
            break;
        case NEEDS_NEW_VERSION:
            [alert setMessage:@"For security reasons, you first need to update Tux Rider World Challenge. Go to the App Store to do the update."];
            [alert show];
            break;
        case SUSCRIBTION_SUCCESSFUL:
            [alert setTitle:@"Congratulations !"];
            [alert setMessage:@"You can now compare your best score with your friends and with people around the world!"];
            [alert show];
            [preferences setValue:[RegLogin text] forKey:@"username"];
            [preferences setValue:[RegMdp1 text] forKey:@"password"];
            [preferences setValue:[RegPays titleForState:UIControlStateNormal] forKey:@"pays"];
            [preferences setBool:TRUE forKey:@"registered"];
            [preferences setBool:TRUE forKey:@"saveScoresAfterRace"];
            [preferences setBool:TRUE forKey:@"displayRankingsAfterRace"];
            [self loadPrefs];
            [self performRegisterTransition];
            break;
        case LOGIN_SUCCESSFUL:
            //NSUserDefaults *preferences = [NSUserDefaults standardUserDefaults];
            [preferences setValue:[login text] forKey:@"username"];
            [preferences setValue:[mdp text] forKey:@"password"];
            [preferences setBool:TRUE forKey:@"registered"];
            [preferences setBool:TRUE forKey:@"saveScoresAfterRace"];
            [preferences setBool:TRUE forKey:@"displayRankingsAfterRace"];
            [alert setTitle:@"Welcome !"];
            [alert setMessage:@"You can now compare your best score with your friends and with people around the world !"];
            [alert show];
            [self loadPrefs];
            break;
        case LOGIN_ERROR:
            [alert setMessage:@"Wrong login/password."];
            [self loadPrefs];
            [alert show];
            break;
        case LOGIN_DONT_EXISTS:
            [alert setMessage:@"Nobody is registered with this login."];
            [alert show];
            break;
        case BAD_LOGIN_CHARACTERS:
            [alert setMessage:@"Use only alphanumeric characters, \"-\", and \"_\" for the login."];
            [alert show];
            break;
        case BAD_MDP_CHARACTERS:
            [alert setMessage:@"Use only alphanumeric characters, \"-\", and \"_\" for the password."];
            [alert show];
            break;
        default:
            [alert setMessage:@"Unknown error!"];
            [alert show];
            break;
    }
}

//vérifie une première fois les données avant de les envoyer
-(int) firstVerifForm{
    if ([[RegLogin text] length]>30||[[RegLogin text] length]<1) { return LOGIN_TOO_LONG; }
    if (![[RegMdp1 text] isEqualToString:[RegMdp2 text]]) { return DIFFERENTS_PASSWORDS; }
    if ([[RegMdp1 text] length]>30) {return PASSWORD_TOO_LONG; }
    if ([[RegMdp2 text] length]<5) {return PASSWORD_TOO_SHORT; }
    if ([[RegPays titleForState:UIControlStateNormal] isEqualToString:@"Choose one"]) { return COUNTRY_EMPTY; }
    return DATA_OK;
}

//envoie les données pour s'inscrire
- (IBAction)suscribe:(id)sender {
    int error = [self firstVerifForm];
    if (error == DATA_OK)
    {
        ConnectionController* conn = [[[ConnectionController alloc] init] autorelease];
        NSString* queryString = [NSString stringWithFormat:@"login=%@&mdp1=%@&mdp2=%@&pays=%@",[RegLogin text],[RegMdp1 text],[RegMdp2 text],[RegPays titleForState:UIControlStateNormal]];
        [conn postRequest:queryString atURL:[tuxRiderRootServer stringByAppendingString:@"save.php"] withWaitMessage:(NSString*)@"Sending registration data..." sendResponseTo:self withMethod:@selector(treatError:)];
    } else [self treatError:[NSString stringWithFormat:@"%d",error]];
}

- (void)login {
    ConnectionController* conn = [[[ConnectionController alloc] init] autorelease];
    NSString* queryString = [NSString stringWithFormat:@"login=%@&mdp=%@",[login text],[mdp text]];
    [conn postRequest:queryString atURL:[tuxRiderRootServer stringByAppendingString:@"login.php"] withWaitMessage:(NSString*)@"Checking login/password..." sendResponseTo:self withMethod:@selector(treatError:)];
}

#pragma mark tableView delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    TRDebugLog("%s selected !\n",[[[tableView cellForRowAtIndexPath:indexPath] text] UTF8String]);
    [pays setTitle:[[tableView cellForRowAtIndexPath:indexPath] text] forState:UIControlStateNormal];
    [self listCountriesTransition:nil];
}


#pragma mark UITableViewDataSource
//friends table view
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [filteredListContent count];
}

- (UITableViewCellAccessoryType)tableView:(UITableView *)tableView accessoryTypeForRowWithIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellAccessoryNone;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"cellID"];
    if (cell == nil)
    {
        cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"cellID"] autorelease];
        cell.selectionStyle = UITableViewCellSelectionStyleBlue;
    }
    
    cell.text = [filteredListContent objectAtIndex:indexPath.row];
    
    return cell;
}

#pragma mark UISearchBarDelegate

- (void)searchBarTextDidBeginEditing:(UISearchBar *)searchBar
{
    // only show the status bar's cancel button while in edit mode
    mySearchBar.showsCancelButton = YES;
    
    // flush and save the current list content in case the user cancels the search later
    [savedContent removeAllObjects];
    [savedContent addObjectsFromArray: filteredListContent];
}

- (void)searchBarTextDidEndEditing:(UISearchBar *)searchBar
{
    mySearchBar.showsCancelButton = NO;
}

- (void)searchBar:(UISearchBar *)searchBar textDidChange:(NSString *)searchText
{
    [filteredListContent removeAllObjects];	// clear the filtered array first
    
    // search the table content for cell titles that match "searchText"
    // if found add to the mutable array and force the table to reload
    //
    NSString *cellTitle;
    for (cellTitle in listContent)
    {
        NSComparisonResult result = [cellTitle compare:searchText options:NSCaseInsensitiveSearch
                                                 range:NSMakeRange(0, [searchText length])];
        if (result == NSOrderedSame)
        {
            [filteredListContent addObject:cellTitle];
        }
    }
    
    [myTableView reloadData];
}

// called when cancel button pressed
- (void)searchBarCancelButtonClicked:(UISearchBar *)searchBar
{
    // if a valid search was entered but the user wanted to cancel, bring back the saved list content
    if (searchBar.text.length > 0)
    {
        [filteredListContent removeAllObjects];
        [filteredListContent addObjectsFromArray: savedContent];
    }
    
    [myTableView reloadData];
    
    [searchBar resignFirstResponder];
    searchBar.text = @"";
}

// called when Search (in our case "Done") button pressed
- (void)searchBarSearchButtonClicked:(UISearchBar *)searchBar
{
    [searchBar resignFirstResponder];
}


#pragma mark TextField delegate function
- (BOOL)textFieldShouldReturn:(UITextField *)theTextField {
    if ([theTextField isEqual:login]) {
        [mdp becomeFirstResponder];
    }
    
    if ([theTextField isEqual:mdp]) {
        [self login];
    }
    [theTextField resignFirstResponder];
    return YES;
}

@end

*/
