#import <Cocoa/Cocoa.h>
#import "settings_dialog_controller.h"

@interface MainMenuController : NSResponder
@property(strong) SettingsDialogController* settingsDialogController;
- (void)setup;
@end
