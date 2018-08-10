//
//  TRRankingDelegate.h
//  tuxracer
//
//  Created by emmanuel de Roux on 06/12/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "scoresController.h"
#import "myHTTPErrors.h"

@interface TRRankingDelegate : NSObject {
    IBOutlet scoresController* SC;
    IBOutlet UITableView* rankingTableView;
    IBOutlet UILabel* countryName;
    IBOutlet UINavigationItem* nav;
    
    IBOutlet UIView* friendsSectionHeader;
    IBOutlet UIView* countrySectionHeader;
    IBOutlet UIView* worldSectionHeader;
    
    NSMutableArray* _worldRanking;
    NSMutableArray* _countryRanking;
    NSMutableArray* _worldRankingInfos;
    NSMutableArray* _countryRankingInfos;
    NSMutableArray* _friendsRanking;
    NSMutableArray* _friendsRankingInfos;
    NSString* _playerCountry;
}

- (void) treatData:(NSString*) data;
- (void) resetData;
- (void) navTitle:(NSString*)title;
@end
