//
//  TRRankingTableViewCell.m
//  tuxracer
//
//  Created by emmanuel de Roux on 06/12/08.
//  Copyright 2008 école Centrale de Lyon. All rights reserved.
//

#import "TRRankingTableViewCell.h"
#import <QuartzCore/QuartzCore.h>


@implementation TRRankingTableViewCell
@synthesize rankingLabel, nameLabel, scoreLabel, timeLabel, herringLabel, scoreImg, herringImg, timeImg, addButton;

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier boldText:(BOOL)bold {
	if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier]) {
        
		// we need a view to place our labels on.
		UIView *myContentView = self.contentView;
        
		self.rankingLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor whiteColor] fontSize:18.0 bold:bold];
		self.rankingLabel.textAlignment = UITextAlignmentLeft; // default
		[myContentView addSubview:self.rankingLabel];
		[self.rankingLabel release];
        
        self.nameLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor lightGrayColor] fontSize:18.0 bold:bold];
		self.nameLabel.textAlignment = UITextAlignmentLeft; // default
		[myContentView addSubview:self.nameLabel];
		[self.nameLabel release];
        
        self.scoreLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor lightGrayColor] fontSize:11.0 bold:bold];
		self.scoreLabel.textAlignment = UITextAlignmentLeft; // default
		[myContentView addSubview:self.scoreLabel];
		[self.scoreLabel release];
        
        self.timeLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor lightGrayColor] fontSize:11.0 bold:bold];
		self.timeLabel.textAlignment = UITextAlignmentLeft; // default
		[myContentView addSubview:self.timeLabel];
		[self.timeLabel release];
        
        self.herringLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor lightGrayColor] fontSize:11.0 bold:bold];
		self.herringLabel.textAlignment = UITextAlignmentLeft; // default
		[myContentView addSubview:self.herringLabel];
		[self.herringLabel release];
        
        self.scoreImg = [self newImageWithName:@"calculatriceicon.png"];
		[myContentView addSubview:self.scoreImg];
		[self.scoreImg release];
        
        self.herringImg = [self newImageWithName:@"herringicon.png"];
		[myContentView addSubview:self.herringImg];
		[self.herringImg release];
        
        self.timeImg = [self newImageWithName:@"timeicon.png"];
		[myContentView addSubview:self.timeImg];
		[self.timeImg release];
        
        self.addButton = [UIButton buttonWithType:UIButtonTypeContactAdd];
        [myContentView addSubview:self.addButton];
        [self.addButton release];
        
        empty=NO;
        
	}
    
	return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    
	[super setSelected:selected animated:animated];
    
	// Configure the view for the selected state
}

-(void) boldFont:(BOOL)bold {
    UIFont* oldFont;
    
    oldFont = self.rankingLabel.font;
    self.rankingLabel.font = bold?[UIFont boldSystemFontOfSize:oldFont.pointSize]:[UIFont systemFontOfSize:oldFont.pointSize];
    
    oldFont = self.nameLabel.font;
    self.nameLabel.font = bold?[UIFont boldSystemFontOfSize:oldFont.pointSize]:[UIFont systemFontOfSize:oldFont.pointSize];
    
    oldFont = self.scoreLabel.font;
    self.scoreLabel.font = bold?[UIFont boldSystemFontOfSize:oldFont.pointSize]:[UIFont systemFontOfSize:oldFont.pointSize];
    
    oldFont = self.timeLabel.font;
    self.timeLabel.font = bold?[UIFont boldSystemFontOfSize:oldFont.pointSize]:[UIFont systemFontOfSize:oldFont.pointSize];
    
    oldFont = self.herringLabel.font;
    self.herringLabel.font = bold?[UIFont boldSystemFontOfSize:oldFont.pointSize]:[UIFont systemFontOfSize:oldFont.pointSize];
}

