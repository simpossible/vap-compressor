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
extern char *create_monitor_with_helpers(void);

extern void startVapServer(void);
extern int transcode_vap(char* inputPath, char *outputPath);


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
    // 拷贝resource文件到vap工作空间
//    dispatch_async(dispatch_get_global_queue(0, 0), ^{
//        [self startCompress];
//    });
    create_monitor_with_helpers();
//    struct Dictionary *aDic = dicCreate();
//    dicSetValue(aDic, @"1111".UTF8String, @"gogogo");
//    dicSetValue(aDic, @"2222".UTF8String, @"ggg");
//    dicSetValue(aDic, @"3333".UTF8String, @"bbbg");
//    NSString *a = (__bridge NSString *)dicGetValue(aDic, [@"1111" UTF8String]);
//    NSString *b = (__bridge NSString *)dicGetValue(aDic, @"2222".UTF8String);
//    NSString *c = (__bridge NSString *)dicGetValue(aDic, @"3333".UTF8String);
//    
//    NSString *input = @"/Users/liangjinfeng/Downloads/aaaa/video.mp4";
   
   
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


char *create_monitor_with_helpers(void)
{
    const unsigned int resolution_numbers[3][2] = {
        {1280, 720},
        {1920, 1080},
        {3840, 2160}
    };
    char *string = NULL;
    cJSON *resolutions = NULL;
    size_t index = 0;

    cJSON *monitor = cJSON_CreateObject();

    if (cJSON_AddStringToObject(monitor, "name", "Awesome 4K") == NULL)
    {
        goto end;
    }

    resolutions = cJSON_AddArrayToObject(monitor, "resolutions");
    if (resolutions == NULL)
    {
        goto end;
    }

    for (index = 0; index < (sizeof(resolution_numbers) / (2 * sizeof(int))); ++index)
    {
        cJSON *resolution = cJSON_CreateObject();

        if (cJSON_AddNumberToObject(resolution, "width", resolution_numbers[index][0]) == NULL)
        {
            goto end;
        }

        if (cJSON_AddNumberToObject(resolution, "height", resolution_numbers[index][1]) == NULL)
        {
            goto end;
        }

        cJSON_AddItemToArray(resolutions, resolution);
    }

    string = cJSON_Print(monitor);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print monitor.\n");
    }

end:
    cJSON_Delete(monitor);
    return string;
}
