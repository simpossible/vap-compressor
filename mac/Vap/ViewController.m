//
//  ViewController.m
//  Vap
//
//  Created by 梁金锋 on 2024/4/18.
//

#import "ViewController.h"
#import <Foundation/Foundation.h>

@interface ViewController()


@end


@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    NSButton *button = [[NSButton alloc] initWithFrame:(NSMakeRect(0, 0, 100, 100))];
    [self.view addSubview:button];
    button.layer.backgroundColor = [NSColor redColor].CGColor;
    // Do any additional setup after loading the view.
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