-(void)setData:(NSArray *)data {
    //cas normal
    if ([data count]==5) {
        self.rankingLabel.text = [data objectAtIndex:0];
        self.nameLabel.text = [NSString stringWithFormat:@"%@",[data objectAtIndex:1]];
        self.scoreLabel.text = [NSString stringWithFormat:@"%@",[data objectAtIndex:2]];
        self.timeLabel.text = [NSString stringWithFormat:@"%@",[data objectAtIndex:3]];
        self.herringLabel.text = [NSString stringWithFormat:@"%@",[data objectAtIndex:4]];
        
        //Au cas ou on réutilise une case qui a servi a etre celle d'une empty list friend, on reset tout
        empty=NO;
        self.accessoryType=UITableViewCellAccessoryNone;
        self.addButton.hidden=true;
        self.rankingLabel.hidden=false;
        self.scoreLabel.hidden=false;
        self.timeLabel.hidden=false;
        self.herringLabel.hidden=false;
        self.scoreImg.hidden=false;
        self.timeImg.hidden=false;        
        self.herringImg.hidden=false;
        self.nameLabel.textAlignment = UITextAlignmentLeft;
        [self layoutSubviews];
    }
    //cas empty friends list
    else if ([data count]==1) {
        empty=YES;
        
        //On indique que la liste est vide en l'écrivant dans la cell
        self.nameLabel.text = [NSString stringWithFormat:@"%@",[data objectAtIndex:0]];
        
        self.addButton.hidden=false;
        self.rankingLabel.hidden=true;
        self.scoreLabel.hidden=true;
        self.timeLabel.hidden=true;
        self.herringLabel.hidden=true;
        self.scoreImg.hidden=true;
        self.timeImg.hidden=true;        
        self.herringImg.hidden=true;
        
        // getting the cell size
        CGRect contentRect = self.contentView.bounds;
        
        // place the name label
		self.nameLabel.frame = CGRectMake(contentRect.origin.x+ 10, contentRect.origin.y+10, contentRect.size.width-20, contentRect.size.height-20);
        
        //center the text
        self.nameLabel.textAlignment = UITextAlignmentCenter;
        
        // place the add button
		self.addButton.frame = CGRectMake(contentRect.origin.x+ contentRect.size.width - 30, contentRect.origin.y+10,23, 23);
        
        [self setNeedsLayout];
    }
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
         
        // place the ranking label
		frame = CGRectMake(boundsX + 10, 4, 90, 20);
		self.rankingLabel.frame = frame;
        
        //dans le cas empty, ca a déja été défini
        if (!empty) {
            // place the name label
            frame = CGRectMake(boundsX + 100, 4, 195, 20);
            self.nameLabel.frame = frame;
        }
        
        // place the score image
		frame = CGRectMake(boundsX + 10, 27, 10, 13);
		self.scoreImg.frame = frame;
        
        // place the score label
		frame = CGRectMake(boundsX + 30, 30, 70, 10);
		self.scoreLabel.frame = frame;
        
        // place the time image
		frame = CGRectMake(boundsX + 100, 24, 20, 20);
		self.timeImg.frame = frame;
        
        // place the time label
		frame = CGRectMake(boundsX + 130, 30, 70, 10);
		self.timeLabel.frame = frame;
        
        // place the herring image
		frame = CGRectMake(boundsX + 190, 24, 20, 20);
		self.herringImg.frame = frame;
        
        // place the herring label
		frame = CGRectMake(boundsX + 220, 30, 70, 10);
		self.herringLabel.frame = frame;
	}
}

- (UIImageView *)newImageWithName:(NSString *)name
{
    UIImage* image = [UIImage imageNamed:name];
   	UIImageView *imageView = [[UIImageView alloc] initWithImage:image];
    return imageView;
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

-(void)selectedAction {
    if (empty) {
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Empty friends list !" message:@"Do you want to add friends to your friends list ?" delegate:self cancelButtonTitle:@"No" otherButtonTitles:@"Yes",nil];
        [alert show];
    }
}

#pragma mark alertView delegate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
    //case Yes
    if (buttonIndex==1) {
        [[scoresController sharedScoresController] displayFriendsManagerFromRankingsView:nil];
    }
}

- (void)dealloc {
	// make sure you free the memory
    [timeLabel dealloc]; 
    [herringLabel dealloc]; 
    [scoreImg dealloc]; 
    [herringImg dealloc]; 
    [timeImg dealloc]; 
    [addButton dealloc];
    [rankingLabel dealloc];
	[nameLabel dealloc];
	[scoreLabel dealloc];
	[super dealloc];
}

@end