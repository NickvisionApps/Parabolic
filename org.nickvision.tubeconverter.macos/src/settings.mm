#import "settings.h"
#include "application.h"

#define K_GENERAL_TAB_ID @"SettingsTabGeneral"

@interface SettingsDialogController ()
@property(nonatomic, strong) NSViewController* generalViewController;
@end

@implementation SettingsDialogController

- (instancetype)init
{
    NSWindow* window = [[NSWindow alloc]
            initWithContentRect:NSMakeRect(0, 0, 500, 300)
                      styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable)
                        backing:NSBackingStoreBuffered
                          defer:YES];
    window.releasedWhenClosed = NO;
    self = [super initWithWindow:window];
    if (self != nil)
    {
        const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
        self.generalViewController = [[NSViewController alloc] init];
        self.generalViewController.title = @(translator._("General"));
        self.generalViewController.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 500, 300)];
        NSToolbar* toolbar{ [[NSToolbar alloc] initWithIdentifier:@"SettingsToolbar"] };
        toolbar.delegate = self;
        toolbar.allowsUserCustomization = NO;
        toolbar.displayMode = NSToolbarDisplayModeIconAndLabel;
        window.delegate = self;
        window.toolbar = toolbar;
        window.toolbarStyle = NSWindowToolbarStylePreference;
        [window center];
    }
    return self;
}

- (void)show
{
    [self showWindow:nil];
    [self.window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

- (NSArray<NSToolbarItemIdentifier>*)toolbarAllowedItemIdentifiers:(NSToolbar*)toolbar
{
    return @[ K_GENERAL_TAB_ID ];
}

- (NSArray<NSToolbarItemIdentifier>*)toolbarDefaultItemIdentifiers:(NSToolbar*)toolbar
{
    return @[ K_GENERAL_TAB_ID ];
}

- (NSArray<NSToolbarItemIdentifier>*)toolbarSelectableItemIdentifiers:(NSToolbar*)toolbar
{
    return @[ K_GENERAL_TAB_ID ];
}

- (NSToolbarItem*)toolbar:(NSToolbar*)toolbar
            itemForItemIdentifier:(NSToolbarItemIdentifier)itemIdentifier
        willBeInsertedIntoToolbar:(BOOL)flag
{
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    NSToolbarItem* item{ [[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] };
    if ([itemIdentifier isEqualToString:K_GENERAL_TAB_ID])
    {
        item.label = @(translator._("General"));
        item.paletteLabel = @(translator._("General"));
        item.image = [NSImage imageWithSystemSymbolName:@"gearshape"
                               accessibilityDescription:@(translator._("General"))];
    }
    item.target = self;
    item.action = @selector(toolbarItemClicked:);
    return item;
}

- (void)toolbarItemClicked:(NSToolbarItem*)sender
{
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    if ([sender.itemIdentifier isEqualToString:K_GENERAL_TAB_ID])
    {
        self.window.contentView = self.generalViewController.view;
        self.window.title = self.generalViewController.title;
    }
    self.window.toolbar.selectedItemIdentifier = sender.itemIdentifier;
}

@end
