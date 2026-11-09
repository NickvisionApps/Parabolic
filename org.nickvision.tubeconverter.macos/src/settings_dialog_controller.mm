#import "settings_dialog_controller.h"
#include "application.h"
#import "settings_aria2_view_controller.h"
#import "settings_discovery_view_controller.h"
#import "settings_downloads_view_controller.h"
#import "settings_extractor_view_controller.h"
#import "settings_general_view_controller.h"
#import "settings_history_view_controller.h"
#import "settings_postprocessing_view_controller.h"
#include "translation.h"

#define K_ARIA2_TAB_ID @"SettingsTabAria2"
#define K_DISCOVERY_TAB_ID @"SettingsTabDiscovery"
#define K_DOWNLOADS_TAB_ID @"SettingsTabDownloads"
#define K_EXTRACTOR_TAB_ID @"SettingsTabExtractor"
#define K_GENERAL_TAB_ID @"SettingsTabGeneral"
#define K_HISTORY_TAB_ID @"SettingsTabHistory"
#define K_POSTPROCESSING_TAB_ID @"SettingsTabPostprocessing"

@interface SettingsDialogController ()
@property(nonatomic, strong) NSArray<NSToolbarItemIdentifier>* tabIdentifiers;
@property(nonatomic, strong) NSViewController* aria2ViewController;
@property(nonatomic, strong) NSViewController* discoveryViewController;
@property(nonatomic, strong) NSViewController* downloadsViewController;
@property(nonatomic, strong) NSViewController* extractorViewController;
@property(nonatomic, strong) NSViewController* generalViewController;
@property(nonatomic, strong) NSViewController* historyViewController;
@property(nonatomic, strong) NSViewController* postprocessingViewController;
@end

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
        self.tabIdentifiers = @[
            K_GENERAL_TAB_ID,
            K_HISTORY_TAB_ID,
            K_DOWNLOADS_TAB_ID,
            K_DISCOVERY_TAB_ID,
            K_EXTRACTOR_TAB_ID,
            K_POSTPROCESSING_TAB_ID,
            K_ARIA2_TAB_ID,
        ];
        self.generalViewController = [[SettingsGeneralViewController alloc] init];
        self.historyViewController = [[SettingsHistoryViewController alloc] init];
        self.downloadsViewController = [[SettingsDownloadsViewController alloc] init];
        self.discoveryViewController = [[SettingsDiscoveryViewController alloc] init];
        self.extractorViewController = [[SettingsExtractorViewController alloc] init];
        self.postprocessingViewController = [[SettingsPostprocessingViewController alloc] init];
        self.aria2ViewController = [[SettingsAria2ViewController alloc] init];
        NSToolbar* toolbar{ [[NSToolbar alloc] initWithIdentifier:@"SettingsToolbar"] };
        toolbar.delegate = self;
        toolbar.allowsUserCustomization = NO;
        toolbar.displayMode = NSToolbarDisplayModeIconAndLabel;
        toolbar.selectedItemIdentifier = K_GENERAL_TAB_ID;
        window.delegate = self;
        window.toolbar = toolbar;
        window.toolbarStyle = NSWindowToolbarStylePreference;
        window.contentViewController = self.generalViewController;
        [window bind:NSTitleBinding toObject:window withKeyPath:@"contentViewController.title" options:nil];
    }
    return self;
}

