#import "settings_dialog_controller.h"
#include "application.h"
#import "settings_aria2_view_controller.h"
#import "settings_discovery_view_controller.h"
#import "settings_downloads_view_controller.h"
#import "settings_extractor_view_controller.h"
#import "settings_general_view_controller.h"
#import "settings_history_view_controller.h"
#import "settings_postprocessing_view_controller.h"
#import "settings_tab_view_controller.h"
#include "translation.h"

@implementation SettingsDialogController

- (instancetype)init
{
    NSWindow* window = [[NSWindow alloc]
            initWithContentRect:NSZeroRect
                      styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable)
                        backing:NSBackingStoreBuffered
                          defer:YES];
    window.releasedWhenClosed = NO;
    self = [super initWithWindow:window];
    if (self != nil)
    {
        const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
        NSTabViewController* tabViewController{ [[SettingsTabViewController alloc] init] };
        tabViewController.tabStyle = NSTabViewControllerTabStyleToolbar;
        NSArray<NSViewController*>* viewControllers = @[
            [[SettingsGeneralViewController alloc] init],
            [[SettingsHistoryViewController alloc] init],
            [[SettingsDownloadsViewController alloc] init],
            [[SettingsDiscoveryViewController alloc] init],
            [[SettingsExtractorViewController alloc] init],
            [[SettingsPostprocessingViewController alloc] init],
            [[SettingsAria2ViewController alloc] init],
        ];
        NSArray<NSString*>* labels = @[
            @(translator._p("Settings", "General")),
            @(translator._p("Settings", "History")),
            @(translator._p("Settings", "Downloads")),
            @(translator._p("Settings", "Discovery")),
            @(translator._p("Settings", "Extractor")),
            @(translator._p("Settings", "Postprocessing")),
            @(translator._p("Settings", "aria2")),
        ];
        NSArray<NSString*>* symbols = @[
            @"gearshape",
            @"clock.arrow.circlepath",
            @"arrow.down.circle",
            @"globe",
            @"film.stack",
            @"wrench.and.screwdriver",
            @"apple.terminal.on.rectangle",
        ];
        for (NSUInteger i{ 0 }; i < viewControllers.count; i++)
        {
            NSTabViewItem* item{ [NSTabViewItem tabViewItemWithViewController:viewControllers[i]] };
            item.label = labels[i];
            item.image = [NSImage imageWithSystemSymbolName:symbols[i] accessibilityDescription:labels[i]];
            [tabViewController addTabViewItem:item];
        }
        window.contentViewController = tabViewController;
    }
    return self;
}

- (void)show
{
    [self showWindow:nil];
    [self.window center];
    [self.window makeKeyAndOrderFront:nil];
    [NSApp activate];
}

@end
