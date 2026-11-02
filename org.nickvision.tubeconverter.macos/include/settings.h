#import <Cocoa/Cocoa.h>

@interface GeneralViewController : NSViewController
@end

@interface HistoryViewController : NSViewController
@end

@interface DownloadsViewController : NSViewController
@end

@interface DiscoveryViewController : NSViewController
@end

@interface ExtractorViewController : NSViewController
@end

@interface PostprocessingViewController : NSViewController
@end

@interface Aria2ViewController : NSViewController
@end

@interface SettingsDialogController : NSWindowController <NSToolbarDelegate, NSWindowDelegate>
- (void)show;
@end
