#import "settings.h"
#include "application.h"
#include "configuration.h"

#define K_GENERAL_TAB_ID @"SettingsTabGeneral"

@implementation GeneralViewController

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSZeroRect];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.translatesAutoresizingMaskIntoConstraints = NO;
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    self.title = @(translator._p("Settings", "General"));
    NSTextField* theme_label
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Theme")).c_str())]
    };
    theme_label.translatesAutoresizingMaskIntoConstraints = NO;
    theme_label.alignment = NSTextAlignmentRight;
    NSPopUpButton* theme_pop_up_button{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    theme_pop_up_button.translatesAutoresizingMaskIntoConstraints = NO;
    [theme_pop_up_button addItemsWithTitles:@[
        @(translator._p("Theme", "Light")),
        @(translator._p("Theme", "Dark")),
        @(translator._p("Theme", "System")),
    ]];
    [[theme_pop_up_button itemAtIndex:0] setTag:static_cast<NSInteger>(parabolic::configuration::Theme::Light)];
    [[theme_pop_up_button itemAtIndex:1] setTag:static_cast<NSInteger>(parabolic::configuration::Theme::Dark)];
    [[theme_pop_up_button itemAtIndex:2] setTag:static_cast<NSInteger>(parabolic::configuration::Theme::System)];
    parabolic::configuration::Theme current_theme{ parabolic::application::State::instance().configuration().theme() };
    [theme_pop_up_button selectItemWithTag:static_cast<NSInteger>(current_theme)];
    theme_pop_up_button.target = self;
    theme_pop_up_button.action = @selector(themeChanged:);
    [self.view addSubview:theme_label];
    [self.view addSubview:theme_pop_up_button];
    [NSLayoutConstraint activateConstraints:@[
        [self.view.widthAnchor constraintEqualToConstant:400],
        [theme_label.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-6],
        [theme_label.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [theme_pop_up_button.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:6],
        [theme_pop_up_button.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [theme_pop_up_button.widthAnchor constraintGreaterThanOrEqualToConstant:100],
        [theme_label.firstBaselineAnchor constraintEqualToAnchor:theme_pop_up_button.firstBaselineAnchor],
        [theme_pop_up_button.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [theme_pop_up_button.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20]
    ]];
}

- (void)themeChanged:(NSPopUpButton*)sender
{
    parabolic::configuration::Theme theme{ static_cast<parabolic::configuration::Theme>(sender.selectedItem.tag) };
    parabolic::application::State::instance().configuration().set_theme(theme);
    switch (theme)
    {
    case parabolic::configuration::Theme::Light:
        NSApp.appearance = [NSAppearance appearanceNamed:NSAppearanceNameAqua];
        break;
    case parabolic::configuration::Theme::Dark:
        NSApp.appearance = [NSAppearance appearanceNamed:NSAppearanceNameDarkAqua];
        break;
    case parabolic::configuration::Theme::System:
    default:
        NSApp.appearance = nil;
        break;
    }
}

@end

@interface SettingsDialogController ()
@property(nonatomic, strong) NSViewController* generalViewController;
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
        self.generalViewController = [[GeneralViewController alloc] init];
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
            itemForItemIdentifier:(NSToolbarItemIdentifier)item_identifier
        willBeInsertedIntoToolbar:(BOOL)flag
{
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    NSToolbarItem* item{ [[NSToolbarItem alloc] initWithItemIdentifier:item_identifier] };
    if ([item_identifier isEqualToString:K_GENERAL_TAB_ID])
    {
        item.label = @(translator._p("Settings", "General"));
        item.paletteLabel = @(translator._p("Settings", "General"));
        item.image = [NSImage imageWithSystemSymbolName:@"gearshape"
                               accessibilityDescription:@(translator._p("Settings", "General"))];
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
    self.window.toolbar.selectedItemIdentifier = sender.itemIdentifier;
}

@end
