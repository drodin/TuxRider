//
//  TRSlopeInfoCell.m
//  tuxracer
//
//  Created by emmanuel de Roux on 08/12/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import "TRSlopeInfoCell.h"

@implementation TRSlopeInfoCell

// we need to synthesize the labels
@synthesize titleLabel, friendsLabel, countryLabel, worldLabel, friendsImg, countryImg, worldImg;

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier {
	if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier]) {
        
		// we need a view to place our labels on.
		UIView *myContentView = self.contentView;
        
		self.titleLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor whiteColor] fontSize:14.0 bold:YES];
		self.titleLabel.textAlignment = UITextAlignmentLeft; // default
		[myContentView addSubview:self.titleLabel];
		[self.titleLabel release];
        
        self.friendsLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor lightGrayColor] fontSize:10.0 bold:YES];
		self.friendsLabel.textAlignment = UITextAlignmentLeft; // default
		[myContentView addSubview:self.friendsLabel];
		[self.friendsLabel release];
        
        self.countryLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor lightGrayColor] fontSize:10.0 bold:YES];
		self.countryLabel.textAlignment = UITextAlignmentLeft; // default
		[myContentView addSubview:self.countryLabel];
		[self.countryLabel release];
        
        self.worldLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor lightGrayColor] fontSize:10.0 bold:YES];
		self.worldLabel.textAlignment = UITextAlignmentLeft; // default
		[myContentView addSubview:self.worldLabel];
		[self.worldLabel release];
        
        self.friendsImg = [self newImageWithName:@"friends.png"];
		[myContentView addSubview:self.friendsImg];
		[self.friendsImg release];
        
        self.countryImg = [self newImageWithName:@"flag.png"];
		[myContentView addSubview:self.countryImg];
		[self.countryImg release];
        
        self.worldImg = [self newImageWithName:@"worldicon.png"];
		[myContentView addSubview:self.worldImg];
		[self.worldImg release];
	}
    
	return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    
	[super setSelected:selected animated:animated];
    
	// Configure the view for the selected state
}

-(void)setData:(NSArray *)data {
	self.titleLabel.text = [data objectAtIndex:0];
	self.friendsLabel.text = [NSString stringWithFormat:@"%@",[data objectAtIndex:1]];
    self.countryLabel.text = [NSString stringWithFormat:@"%@",[data objectAtIndex:2]];
    self.worldLabel.text = [NSString stringWithFormat:@"%@",[data objectAtIndex:3]];
}


- (void)layoutSubviews {
    
    [super layoutSubviews];
    
	// getting the cell size
    CGRect contentRect = self.contentView.bounds;
    
	// In this example we will never be editing, but this illustrates the appropriate pattern
    if (!self.editing) {
        
		// get the X pixel spot
        CGFloat boundsX = contentRect.origin.x;
		CGRect frame;
        
        /*
		 Place the title label.
		 place the label whatever the current X is plus 10 pixels from the left
		 place the label 4 pixels from the top
		 make the label 200 pixels wide
		 make the label 20 pixels high
         */
		frame = CGRectMake(boundsX + 10, 4, 270, 20);
		self.titleLabel.frame = frame;
        
		// place the ranking friends image
		frame = CGRectMake(boundsX + 10, 24, 14, 14);
		self.friendsImg.frame = frame;
        
        // place the ranking friends label
		frame = CGRectMake(boundsX + 30, 24, 50, 14);
		self.friendsLabel.frame = frame;
        
        // place the ranking country image
		frame = CGRectMake(boundsX + 80, 24, 14, 14);
		self.countryImg.frame = frame;
        
        // place the ranking country label
		frame = CGRectMake(boundsX + 100, 24, 50, 14);
		self.countryLabel.frame = frame;
        
        // place the ranking world image
		frame = CGRectMake(boundsX + 150, 24, 14, 14);
		self.worldImg.frame = frame;
        
        // place the ranking world label
		frame = CGRectMake(boundsX + 170, 24, 50, 14);
		self.worldLabel.frame = frame;
	}
}

- (UILabel *)newLabelWithPrimaryColor:(UIColor *)primaryColor selectedColor:(UIColor *)selectedColor fontSize:(CGFloat)fontSize bold:(BOOL)bold
{
    UIFont *font;
    if (bold) {
        font = [UIFont boldSystemFontOfSize:fontSize];
    } else {
        font = [UIFont systemFontOfSize:fontSize];
    }
    
	UILabel *newLabel = [[UILabel alloc] initWithFrame:CGRectZero];
	newLabel.backgroundColor = [UIColor whiteColor];
	newLabel.opaque = YES;
	newLabel.textColor = primaryColor;
	newLabel.highlightedTextColor = selectedColor;
	newLabel.font = font;
    
	return newLabel;
}

- (UIImageView *)newImageWithName:(NSString *)name
{
    UIImage* image = [UIImage imageNamed:name];
   	UIImageView *imageView = [[UIImageView alloc] initWithImage:image];
    return imageView;
}

- (void)dealloc {
	// make sure you free the memory
    [titleLabel dealloc];
	[friendsImg dealloc];
	[countryImg dealloc];
	[worldImg dealloc];
	[friendsLabel dealloc];
	[countryLabel dealloc];
	[worldLabel dealloc];
	[super dealloc];
}

@end