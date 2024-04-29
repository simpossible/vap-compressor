//
//  ContainerView.m
//  Vap
//
//  Created by 梁金锋 on 2024/4/29.
//

#import "ContainerView.h"

static NSArray *dragfiles = @[@"/Users/liangjinfeng/Downloads/aaaa"];

char * getAppDragFiles(void) {
    NSMutableArray *fileArray = [NSMutableArray array];
    for (NSString *file in dragfiles) {
        [fileArray addObject:@{
            @"path":file
        }];
    }
    NSDictionary *dic = @{
        @"files":fileArray
    };
    NSData * data = [NSJSONSerialization dataWithJSONObject:dic options:0 error:nil];
    NSString *str = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
//    dragfiles = @[];
    char *jsonChar = (char *)[str UTF8String];
    char *copystr = malloc(strlen(jsonChar)+1); //交给他人释放
    memcpy(copystr, jsonChar, strlen(jsonChar)+1);
    return copystr;
}

@implementation ContainerView


- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    // Drawing code here.
}

- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender {
    NSPasteboard *pasteboard = [sender draggingPasteboard];
    NSArray *files = [pasteboard propertyListForType:NSPasteboardTypeFileURL];
    if (files.count > 0) {
        dragfiles = files;
    }
    if (files.count > 0) {
        NSString *fileURL = files[0];
        NSLog(@"文件拖入成功，路径为: %@", fileURL);
        // 处理文件，例如加载文件等
    }
    return [super performDragOperation:sender];
}


@end
