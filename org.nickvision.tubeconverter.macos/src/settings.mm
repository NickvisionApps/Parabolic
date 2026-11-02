#import "settings.h"
#include "application.h"
#include "configuration.h"
#include "translation.h"
#include "update.h"

#define K_GENERAL_TAB_ID @"SettingsTabGeneral"
#define K_HISTORY_TAB_ID @"SettingsTabHistory"
#define K_DOWNLOADS_TAB_ID @"SettingsTabDownloads"
#define K_DISCOVERY_TAB_ID @"SettingsTabDiscovery"
#define K_EXTRACTOR_TAB_ID @"SettingsTabExtractor"
#define K_POSTPROCESSING_TAB_ID @"SettingsTabPostprocessing"

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
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    self.title = @(translator._p("Settings", "General"));
    NSTextField* themeLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Theme")).c_str())],
    };
    themeLabel.translatesAutoresizingMaskIntoConstraints = NO;
    themeLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* themePopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    themePopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    [themePopUpButton addItemsWithTitles:@[
        @(translator._p("Theme", "Light")),
        @(translator._p("Theme", "Dark")),
        @(translator._p("Theme", "System")),
    ]];
    [[themePopUpButton itemAtIndex:0] setTag:static_cast<NSInteger>(parabolic::configuration::Theme::Light)];
    [[themePopUpButton itemAtIndex:1] setTag:static_cast<NSInteger>(parabolic::configuration::Theme::Dark)];
    [[themePopUpButton itemAtIndex:2] setTag:static_cast<NSInteger>(parabolic::configuration::Theme::System)];
    [themePopUpButton selectItemWithTag:static_cast<NSInteger>(configuration.theme())];
    themePopUpButton.target = self;
    themePopUpButton.action = @selector(themeChanged:);
    NSTextField* languageLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Translation Language")).c_str())],
    };
    languageLabel.translatesAutoresizingMaskIntoConstraints = NO;
    languageLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* languagePopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    languagePopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    NSMenuItem* followSystemLanguageItem
    {
        [[NSMenuItem alloc] initWithTitle:@(translator._p("Translation Language", "Follow System Language"))
                                   action:nil
                            keyEquivalent:@""]
    };
    followSystemLanguageItem.representedObject =
            @(parabolic::translation::Translator::follow_system_language().c_str());
    [languagePopUpButton.menu addItem:followSystemLanguageItem];
    NSLocale* currentLocale{ [NSLocale currentLocale] };
    for (const std::string& language : translator.available_languages())
    {
        NSString* identifier
        {
            @(language.c_str())
        };
        NSString* displayName{ [currentLocale localizedStringForLocaleIdentifier:identifier] };
        NSMenuItem* languageItem{ [[NSMenuItem alloc] initWithTitle:displayName.length > 0 ? displayName : identifier
                                                             action:nil
                                                      keyEquivalent:@""] };
        languageItem.representedObject = identifier;
        [languagePopUpButton.menu addItem:languageItem];
    }
    NSString* currentLanguage
    {
        @(configuration.translation_options().language().c_str())
    };
    for (NSMenuItem* languageItem in languagePopUpButton.itemArray)
    {
        if ([languageItem.representedObject isEqualToString:currentLanguage])
        {
            [languagePopUpButton selectItem:languageItem];
            break;
        }
    }
    languagePopUpButton.target = self;
    languagePopUpButton.action = @selector(languageChanged:);
    NSButton* autoUpdateCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Automatically Check for Updates"))
                             target:self
                             action:@selector(autoUpdateChanged:)]
    };
    autoUpdateCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    autoUpdateCheckbox.state =
            configuration.automatically_check_for_updates() ? NSControlStateValueOn : NSControlStateValueOff;
    NSButton* allowPreviewUpdatesCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Allow Preview Updates"))
                             target:self
                             action:@selector(allowPreviewUpdatesChanged:)]
    };
    allowPreviewUpdatesCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    allowPreviewUpdatesCheckbox.state =
            configuration.update_options().allow_preview_updates() ? NSControlStateValueOn : NSControlStateValueOff;
    [self.view addSubview:themeLabel];
    [self.view addSubview:themePopUpButton];
    [self.view addSubview:languageLabel];
    [self.view addSubview:languagePopUpButton];
    [self.view addSubview:autoUpdateCheckbox];
    [self.view addSubview:allowPreviewUpdatesCheckbox];
    [NSLayoutConstraint activateConstraints:@[
        [self.view.widthAnchor constraintEqualToConstant:600],
        [themeLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [themeLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [themePopUpButton.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [themePopUpButton.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [themePopUpButton.widthAnchor constraintGreaterThanOrEqualToConstant:100],
        [themeLabel.firstBaselineAnchor constraintEqualToAnchor:themePopUpButton.firstBaselineAnchor],
        [themePopUpButton.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [languageLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [languageLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [languagePopUpButton.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [languagePopUpButton.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [languagePopUpButton.widthAnchor constraintGreaterThanOrEqualToConstant:100],
        [languageLabel.firstBaselineAnchor constraintEqualToAnchor:languagePopUpButton.firstBaselineAnchor],
        [languagePopUpButton.topAnchor constraintEqualToAnchor:themePopUpButton.bottomAnchor constant:12],
        [autoUpdateCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [autoUpdateCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [autoUpdateCheckbox.topAnchor constraintEqualToAnchor:languagePopUpButton.bottomAnchor constant:16],
        [allowPreviewUpdatesCheckbox.leadingAnchor constraintEqualToAnchor:autoUpdateCheckbox.leadingAnchor
                                                                  constant:18],
        [allowPreviewUpdatesCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                             constant:-20],
        [allowPreviewUpdatesCheckbox.topAnchor constraintEqualToAnchor:autoUpdateCheckbox.bottomAnchor constant:8],
        [allowPreviewUpdatesCheckbox.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
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

- (void)languageChanged:(NSPopUpButton*)sender
{
    NSString* identifier{ sender.selectedItem.representedObject };
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::translation::Options options{ configuration.translation_options() };
    options.set_language(identifier.UTF8String);
    configuration.set_translation_options(options);
}

- (void)autoUpdateChanged:(NSButton*)sender
{
    parabolic::application::State::instance().configuration().set_automatically_check_for_updates(
            sender.state == NSControlStateValueOn);
}

- (void)allowPreviewUpdatesChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::update::Options options{ configuration.update_options() };
    options.set_allow_preview_updates(sender.state == NSControlStateValueOn);
    configuration.set_update_options(options);
}

@end

@implementation HistoryViewController

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 600, 150)];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.title = @(parabolic::application::State::instance().translator()._p("Settings", "History"));
}

@end

@implementation DownloadsViewController

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 600, 150)];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.title = @(parabolic::application::State::instance().translator()._p("Settings", "Downloads"));
}

@end

@implementation DiscoveryViewController

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 600, 150)];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.title = @(parabolic::application::State::instance().translator()._p("Settings", "Discovery"));
}

@end

@implementation ExtractorViewController

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 600, 150)];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.title = @(parabolic::application::State::instance().translator()._p("Settings", "Extractor"));
}

@end

@implementation PostprocessingViewController

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 600, 150)];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.title = @(parabolic::application::State::instance().translator()._p("Settings", "Postprocessing"));
}

