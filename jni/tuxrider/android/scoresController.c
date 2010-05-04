//
//  scoresController.m
//  tuxracer
//
//  Created by emmanuel de Roux on 01/12/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import "sharedGeneralFunctions.h"

//static scoresController* sharedScoresController=nil;

//NSString * tuxRiderRootServer = TUXRIDER_ROOT_SERVER;

void saveScoreOnlineAfterRace(char* raceName,int score,int herring,int minutes,int seconds,int hundredths){
    /*
    char buff[10];
    sprintf( buff, "%02d:%02d:%02d", minutes, seconds, hundredths );
    [sharedScoresController saveScoreOnlineAfterRace:score onPiste:[NSString stringWithCString:raceName] herring:herring time:buff];
    */
}

void displayRankingsAfterRace(char* raceName,int score,int herring,int minutes,int seconds,int hundredths){
    /*
    char buff[10];
    sprintf( buff, "%02d:%02d:%02d", minutes, seconds, hundredths );
    [sharedScoresController displayRankingsAfterRace:score onPiste:[NSString stringWithCString:raceName] herring:herring time:buff];
    */
}

void displaySlopes()
{
    //[sharedScoresController displaySlopes:nil];
}

void dirtyScores ()
{
    //[sharedScoresController dirtyScores];
}

