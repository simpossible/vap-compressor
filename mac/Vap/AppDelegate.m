//
//  AppDelegate.m
//  Vap
//
//  Created by 梁金锋 on 2024/4/18.
//

#import "AppDelegate.h"

extern void startVapServer(void);
extern int transcode_vap(char* inputPath, char *outputPath);

@interface AppDelegate ()

@property (nonatomic, strong) NSThread *thread;

@end

NSString * vapServerGetDocumentPath(void) {
    NSString *documentPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
    NSString *vapWorkSpace =  [documentPath stringByAppendingPathComponent:@"vap"];
    return vapWorkSpace;
}

void onProgress(int p){
    printf("progress is %d", p);
}

@implementation AppDelegate


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // 拷贝resource文件到vap工作空间
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        [self startCompress];
    });
   
    NSString * resourcePath = [[NSBundle mainBundle] pathForResource:@"resource" ofType:@""];
    if (![[NSFileManager defaultManager] fileExistsAtPath:resourcePath]) {
        return;
    }
    NSString *workSpacePath = vapServerGetDocumentPath();
    if (![[NSFileManager defaultManager] fileExistsAtPath:workSpacePath]) {
        NSError *error;
        [[NSFileManager defaultManager] copyItemAtPath:resourcePath toPath:workSpacePath error:&error];
        if (error) {
            NSLog(@"文件拷贝失败:%@", error.localizedDescription);
        }
    }

    self.thread = [[NSThread alloc] initWithBlock:^{
        startVapServer();
    }];
    [self.thread start];
    
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}

- (void)startCompress {
    NSString *input = @"/Users/liangjinfeng/Downloads/aaaa/b.mp4";
    NSString *output = @"/Users/liangjinfeng/Downloads/aaaa/b_1.mp4";
    transcode_vap(input.UTF8String, output.UTF8String);
    return;
}

@end


const char * vapServerWorkSpacePath(void) {
    NSString *vapWorkSpace =  vapServerGetDocumentPath();
    return vapWorkSpace.UTF8String;
    
}
