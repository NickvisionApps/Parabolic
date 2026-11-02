#import "settings.h"
#include "application.h"
#include "configuration.h"

#define K_GENERAL_TAB_ID @"SettingsTabGeneral"

@implementation GeneralViewController

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 500, 300)];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    self.title = @(translator._p("Settings", "General"));
    NSTextField* themeLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Theme")).c_str())]
    };
    themeLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* themePopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    [themePopUpButton addItemsWithTitles:@[
        @(translator._p("Theme", "Light")), @(translator._p("Theme", "Dark")), @(translator._p("Theme", "System"))
    ]];
    [[themePopUpButton itemAtIndex:0] setTag:static_cast<NSInteger>(parabolic::configuration::Theme e::Light)];
    [[themePopUpButton itemAtIndex:1] setTag:static_cast<NSInteger>(parabolic::configuration::Theme::Dark)];
    [[themePopUpButton itemAtIndex:2] setTag:static_cast<NSInteger>(parabolic::configuration::Theme::System)];
    parabolic::configuration::Theme currentTheme{ parabolic::application::State::instance().configuration().theme() };
    [themePopUpButton selectItemWithTag:static_cast<NSInteger>(currentTheme)];
    themePopUpButton.target = self;
    themePopUpButton.action = @selector(themeChanged:);
    NSGridView* grid
    {
        [NSGridView gridViewWithViews:@[ @[ themeLabel, themePopUpButton ] ]]
    };
    grid.translatesAutoresizingMaskIntoConstraints = NO;
    grid.rowSpacing = 12;
    grid.columnSpacing = 8;
    [grid columnAtIndex:0].xPlacement = NSGridCellPlacementTrailing;
    [grid columnAtIndex:1].xPlacement = NSGridCellPlacementLeading;
    [grid cellAtColumnIndex:0 rowIndex:0].yPlacement = NSGridCellPlacementCenter;
    [grid cellAtColumnIndex:1 rowIndex:0].yPlacement = NSGridCellPlacementCenter;
    [self.view addSubview:grid];
    [NSLayoutConstraint activateConstraints:@[
        [grid.centerXAnchor constraintEqualToAnchor:self.view.centerXAnchor],
        [grid.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20]
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
            initWithContentRect:NSMakeRect(0, 0, 500, 300)
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
        window.delegate = self;
        window.toolbar = toolbar;
        window.toolbarStyle = NSWindowToolbarStylePreference;
        window.contentView = self.generalViewController.view;
        window.title = self.generalViewController.title;
        window.toolbar.selectedItemIdentifier = K_GENERAL_TAB_ID;
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
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    if ([sender.itemIdentifier isEqualToString:K_GENERAL_TAB_ID])
    {
        self.window.contentView = self.generalViewController.view;
        self.window.title = self.generalViewController.title;
    }
    self.window.toolbar.selectedItemIdentifier = sender.itemIdentifier;
}

@end
