//
//  DetailViewController.h
//  ios
//
//  Created by 周博 on 16-3-20.
//  Copyright (c) 2016年 trix. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface DetailViewController : UIViewController

@property (strong, nonatomic) id detailItem;
@property (weak, nonatomic) IBOutlet UILabel *detailDescriptionLabel;

@end

