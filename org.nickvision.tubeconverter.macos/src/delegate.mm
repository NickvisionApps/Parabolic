#import "delegate.h"
#import "menu.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)notification
{
    self.mainMenuController = [[MainMenuController alloc] init];
    [self.mainMenuController setup];
}

- (void)applicationWillTerminate:(NSNotification*)notification
{
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    return NO;
}

@end
