#import <Cocoa/Cocoa.h>

@class SettingsDialogController;

@interface MainMenuController : NSObject <NSMenuItemValidation>
@property(nonatomic, strong) SettingsDialogController* settingsDialogController;
- (void)setup;
@end