- (void)show
{
    [self showWindow:nil];
    [self.window center];
    [self.window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

- (NSArray<NSToolbarItemIdentifier>*)toolbarAllowedItemIdentifiers:(NSToolbar*)toolbar
{
    return self.tabIdentifiers;
}

- (NSArray<NSToolbarItemIdentifier>*)toolbarDefaultItemIdentifiers:(NSToolbar*)toolbar
{
    return self.tabIdentifiers;
}

- (NSArray<NSToolbarItemIdentifier>*)toolbarSelectableItemIdentifiers:(NSToolbar*)toolbar
{
    return self.tabIdentifiers;
}

- (NSToolbarItem*)toolbar:(NSToolbar*)toolbar
            itemForItemIdentifier:(NSToolbarItemIdentifier)itemIdentifier
        willBeInsertedIntoToolbar:(BOOL)flag
{
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    NSToolbarItem* item{ [[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] };
    if ([itemIdentifier isEqualToString:K_GENERAL_TAB_ID])
    {
        item.label = @(translator._p("Settings", "General"));
        item.paletteLabel = @(translator._p("Settings", "General"));
        item.image = [NSImage imageWithSystemSymbolName:@"gearshape" accessibilityDescription:item.label];
    }
    else if ([itemIdentifier isEqualToString:K_HISTORY_TAB_ID])
    {
        item.label = @(translator._p("Settings", "History"));
        item.paletteLabel = @(translator._p("Settings", "History"));
        item.image = [NSImage imageWithSystemSymbolName:@"clock.arrow.circlepath" accessibilityDescription:item.label];
    }
    else if ([itemIdentifier isEqualToString:K_DOWNLOADS_TAB_ID])
    {
        item.label = @(translator._p("Settings", "Downloads"));
        item.paletteLabel = @(translator._p("Settings", "Downloads"));
        item.image = [NSImage imageWithSystemSymbolName:@"arrow.down.circle" accessibilityDescription:item.label];
    }
    else if ([itemIdentifier isEqualToString:K_DISCOVERY_TAB_ID])
    {
        item.label = @(translator._p("Settings", "Discovery"));
        item.paletteLabel = @(translator._p("Settings", "Discovery"));
        item.image = [NSImage imageWithSystemSymbolName:@"globe" accessibilityDescription:item.label];
    }
    else if ([itemIdentifier isEqualToString:K_EXTRACTOR_TAB_ID])
    {
        item.label = @(translator._p("Settings", "Extractor"));
        item.paletteLabel = @(translator._p("Settings", "Extractor"));
        item.image = [NSImage imageWithSystemSymbolName:@"film.stack" accessibilityDescription:item.label];
    }
    else if ([itemIdentifier isEqualToString:K_POSTPROCESSING_TAB_ID])
    {
        item.label = @(translator._p("Settings", "Postprocessing"));
        item.paletteLabel = @(translator._p("Settings", "Postprocessing"));
        item.image = [NSImage imageWithSystemSymbolName:@"wrench.and.screwdriver" accessibilityDescription:item.label];
    }
    else if ([itemIdentifier isEqualToString:K_ARIA2_TAB_ID])
    {
        item.label = @(translator._p("Settings", "aria2"));
        item.paletteLabel = @(translator._p("Settings", "aria2"));
        item.image = [NSImage imageWithSystemSymbolName:@"apple.terminal.on.rectangle"
                               accessibilityDescription:item.label];
    }
    item.target = self;
    item.action = @selector(toolbarItemClicked:);
    return item;
}

- (void)toolbarItemClicked:(NSToolbarItem*)sender
{
    if ([sender.itemIdentifier isEqualToString:K_GENERAL_TAB_ID])
    {
        self.window.contentViewController = self.generalViewController;
    }
    else if ([sender.itemIdentifier isEqualToString:K_HISTORY_TAB_ID])
    {
        self.window.contentViewController = self.historyViewController;
    }
    else if ([sender.itemIdentifier isEqualToString:K_DOWNLOADS_TAB_ID])
    {
        self.window.contentViewController = self.downloadsViewController;
    }
    else if ([sender.itemIdentifier isEqualToString:K_DISCOVERY_TAB_ID])
    {
        self.window.contentViewController = self.discoveryViewController;
    }
    else if ([sender.itemIdentifier isEqualToString:K_EXTRACTOR_TAB_ID])
    {
        self.window.contentViewController = self.extractorViewController;
    }
    else if ([sender.itemIdentifier isEqualToString:K_POSTPROCESSING_TAB_ID])
    {
        self.window.contentViewController = self.postprocessingViewController;
    }
    else if ([sender.itemIdentifier isEqualToString:K_ARIA2_TAB_ID])
    {
        self.window.contentViewController = self.aria2ViewController;
    }
    self.window.toolbar.selectedItemIdentifier = sender.itemIdentifier;
}

@end
