#import <Cocoa/Cocoa.h>

@class MainWindowController;
@class SettingsWindowController;

@interface AppDelegate : NSObject <NSApplicationDelegate, NSMenuItemValidation>
@property(nonatomic, strong) MainWindowController* mainWindowController;
@property(nonatomic, strong) SettingsWindowController* settingsWindowController;
@end
