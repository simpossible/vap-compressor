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

@property (nonatomic, strong) NSString * cachePath;

@end

@implementation AppDelegate


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
   
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSMoviesDirectory, NSUserDomainMask, YES);
    NSString *documentDirectory = [paths firstObject];
    NSString *cachePath = [documentDirectory stringByAppendingPathComponent:@"vapc"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:cachePath]) {
        NSError *error;
        [[NSFileManager defaultManager] createDirectoryAtPath:cachePath withIntermediateDirectories:YES attributes:nil error:&error];
        if (error) {
            NSLog(@"error is %@",error);
        }
    }
    self.cachePath = cachePath;
    set_app_cache_dir([cachePath UTF8String]);
    [self removeEmptySubdirectoriesAtPath:cachePath];
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
    NSLog(@"applicationWillTerminate");
    [self removeEmptySubdirectoriesAtPath:self.cachePath];
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


- (void)removeEmptySubdirectoriesAtPath:(NSString *)directoryPath {
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDirectoryEnumerator *enumerator = [fileManager enumeratorAtPath:directoryPath];
    NSString *subdirectory;
    NSMutableArray *emptyDirectories = [NSMutableArray array];

    // Collect all empty subdirectories
    while ((subdirectory = [enumerator nextObject])) {
        NSString *fullPath = [directoryPath stringByAppendingPathComponent:subdirectory];
        BOOL isDirectory;
        if ([fileManager fileExistsAtPath:fullPath isDirectory:&isDirectory] && isDirectory) {
            NSArray *contents = [fileManager contentsOfDirectoryAtPath:fullPath error:nil];
            if (contents.count == 0) {
                [emptyDirectories addObject:fullPath];
            }
        }
    }

    // Remove all empty subdirectories
    for (NSString *emptyDirectory in emptyDirectories) {
        NSError *error = nil;
        if (![fileManager removeItemAtPath:emptyDirectory error:&error]) {
            NSLog(@"Failed to remove directory: %@, error: %@", emptyDirectory, error);
        }
    }
}


@end

extern char * OpenFinderForPath(char *path) {
    NSString *filePath = [NSString stringWithUTF8String:path];
    if([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        NSURL *fileURL = [NSURL fileURLWithPath:filePath];
        [[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[fileURL]];
        [[NSWorkspace sharedWorkspace] openURL:[NSURL fileURLWithPath:@"/System/Library/CoreServices/Finder.app"]];
    }
    return "ok";
}

//const char * vapServerWorkSpacePath(void) {
//    NSString *vapWorkSpace =  vapServerGetDocumentPath();
//    return vapWorkSpace.UTF8String;
//    
//}
