#import "settings_tab_view_controller.h"
#import <QuartzCore/QuartzCore.h>

@implementation SettingsTabViewController

- (void)viewWillAppear
{
    [super viewWillAppear];
    self.transitionOptions = NSViewControllerTransitionNone;
    [self resizeWindowForTabViewItem:self.tabViewItems[static_cast<NSUInteger>(self.selectedTabViewItemIndex)]
                             animate:NO];
}

- (void)resizeWindowForTabViewItem:(NSTabViewItem*)tabViewItem animate:(BOOL)animate
{
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
    if (!animate)
    {
        [window setFrame:frame display:YES];
        return;
    }
    [NSAnimationContext runAnimationGroup:^(NSAnimationContext* context) {
      context.duration = 0.25;
      context.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
      context.allowsImplicitAnimation = YES;
      [[window animator] setFrame:frame display:YES];
    }];
}

- (void)tabView:(NSTabView*)tabView didSelectTabViewItem:(NSTabViewItem*)tabViewItem
{
    [super tabView:tabView didSelectTabViewItem:tabViewItem];
    [self resizeWindowForTabViewItem:tabViewItem animate:YES];
}

@end
