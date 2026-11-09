#import <Cocoa/Cocoa.h>

@class MainMenuController;
@class MainWindowController;

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property(nonatomic, strong) MainMenuController* mainMenuController;
@property(nonatomic, strong) MainWindowController* mainWindowController;
@end
