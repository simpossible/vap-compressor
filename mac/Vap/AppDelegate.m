//
//  AppDelegate.m
//  Vap
//
//  Created by 梁金锋 on 2024/4/18.
//

#import "AppDelegate.h"
#import "transcode.h"
#import "dictionary.h"
#import "Cjson.h"
#import "ViewController.h"
#import "adaptor.h"
extern void startVapServer(void);
extern void set_app_cache_dir(const char * path);

@interface AppDelegate ()

@property (nonatomic, strong) NSThread *thread;
@property (nonatomic, strong) NSThread *thread2;
@property (nonatomic, strong) NSThread *thread3;

@end

NSString * vapServerGetDocumentPath(void) {
    NSString *documentPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
    NSString *vapWorkSpace =  [documentPath stringByAppendingPathComponent:@"vap"];
    return vapWorkSpace;
}


@interface AppDelegate()<NSWindowDelegate>

@property (nonatomic, strong) NSWindow * window;

@end

@implementation AppDelegate


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
   
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentDirectory = [paths firstObject];
    set_app_cache_dir([documentDirectory UTF8String]);
    [self initialUI];
    
    NSString * resourcePath = [[NSBundle mainBundle] pathForResource:@"resource" ofType:@""];
    if (![[NSFileManager defaultManager] fileExistsAtPath:resourcePath]) {
        return;
    }
    
    setVapHtmlResourcePath([resourcePath UTF8String]);    
    vapServerStart();
        
}


- (void)initialUI{
    NSWindow *window = [[NSWindow alloc] initWithContentRect:NSMakeRect(100, 100, 1024, 560)
                                                   styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
  
    
    // 2. 创建一个NSViewController实例
    // 如果你有一个在Storyboard中定义的ViewController，可以通过以下方式获取：
    // NSViewController *viewController = [self.storyboard instantiateViewControllerWithIdentifier:@"YourViewControllerIdentifier"];
    
    // 或者，如果你在代码中创建它：
    NSViewController *viewController = [[ViewController alloc] init];
    [window setTitle:@"VapCompressor"];
    
    // 3. 设置NSViewController的View为NSWindow的内容视图
    [window setContentViewController:viewController];
    window.delegate = self;
    
    // 4. 显示NSWindow
    [window makeKeyAndOrderFront:self];
    [window setContentSize:NSMakeSize(1024, 560)];
    [window setContentMaxSize:NSMakeSize(1024, 560)];
    [window setContentMinSize:NSMakeSize(1024, 560)];
    [window setMinSize:NSMakeSize(1024, 560)];
    [window setMaxSize:NSMakeSize(1024, 560)];
    
    // 将新创建的窗口添加到应用程序的窗口数组中，这样它就可以接收事件
    self.window = window;
    
    // 设置窗口的初始状态
    [window center];
    [window setInitialFirstResponder:viewController.view];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}

- (void)startCompress {

    return;
}


- (BOOL)windowShouldClose:(NSWindow *)sende {
    exit(0);
    return YES;
}


@end


//const char * vapServerWorkSpacePath(void) {
//    NSString *vapWorkSpace =  vapServerGetDocumentPath();
//    return vapWorkSpace.UTF8String;
//    
//}
