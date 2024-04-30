//
//  ContainerView.m
//  Vap
//
//  Created by 梁金锋 on 2024/4/29.
//

#import "ContainerView.h"

static NSArray *dragfiles = @[];

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
    dragfiles = @[];
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
    if ([files isKindOfClass:[NSString class]]) {
        files = @[files];
    }
    // 处理文件路径

    NSMutableArray *pathArray = [NSMutableArray array];
    for (NSString *filePath in files) {
       NSURL *u = [NSURL URLWithString:filePath];
        NSString *uPath = [u path];
        [pathArray addObject:uPath];
    }
    dragfiles = pathArray;
    return [super performDragOperation:sender];
}


@end