@end

@interface SettingsDialogController ()
@property(nonatomic, strong) NSArray<NSToolbarItemIdentifier>* tabIdentifiers;
@property(nonatomic, strong) NSViewController* generalViewController;
@property(nonatomic, strong) NSViewController* historyViewController;
@property(nonatomic, strong) NSViewController* downloadsViewController;
@property(nonatomic, strong) NSViewController* discoveryViewController;
@property(nonatomic, strong) NSViewController* extractorViewController;
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
        ];
        self.generalViewController = [[GeneralViewController alloc] init];
        self.historyViewController = [[HistoryViewController alloc] init];
        self.downloadsViewController = [[DownloadsViewController alloc] init];
        self.discoveryViewController = [[DiscoveryViewController alloc] init];
        self.extractorViewController = [[ExtractorViewController alloc] init];
        self.postprocessingViewController = [[PostprocessingViewController alloc] init];
        NSToolbar* toolbar{ [[NSToolbar alloc] initWithIdentifier:@"SettingsToolbar"] };
        toolbar.delegate = self;
        toolbar.allowsUserCustomization = NO;
        toolbar.displayMode = NSToolbarDisplayModeIconAndLabel;
        toolbar.selectedItemIdentifier = K_GENERAL_TAB_ID;
        window.delegate = self;
        window.toolbar = toolbar;
        window.toolbarStyle = NSWindowToolbarStylePreference;
        window.contentViewController = self.generalViewController;
        window.minSize = NSMakeSize(300, 120);
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
    NSImageSymbolConfiguration* symbolConfiguration{ [NSImageSymbolConfiguration
            configurationWithPointSize:18
                                weight:NSFontWeightRegular
                                 scale:NSImageSymbolScaleMedium] };
    if ([itemIdentifier isEqualToString:K_GENERAL_TAB_ID])
    {
        item.label = @(translator._p("Settings", "General"));
        item.paletteLabel = @(translator._p("Settings", "General"));
        item.image = [[NSImage imageWithSystemSymbolName:@"gearshape"
                                accessibilityDescription:item.label] imageWithSymbolConfiguration:symbolConfiguration];
    }
    else if ([itemIdentifier isEqualToString:K_HISTORY_TAB_ID])
    {
        item.label = @(translator._p("Settings", "History"));
        item.paletteLabel = @(translator._p("Settings", "History"));
        item.image = [[NSImage imageWithSystemSymbolName:@"clock.arrow.circlepath"
                                accessibilityDescription:item.label] imageWithSymbolConfiguration:symbolConfiguration];
    }
    else if ([itemIdentifier isEqualToString:K_DOWNLOADS_TAB_ID])
    {
        item.label = @(translator._p("Settings", "Downloads"));
        item.paletteLabel = @(translator._p("Settings", "Downloads"));
        item.image = [[NSImage imageWithSystemSymbolName:@"square.and.arrow.down"
                                accessibilityDescription:item.label] imageWithSymbolConfiguration:symbolConfiguration];
    }
    else if ([itemIdentifier isEqualToString:K_DISCOVERY_TAB_ID])
    {
        item.label = @(translator._p("Settings", "Discovery"));
        item.paletteLabel = @(translator._p("Settings", "Discovery"));
        item.image = [[NSImage imageWithSystemSymbolName:@"magnifyingglass"
                                accessibilityDescription:item.label] imageWithSymbolConfiguration:symbolConfiguration];
    }
    else if ([itemIdentifier isEqualToString:K_EXTRACTOR_TAB_ID])
    {
        item.label = @(translator._p("Settings", "Extractor"));
        item.paletteLabel = @(translator._p("Settings", "Extractor"));
        item.image = [[NSImage imageWithSystemSymbolName:@"globe"
                                accessibilityDescription:item.label] imageWithSymbolConfiguration:symbolConfiguration];
    }
    else if ([itemIdentifier isEqualToString:K_POSTPROCESSING_TAB_ID])
    {
        item.label = @(translator._p("Settings", "Postprocessing"));
        item.paletteLabel = @(translator._p("Settings", "Postprocessing"));
        item.image = [[NSImage imageWithSystemSymbolName:@"film.stack"
                                accessibilityDescription:item.label] imageWithSymbolConfiguration:symbolConfiguration];
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
    self.window.toolbar.selectedItemIdentifier = sender.itemIdentifier;
}

@end
