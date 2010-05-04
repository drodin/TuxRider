//
//  TRSlopeInfoCell.h
//  tuxracer
//
//  Created by emmanuel de Roux on 08/12/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface TRSlopeInfoCell : UITableViewCell {
	// adding the labels we want to show in the cell
	UILabel *titleLabel;
	UILabel *friendsLabel;
    UILabel *countryLabel;
    UILabel *worldLabel;
    UIImageView *friendsImg;
    UIImageView *countryImg;
    UIImageView *worldImg;
}


// gets the data from another class
-(void)setData:(NSArray *)data;

// internal function to ease setting up label text and image
-(UILabel *)newLabelWithPrimaryColor:(UIColor *)primaryColor selectedColor:(UIColor *)selectedColor fontSize:(CGFloat)fontSize bold:(BOOL)bold;
- (UIImageView *)newImageWithName:(NSString *)name;

// you should know what this is for by know
@property (nonatomic, retain) UILabel *titleLabel;
@property (nonatomic, retain) UILabel *friendsLabel;
@property (nonatomic, retain) UILabel *countryLabel;
@property (nonatomic, retain) UILabel *worldLabel;
@property (nonatomic, retain) UIImageView *friendsImg;
@property (nonatomic, retain) UIImageView *countryImg;
@property (nonatomic, retain) UIImageView *worldImg;

@end
