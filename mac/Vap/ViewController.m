//
//  ViewController.m
//  Vap
//
//  Created by 梁金锋 on 2024/4/18.
//

#import "ViewController.h"
#import <Foundation/Foundation.h>
#import "ContainerView.h"
@import WebKit;



@interface ViewController()<NSDraggingDestination>

@property (nonatomic, strong) WKWebView *webView;

@end


@implementation ViewController


- (void)viewDidLoad {
    [super viewDidLoad];
    WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
    self.webView = [[ContainerView alloc] initWithFrame:self.view.bounds configuration:configuration];
    [self.view addSubview:self.webView];
    self.webView.translatesAutoresizingMaskIntoConstraints = NO;
    // Load a webpage
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://127.0.0.1:3000?a=%ld",(long)CFAbsoluteTimeGetCurrent()]];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    [self.webView loadRequest:request];
    [self setupConstraints];
}


- (void)setupConstraints {
    // 约束web视图顶部、底部、左部和右部与父视图对齐
    NSDictionary *views = @{@"webView": self.webView};
    [self.view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[webView]|" options:0 metrics:nil views:views]];
    [self.view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[webView]|" options:0 metrics:nil views:views]];
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

#pragma mark - NSDraggingDestination



@end
