#import "main_window_controller.h"
#include "application.h"
#include "configuration.h"
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
    window.titlebarSeparatorStyle = NSTitlebarSeparatorStyleNone;
    parabolic::configuration::WindowGeometry geometry{
        parabolic::application::State::instance().configuration().window_geometry(),
    };
    NSRect frame{ NSMakeRect(geometry.x(), geometry.y(), geometry.width(), geometry.height()) };
    BOOL isOnScreen{ NO };
    for (NSScreen* screen in NSScreen.screens)
    {
        if (NSIntersectsRect(screen.visibleFrame, frame))
        {
            isOnScreen = YES;
            break;
        }
    }
    if (geometry == parabolic::configuration::WindowGeometry{} || !isOnScreen)
    {
        [window setContentSize:NSMakeSize(geometry.width(), geometry.height())];
        [window center];
    }
    else
    {
        [window setFrame:frame display:NO];
    }
    self = [super initWithWindow:window];
    if (self != nil)
    {
        window.delegate = self;
    }
    return self;
}

- (void)saveGeometry
{
    if ((self.window.styleMask & NSWindowStyleMaskFullScreen) != 0)
    {
        return;
    }
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::configuration::WindowGeometry geometry{ configuration.window_geometry() };
    geometry.set_is_maximized(self.window.zoomed);
    if (!self.window.zoomed)
    {
        geometry.set_width(static_cast<int>(self.window.frame.size.width));
        geometry.set_height(static_cast<int>(self.window.frame.size.height));
        geometry.set_x(static_cast<int>(self.window.frame.origin.x));
        geometry.set_y(static_cast<int>(self.window.frame.origin.y));
    }
    configuration.set_window_geometry(geometry);
}

- (void)show
{
    [self showWindow:nil];
    [self.window makeKeyAndOrderFront:nil];
    [self.window orderFrontRegardless];
    [NSApp activate];
    if (parabolic::application::State::instance().configuration().window_geometry().is_maximized() &&
        !self.window.zoomed)
    {
        [self.window zoom:nil];
    }
}

- (void)windowDidEndLiveResize:(NSNotification*)notification
{
    [self saveGeometry];
}

- (void)windowDidResize:(NSNotification*)notification
{
    if (!self.window.inLiveResize)
    {
        [self saveGeometry];
    }
}

- (void)windowWillClose:(NSNotification*)notification
{
    [self saveGeometry];
}

@end