/*
@implementation scoresController
@synthesize _currentSlope;

- (id) init
{
    self = [super init];
    if (self != nil) {
        sharedScoresController=self;
    }
    return self;
}

- (void) dealloc
{
    [_currentRankings release];
    [_listOfCourses release];
    [super dealloc];
}

+ (id) sharedScoresController {
    return sharedScoresController;
}

#pragma mark Saving scores functions
//called by a C func
- (void) saveScoreOnlineAfterRace:(int)score onPiste:(NSString*)piste herring:(int)herring time:(char*)time{
    //On enregistre le score en ligne que si l'utilisateur l'a choisi dans les prefs
    NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
    if([prefs boolForKey:@"saveScoresAfterRace"]) {
        ConnectionController* conn = [[[ConnectionController alloc] init] autorelease];
        NSMutableString* queryString = [NSMutableString stringWithFormat:@"login=%@&mdp=%@&piste=%@&score=%d&herring=%d&time=%s",[prefs valueForKey:@"username"],[prefs valueForKey:@"password"],piste,score,herring,time];
        //Si le joueur ajouté des amis
        if (![[[prefs objectForKey:@"friendsList"] objectAtIndex:0] isEqualToString:@"*empty*"]){
            int i=0;
            for (i=0; i<[[prefs objectForKey:@"friendsList"] count]; i++) {
                [queryString appendFormat:@"&friends[%d]=%@",i,[[prefs objectForKey:@"friendsList"] objectAtIndex:i]];
            }
        }
        [conn postRequest:queryString atURL:[tuxRiderRootServer stringByAppendingString:@"saveScore.php"] withWaitMessage:(NSString*)@"Saving score online..." sendResponseTo:self withMethod:@selector(treatSaveScoreAfterRaceResult:)];
    
        //Set this to true so the user car go back to race select screen by touching the screen whatever happens (cancel, error, all good, etc...)
        g_game.rankings_displayed=true;
    }
    else {
        [self displayRankingsAfterRace:score onPiste:piste herring:herring time:time];
    }
}

//calls a C func
- (void) syncScores {
    NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
    ConnectionController* conn = [[[ConnectionController alloc] init] autorelease];
    NSString* queryString = [NSString stringWithFormat:@"login=%@&mdp=%@&%s",[prefs valueForKey:@"username"],[prefs valueForKey:@"password"],editSynchronizeScoresRequest()];
    [conn postRequest:queryString atURL:[tuxRiderRootServer stringByAppendingString:@"synchronize.php"] withWaitMessage:(NSString*)@"Saving unsaved scores online..." sendResponseTo:self withMethod:@selector(treatSyncScoresResult:)];
}

- (void) dirtyScores {
    NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
    [prefs setInteger:([prefs integerForKey:@"needsSync"]+1) forKey:@"needsSync"];
}

#pragma mark display functions

- (IBAction) displaySlopes:(id)sender {
    if([transitionView isTransitioning]) {
		// Don't interrupt an ongoing transition
		return;
	}
	// If view 1 is already a subview of the transition view replace it with view 2, and vice-versa.
	if([glView superview]) {
		//Affiche les slopes
		[glView stopAnimation];
		[transitionView replaceSubview:glView withSubview:slopesView transition:kCATransitionPush direction:kCATransitionFromBottom duration:0.75];
        [self refreshSlopes:TRUE];
	} else {
		//Affiche le jeu
        [glView startAnimation];
		[transitionView replaceSubview:slopesView withSubview:glView transition:kCATransitionPush direction:kCATransitionFromTop duration:0.75];
    }
    
}

- (IBAction) displayRankings:(id)sender {
    if([transitionView isTransitioning]) {
		// Don't interrupt an ongoing transition
		return;
	}
	// If view 1 is already a subview of the transition view replace it with view 2, and vice-versa.
	if([slopesView superview]) {
		//Affiche les rankings
        [self refreshRankings];
		[transitionView replaceSubview:slopesView withSubview:rankingsView transition:kCATransitionPush direction:kCATransitionFromRight duration:0.75];
	} else {
		//Affiche les slopes
		[transitionView replaceSubview:rankingsView withSubview:slopesView transition:kCATransitionPush direction:kCATransitionFromLeft duration:0.75];
    }
}

- (IBAction) displayFriendsManagerFromRankingsView:(id)sender {
    if([transitionView isTransitioning]) {
		// Don't interrupt an ongoing transition
		return;
	}
	// If view 1 is already a subview of the transition view replace it with view 2, and vice-versa.
	if([rankingsView superview]) {
		[transitionView replaceSubview:rankingsView withSubview:friendsManagerView transition:kCATransitionFade direction:kCATransitionFromRight duration:0.25];
    }
}
//called by a C func

- (void) displayRankingsAfterRace:(int)score onPiste:(NSString*)piste herring:(int)herring time:(char*)time{
    //On enregistre le score en ligne que si l'utilisateur l'a choisi dans les prefs
    NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
    if([prefs boolForKey:@"displayRankingsAfterRace"]) {
        ConnectionController* conn = [[[ConnectionController alloc] init] autorelease];
        NSMutableString* queryString = [NSMutableString stringWithFormat:@"login=%@&mdp=%@&piste=%@&score=%d&herring=%d&time=%s",[prefs valueForKey:@"username"],[prefs valueForKey:@"password"],piste,score,herring,time];
        //Si le joueur ajouté des amis
        if (![[[prefs objectForKey:@"friendsList"] objectAtIndex:0] isEqualToString:@"*empty*"]){
            int i=0;
            for (i=0; i<[[prefs objectForKey:@"friendsList"] count]; i++) {
                [queryString appendFormat:@"&friends[%d]=%@",i,[[prefs objectForKey:@"friendsList"] objectAtIndex:i]];
            }
        }
        [conn postRequest:queryString atURL:[tuxRiderRootServer stringByAppendingString:@"displayRankingsAfterRace.php"] withWaitMessage:(NSString*)@"Getting World rankings for the score you just did..." sendResponseTo:self withMethod:@selector(treatDisplayRankingsAfterRaceResult:)];
        
        //Set this to true so the user car go back to race select screen by touching the screen whatever happens (cancel, error, all good, etc...)
        g_game.rankings_displayed=true;
    }
}

#pragma mark traitement des reponses HTTP

-(void) treatError:(NSString*)erreur{
    int err = [erreur intValue];
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Error !" message:@"" delegate:nil cancelButtonTitle:@"Ok" otherButtonTitles:nil];
    switch (err) {
        case LOGIN_ERROR:
            [alert setMessage:@"Wrong login or password. Go to settings pannel to settle this problem."];
            [alert show];
            break;
        case SERVER_ERROR:
            [alert setMessage:@"Internal server error! Please try again Later."];
            [alert show];
            break;
        case CONNECTION_ERROR:
            [alert setMessage:@"Check your network connection and try again."];
            [alert show];
            break;
        case SCORE_SAVED:
            //Set this to true so the rankings will be displayed
            g_game.needs_save_or_display_rankings=true;
            //function implemented in game_over.c
            displaySavedAndRankings("Congratulations !", [(NSString*)[_currentRankings objectAtIndex:0] UTF8String], [(NSString*)[_currentRankings objectAtIndex:1] UTF8String], [(NSString*)[_currentRankings objectAtIndex:2] UTF8String]);
            [prefs setInteger:([prefs integerForKey:@"needsSync"]-1) forKey:@"needsSync"];
            break;
        case RANKINGS_AFTER_RACE_OBTAINED:
            //Set this to true so the rankings will be displayed
            g_game.needs_save_or_display_rankings=true;
            //function implemented in game_over.c
            displaySavedAndRankings("World rankings", [(NSString*)[_currentRankings objectAtIndex:0] UTF8String], [(NSString*)[_currentRankings objectAtIndex:1] UTF8String], [(NSString*)[_currentRankings objectAtIndex:2] UTF8String]);
            break;
        case NEEDS_NEW_VERSION:
            [alert setTitle:@"Score not saved !"];
            [alert setMessage:@"For security reasons, you need to update Tux Rider World Challenge to save scores online. Go to the App Store to do the update."];
            [alert show];
            break;
        case BETTER_SCORE_EXISTS:
            [alert setTitle:@"Score not saved !"];
            [alert setMessage:@"A better score already exists for this login !"];
            [alert show];
            break;
        case NO_SCORES_SAVED_YET:
            [alert setTitle:@"No rankings available !"];
            [alert setMessage:@"You don't have any scores saved online for the moment !"];
            [alert show];
            break;
        case NOTHING_UPDATED:
            [alert setTitle:@"No need to update !"];
            [alert setMessage:@"Scores online were already up-to-date."];
            [prefs setInteger:0 forKey:@"needsSync"];
            [self refreshSlopes:YES];
            [alert show];
            break;
        case RANKINGS_OK:
            //Do nothing
            break;
        default:
            [alert setMessage:@"Unknown error!"];
            [alert show];
            break;
    }
}

- (void) treatData:(NSString*) data {
    //[[[allCountries componentsSeparatedByString:@"|||"] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)] retain];
    //Sur chaque ligne, une série de data séparés par le symbole  |||
    //chaaque ligne est materialisee par \r\n
    //les lignes sont séparées en paquets, chacuns séparés par un \r\t\n\r\t\n
    //paquet n°1 : sur chaque ligne, nomdelapiste|||friendsRanking|||countryRanking|||worldRanking
    //paquer n°2 : une ligne, Status de la requete
    //Mais en cas d'erreur de conection, il n'y a qu'une seule ligne qui contient le numéro de l'erreur
    
    NSArray* datas = [data componentsSeparatedByString:@"\r\t\n\r\t\n"];
    //On traite l'éventuelle erreur
    if ([datas count] == 1) [self treatError:[datas objectAtIndex:0]];
    else {
        [self treatError:[datas objectAtIndex:1]];
        if ([[datas objectAtIndex:1] intValue] == RANKINGS_OK) {
            //on recupere la liste des pistes
            _listOfCourses = [[[datas objectAtIndex:0] componentsSeparatedByString:@"\r\n"] retain];
            
            //save cache
            NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
            [prefs setObject:data forKey:@"rankingCache"];
            
            //reload data
            [slopesTableView reloadData];
        }
    }
}



- (void) treatSaveScoreAfterRaceResult:(NSString*)result {
    NSArray* datas = [result componentsSeparatedByString:@"\r\n"];
    if ([datas count]==2){
        NSString* erreur = [datas objectAtIndex:1];
        _currentRankings = [[[datas objectAtIndex:0] componentsSeparatedByString:@"|||"] retain];
        [self treatError:erreur];
    } else [self treatError:[NSString stringWithFormat:@"%d",SERVER_ERROR ]];
}

- (void) treatDisplayRankingsAfterRaceResult:(NSString*)result {
    NSArray* datas = [result componentsSeparatedByString:@"\r\n"];
    if ([datas count]==2){
        NSString* erreur = [datas objectAtIndex:1];
        _currentRankings = [[[datas objectAtIndex:0] componentsSeparatedByString:@"|||"] retain];
        [self treatError:erreur];
    } else [self treatError:[NSString stringWithFormat:@"%d",SERVER_ERROR ]];
    //Set this to true so the user car go back to race select screen by touching the screen
    g_game.rankings_displayed=true;
}

- (void) treatSyncScoresResult:(NSString*)result {
    printf("%s\n",[result UTF8String]);
    if ([result intValue]==SCORE_SAVED) {
        NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
        [prefs setInteger:0 forKey:@"needsSync"];
        [self refreshSlopes:TRUE];
    } else [self treatData:result];
}

#pragma mark refreshing functions

- (void) refreshSlopes:(BOOL)syncIfNeeded {
    
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    
    //Si un cache existe, on l'utilise
    if (![[prefs objectForKey:@"rankingCache"] isEqualToString:@""]) {
        [(scoresController*)[slopesTableView delegate] treatData:[prefs objectForKey:@"rankingCache"]];
    }
    
    //Si certains scores n'ont pas été sauvegardés en ligne, on averti l'utilisateur et on lui propose d'abord de le faire
    if ([prefs boolForKey:@"needsSync"]>0 && syncIfNeeded) {
        UIActionSheet* alert = [[UIActionSheet alloc] initWithTitle:@"Unsaved scores detected. Do you want to save them online now ?" delegate:self cancelButtonTitle:@"No" destructiveButtonTitle:@"Yes" otherButtonTitles:nil];
        [alert setActionSheetStyle:UIActionSheetStyleBlackOpaque];
        [alert showInView:transitionView];
    }
    else {
        ConnectionController* conn = [[[ConnectionController alloc] init] autorelease];
        NSMutableString* queryString = [NSMutableString stringWithFormat:@"login=%@&mdp=%@",[prefs objectForKey:@"username"],[prefs objectForKey:@"password"]];
        //Si le joueur ajouté des amis
        if (![[[prefs objectForKey:@"friendsList"] objectAtIndex:0] isEqualToString:@"*empty*"]){
            int i=0;
            for (i=0; i<[[prefs objectForKey:@"friendsList"] count]; i++) {
                [queryString appendFormat:@"&friends[%d]=%@",i,[[prefs objectForKey:@"friendsList"] objectAtIndex:i]];
            }
        }
        [conn postRequest:queryString atURL:[tuxRiderRootServer stringByAppendingString:@"displaySlopes.php"] withWaitMessage:(NSString*)@"Refreshing rankings..." sendResponseTo:self withMethod:@selector(treatData:)];
    }
}

- (void) refreshRankings {
    ConnectionController* conn = [[[ConnectionController alloc] init] autorelease];
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    NSMutableString* queryString = [NSMutableString stringWithFormat:@"login=%@&mdp=%@&piste=%@",[prefs objectForKey:@"username"],[prefs objectForKey:@"password"],_currentSlope];
    
    //Si le joueur ajouté des amis
    if (![[[prefs objectForKey:@"friendsList"] objectAtIndex:0] isEqualToString:@"*empty*"]){
        int i=0;
        for (i=0; i<[[prefs objectForKey:@"friendsList"] count]; i++) {
            [queryString appendFormat:@"&friends[%d]=%@",i,[[prefs objectForKey:@"friendsList"] objectAtIndex:i]];
        }
    }
    [conn postRequest:queryString atURL:[tuxRiderRootServer stringByAppendingString:@"displayRanking.php"] withWaitMessage:(NSString*)@"Refreshing ranking infos..." sendResponseTo:[rankingsTableView delegate] withMethod:@selector(treatData:)];
}

#pragma mark actionSheet delegate
//this is only for the alert view concerning syncronizing
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
    switch (buttonIndex) {
            //Yes
        case 0:
            [self syncScores];
            break;
            //No
        case 1:
            [self refreshSlopes:NO];
            break;
        default:
            break;
    }
}

#pragma mark tableView delegate for slopesView

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    _currentSlope=[[(TRSlopeInfoCell*)[tableView cellForRowAtIndexPath:indexPath] titleLabel] text];
    [(TRRankingDelegate*)[rankingsTableView delegate] navTitle:_currentSlope];
    //On charge le cache si il existe
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    if ([prefs objectForKey:_currentSlope]!=nil) {
        [(TRRankingDelegate*)[rankingsTableView delegate] treatData:[prefs objectForKey:_currentSlope]];
    }
    [rankingsTableView reloadData];
    [self displayRankings:nil];
    [[tableView cellForRowAtIndexPath:indexPath] setSelected:NO];
    [prefs synchronize];
}

#pragma mark UITableViewDataSource for slopesView
//Pour la première table view, celle qui affiche toutes les pistes
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [_listOfCourses count];
}

- (UITableViewCellAccessoryType)tableView:(UITableView *)tableView accessoryTypeForRowWithIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellAccessoryDetailDisclosureButton;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    TRSlopeInfoCell *cell = (TRSlopeInfoCell*)[tableView dequeueReusableCellWithIdentifier:@"cellID"];
    if (cell == nil)
    {
        cell = [[[TRSlopeInfoCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"cellID"] autorelease];
        cell.selectionStyle = UITableViewCellSelectionStyleBlue;
    }
    
    NSArray* data = [[_listOfCourses objectAtIndex:indexPath.row] componentsSeparatedByString:@"|||"];
    [cell setData:data];
    [cell setSelected:NO];
    
    return cell;
}
#pragma mark UITableViewDataSource for rankingsView defined in TRRankingDelegate

@end

*/
