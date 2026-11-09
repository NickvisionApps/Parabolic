#import "app_delegate.h"
#include "application.h"
#include "configuration.h"
#import "main_menu_controller.h"
#import "main_window_controller.h"

@implementation AppDelegate

- (void)applicationWillFinishLaunching:(NSNotification*)notification
{
    self.mainMenuController = [[MainMenuController alloc] init];
    [self.mainMenuController setup];
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification
{
    parabolic::configuration::Theme theme{ parabolic::application::State::instance().configuration().theme() };
    switch (theme)
    {
    case parabolic::configuration::Theme::Light:
        NSApp.appearance = [NSAppearance appearanceNamed:NSAppearanceNameAqua];
        break;
    case parabolic::configuration::Theme::Dark:
        NSApp.appearance = [NSAppearance appearanceNamed:NSAppearanceNameDarkAqua];
        break;
    case parabolic::configuration::Theme::System:
    default:
        NSApp.appearance = nil;
        break;
    }
    self.mainWindowController = [[MainWindowController alloc] init];
    [self.mainWindowController show];
}

- (BOOL)applicationShouldHandleReopen:(NSApplication*)sender hasVisibleWindows:(BOOL)flag
{
    if (!flag)
    {
        [self.mainWindowController show];
    }
    return YES;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    return NO;
}

- (BOOL)applicationSupportsSecureRestorableState:(NSApplication*)app
{
    return YES;
}

@end
