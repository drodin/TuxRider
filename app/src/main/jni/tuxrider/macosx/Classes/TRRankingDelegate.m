//
//  TRRankingDelegate.m
//  tuxracer
//
//  Created by emmanuel de Roux on 06/12/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import "TRRankingDelegate.h"
#import "TRRankingTableViewCell.h"


@implementation TRRankingDelegate

- (id) init
{
    self = [super init];
    if (self != nil) {
        _worldRanking = [[NSMutableArray alloc] init];
        _countryRanking = [[NSMutableArray alloc] init];
        _worldRankingInfos = [[NSMutableArray alloc] init];
        _countryRankingInfos = [[NSMutableArray alloc] init];
        _friendsRanking = [[NSMutableArray alloc] init];
        _friendsRankingInfos = [[NSMutableArray alloc] init];
        _playerCountry = @"Country";
    }
    return self;
}

- (void) dealloc
{
    [_worldRanking release];
    [_countryRanking release];
    [_friendsRanking release];
    [_worldRankingInfos release];
    [_countryRankingInfos release];
    [_friendsRankingInfos release];
    [_playerCountry release];
    [super dealloc];
}

#pragma mark tableView delegate Functions
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 3; //Friends, Country, World
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    switch (section) {
        case 0:
            return [_friendsRanking count];
            break;
        case 1:
            return [_countryRanking count];
            break;
        case 2:
            return [_worldRanking count];
            break;
        default:
            break;
    }
    return 0;
}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section {
    switch (section) {
        case 0:
            return friendsSectionHeader;
            break;
        case 1:
            [countryName setText:[_playerCountry stringByAppendingString:@" ranking"]];
            return countrySectionHeader;
            break;
        case 2:
            return worldSectionHeader;
            break;
        default:
            break;
    }
    return NULL;
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section {
    return 35.0;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    switch (section) {
        case 0:
            return @"Friends ranking";
            break;
        case 1:
            return [_playerCountry stringByAppendingString:@" ranking"];
            break;
        case 2:
            return @"World ranking";
            break;
        default:
            break;
    }
    return NULL;
}

- (UITableViewCellAccessoryType)tableView:(UITableView *)tableView accessoryTypeForRowWithIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellSeparatorStyleNone;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    TRRankingTableViewCell* cell = (TRRankingTableViewCell*)[tableView cellForRowAtIndexPath:indexPath];
    [cell selectedAction];
}

