#import "main_window_controller.h"
#include "application.h"
#include "translation.h"

@implementation MainWindowController

- (instancetype)init
{
    NSWindow* window{
        [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 1000, 700)
                                    styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                                              NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable |
                                              NSWindowStyleMaskFullSizeContentView
                                      backing:NSBackingStoreBuffered
                                        defer:NO],
    };
    window.releasedWhenClosed = NO;
    window.title = @(parabolic::application::State::instance().translator()._("Parabolic"));
    window.toolbar = [[NSToolbar alloc] initWithIdentifier:@"MainToolbar"];
    window.toolbarStyle = NSWindowToolbarStyleUnified;
    window.titlebarAppearsTransparent = YES;
    [window center];
    return [super initWithWindow:window];
}

- (void)show
{
    [self showWindow:nil];
    [self.window makeKeyAndOrderFront:nil];
    [self.window orderFrontRegardless];
    [NSApp activate];
}

@end
