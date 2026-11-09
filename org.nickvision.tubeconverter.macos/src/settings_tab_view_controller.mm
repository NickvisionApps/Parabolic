#import "settings_tab_view_controller.h"

@implementation SettingsTabViewController

- (void)tabView:(NSTabView*)tabView didSelectTabViewItem:(NSTabViewItem*)tabViewItem
{
    [super tabView:tabView didSelectTabViewItem:tabViewItem];
    NSWindow* window{ self.view.window };
    if (window == nil)
    {
        return;
    }
    [tabViewItem.viewController.view layoutSubtreeIfNeeded];
    NSRect frame{ window.frame };
    CGFloat height{
        [window frameRectForContentRect:NSMakeRect(0, 0, frame.size.width,
                                                   tabViewItem.viewController.view.fittingSize.height)]
                .size.height,
    };
    frame.origin.y += frame.size.height - height;
    frame.size.height = height;
    [window setFrame:frame display:YES animate:YES];
}

@end
