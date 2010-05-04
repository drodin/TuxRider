//
//  TRRankingTableViewCell.h
//  tuxracer
//
//  Created by emmanuel de Roux on 06/12/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "TransitionView.h"
#import "scoresController.h"

@interface TRRankingTableViewCell : UITableViewCell {
    // adding the labels we want to show in the cell
	UILabel *rankingLabel;
	UILabel *nameLabel;
    UILabel *scoreLabel;
    UILabel *timeLabel;
    UILabel *herringLabel;
    UIButton *addButton;
    UIImageView *scoreImg;
    UIImageView *herringImg;
    UIImageView *timeImg;
    IBOutlet UIView* rankingTableView;
    IBOutlet UIView* manageFriendsView;
    IBOutlet TransitionView* transitionWindow;
    BOOL empty;
}

//init
- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier boldText:(BOOL)bold;
-(void) boldFont:(BOOL)bold;

// gets the data from another class
-(void)setData:(NSArray *)data;

//Action when a cell is selected/ useful only in the case of empty friends list
-(void)selectedAction;

// internal function to ease setting up label text and image
-(UILabel *)newLabelWithPrimaryColor:(UIColor *)primaryColor selectedColor:(UIColor *)selectedColor fontSize:(CGFloat)fontSize bold:(BOOL)bold;
- (UIImageView *)newImageWithName:(NSString *)name;

// you should know what this is for by know
@property (nonatomic, retain) UILabel *rankingLabel;
@property (nonatomic, retain) UILabel *nameLabel;
@property (nonatomic, retain) UILabel *scoreLabel;
@property (nonatomic, retain) UILabel *timeLabel;
@property (nonatomic, retain) UILabel *herringLabel;
@property (nonatomic, retain) UIButton *addButton;
@property (nonatomic, retain) UIImageView *scoreImg;
@property (nonatomic, retain) UIImageView *herringImg;
@property (nonatomic, retain) UIImageView *timeImg;
@end
