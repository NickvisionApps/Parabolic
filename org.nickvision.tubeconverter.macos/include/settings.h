#import <Cocoa/Cocoa.h>

@interface GeneralViewController : NSViewController
@end

@interface SettingsDialogController : NSWindowController <NSToolbarDelegate, NSWindowDelegate>
- (void)show;
@end