- (TRRankingTableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSArray* arrayToUse;
    NSArray* infosArrayToUse;
    
    switch (indexPath.section) {
        case 0:
            arrayToUse = _friendsRanking;
            infosArrayToUse = _friendsRankingInfos;
            break;
        case 1:
            arrayToUse = _countryRanking;
            infosArrayToUse = _countryRankingInfos;
            break;
        case 2:
            arrayToUse = _worldRanking;
            infosArrayToUse = _worldRankingInfos;
            break;
        default:
            break;
    }

    TRRankingTableViewCell *cell = (TRRankingTableViewCell*)[tableView dequeueReusableCellWithIdentifier:@"cellID"];
    if (cell == nil)
    {
        cell = [[[TRRankingTableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"cellID" boldText:[(NSNumber*)[infosArrayToUse objectAtIndex:indexPath.row] boolValue]] autorelease];
        cell.selectionStyle = UITableViewCellSelectionStyleNone;
    } else {
       [cell boldFont:[(NSNumber*)[infosArrayToUse objectAtIndex:indexPath.row] boolValue]];
    }
    NSArray* data = [[arrayToUse objectAtIndex:indexPath.row] componentsSeparatedByString:@"|||"] ;
    [cell setData:data];
    [cell setSelected:NO];
    
    return cell;
}

#pragma mark Ranking Delegate Functions

- (void)navTitle:(NSString*)title {
    [nav setTitle:title];
}

- (void) resetData {
    [_worldRanking removeAllObjects];
    [_countryRanking removeAllObjects];
    [_friendsRanking removeAllObjects];
    [_worldRankingInfos removeAllObjects];
    [_countryRankingInfos removeAllObjects];
    [_friendsRankingInfos removeAllObjects];
}

- (void) treatData:(NSString*) data {
    [self resetData];
    
    //Sur chaque ligne, une série de data séparés par le symbole  |||
    //chaaque ligne est materialisee par \r\n
    //Les lignes sont regroupees par paquets, separes par des \r\t\n\r\t\n
    //paquet n°1 : une seule ligne : le pays du joueur
    //paquet n°2 : plusieurs lignes : les classement mondial
    //paquet n°3 : plusieurs lignes : les classement National
    //paquet n°4 : plusieurs lignes : les classement National
    //paquet n°5 : une seule ligne : le resultCode

    TRDebugLog("%s\n",[data UTF8String]);

    NSArray* datas = [data componentsSeparatedByString:@"\r\t\n\r\t\n"];
    //On traite l'éventuelle erreur
    if ([datas count]==1) [SC treatError:data];
    else if ([datas count]>1 && [datas count]!=5) [SC treatError:[NSString stringWithFormat:@"%d",SERVER_ERROR]];
    else {
        [SC treatError:[datas objectAtIndex:4]];
        if ([[datas objectAtIndex:4] intValue] == RANKINGS_OK) {
            //On recupère le pays du joueur
            _playerCountry = [[datas objectAtIndex:0] retain];
            
            //On recupere le login du joueur
            NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
            NSString* login = [prefs objectForKey:@"username"];
            
            //on recupere les lignes de classement mondial
            //les lignes sont de la forme : classement|||login|||score|||herring|||time
            NSArray* world = [[datas objectAtIndex:1] componentsSeparatedByString:@"\r\n"];
            NSArray* rank;
            NSString* rankString;
            BOOL bold;
            
            int i=0;
            for (i=0;i<[world count];i++) {
                rank = [[world objectAtIndex:i] componentsSeparatedByString:@"|||"];
                rankString = [NSString stringWithFormat:@"%@|||%@|||%@ pts|||%@|||%@",[rank objectAtIndex:0],[rank objectAtIndex:1],[rank objectAtIndex:2],[rank objectAtIndex:4],[rank objectAtIndex:3]];
                [_worldRanking addObject:rankString];
                if ([login isEqualToString:[rank objectAtIndex:1]]) {
                    bold = YES;
                } else {
                    bold = NO;
                }
                [_worldRankingInfos addObject:[NSNumber numberWithBool:bold]];
            }
            
            //on recupere les lignes de classement National
            NSArray* country = [[datas objectAtIndex:2] componentsSeparatedByString:@"\r\n"];
            for (i=0;i<[country count];i++) {
                rank = [[country objectAtIndex:i] componentsSeparatedByString:@"|||"];
                rankString = [NSString stringWithFormat:@"%@|||%@|||%@ pts|||%@|||%@",[rank objectAtIndex:0],[rank objectAtIndex:1],[rank objectAtIndex:2],[rank objectAtIndex:4],[rank objectAtIndex:3]];
                [_countryRanking addObject:rankString];
                if ([login isEqualToString:[rank objectAtIndex:1]]) {
                    bold = YES;
                } else {
                    bold = NO;
                }
                [_countryRankingInfos addObject:[NSNumber numberWithBool:bold]];
            }
            
            //on recupere les lignes de classement des amis
            NSArray* friends = [[datas objectAtIndex:3] componentsSeparatedByString:@"\r\n"];
            for (i=0;i<[friends count];i++) {
                rank = [[friends objectAtIndex:i] componentsSeparatedByString:@"|||"];
                NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
                if ([[[prefs objectForKey:@"friendsList"] objectAtIndex:0] isEqualToString:@"*empty*"])
                {
                    rankString = @"Empty friends List.";
                }
                else
                {
                    rankString = [NSString stringWithFormat:@"%@|||%@|||%@ pts|||%@|||%@",[rank objectAtIndex:0],[rank objectAtIndex:1],[rank objectAtIndex:2],[rank objectAtIndex:4],[rank objectAtIndex:3]];
                }
                [_friendsRanking addObject:rankString];
                if ([login isEqualToString:[rank objectAtIndex:1]]) {
                    TRDebugLog("%s = %s\n",[login UTF8String], [[rank objectAtIndex:1] UTF8String]);
                    bold = YES;
                } else {
                    bold = NO;
                }
                [_friendsRankingInfos addObject:[NSNumber numberWithBool:bold]];
            }
            
            //save cache
            [prefs setObject:data forKey:[SC _currentSlope]];
            
            [rankingTableView reloadData];
        }
    }
}

@end
