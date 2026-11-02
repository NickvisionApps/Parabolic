#import <Cocoa/Cocoa.h>
#import "settings.h"

@interface MainMenuController : NSResponder
@property(strong) SettingsDialogController* settingsDialogController;
- (void)setup;
@end
