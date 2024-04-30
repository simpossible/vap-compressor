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

extern void startVapServer(void);


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

void onProgress(int p){
    printf("progress is %d", p);
}

@implementation AppDelegate


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {

   
    NSString * resourcePath = [[NSBundle mainBundle] pathForResource:@"resource" ofType:@""];
    if (![[NSFileManager defaultManager] fileExistsAtPath:resourcePath]) {
        return;
    }
    NSError *error;
    NSString *workSpacePath = vapServerGetDocumentPath();
    if ([[NSFileManager defaultManager] fileExistsAtPath:workSpacePath]) {
        [[NSFileManager defaultManager] removeItemAtPath:workSpacePath error:&error];
        if (error) {
            NSLog(@"get a error when remove old file");
        }
    }
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

    return;
}


@end


const char * vapServerWorkSpacePath(void) {
    NSString *vapWorkSpace =  vapServerGetDocumentPath();
    return vapWorkSpace.UTF8String;
    
}
