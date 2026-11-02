#import "settings.h"
#include "application.h"
#include "configuration.h"
#include "download.h"
#include "history.h"
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
        [[NSMenuItem alloc] initWithTitle:@(translator._p("Language", "System")) action:nil keyEquivalent:@""]
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
        [self.view.widthAnchor constraintEqualToConstant:700],
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
        [allowPreviewUpdatesCheckbox.leadingAnchor constraintEqualToAnchor:autoUpdateCheckbox.leadingAnchor],
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
    self.view = [[NSView alloc] initWithFrame:NSZeroRect];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.translatesAutoresizingMaskIntoConstraints = NO;
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    self.title = @(translator._p("Settings", "History"));
    NSTextField* historyLengthLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Keep For")).c_str())],
    };
    historyLengthLabel.translatesAutoresizingMaskIntoConstraints = NO;
    historyLengthLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* historyLengthPopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    historyLengthPopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    [historyLengthPopUpButton addItemsWithTitles:@[
        @(translator._("Never")),
        @(translator._("1 Day")),
        @(translator._("1 Week")),
        @(translator._("1 Month")),
        @(translator._("3 Months")),
        @(translator._("6 Months")),
        @(translator._("1 Year")),
        @(translator._("Forever")),
    ]];
    [[historyLengthPopUpButton itemAtIndex:0] setTag:static_cast<NSInteger>(parabolic::history::Length::Never)];
    [[historyLengthPopUpButton itemAtIndex:1] setTag:static_cast<NSInteger>(parabolic::history::Length::OneDay)];
    [[historyLengthPopUpButton itemAtIndex:2] setTag:static_cast<NSInteger>(parabolic::history::Length::OneWeek)];
    [[historyLengthPopUpButton itemAtIndex:3] setTag:static_cast<NSInteger>(parabolic::history::Length::OneMonth)];
    [[historyLengthPopUpButton itemAtIndex:4] setTag:static_cast<NSInteger>(parabolic::history::Length::ThreeMonths)];
    [[historyLengthPopUpButton itemAtIndex:5] setTag:static_cast<NSInteger>(parabolic::history::Length::SixMonths)];
    [[historyLengthPopUpButton itemAtIndex:6] setTag:static_cast<NSInteger>(parabolic::history::Length::OneYear)];
    [[historyLengthPopUpButton itemAtIndex:7] setTag:static_cast<NSInteger>(parabolic::history::Length::Forever)];
    [historyLengthPopUpButton selectItemWithTag:static_cast<NSInteger>(configuration.history_options().length())];
    historyLengthPopUpButton.target = self;
    historyLengthPopUpButton.action = @selector(historyLengthChanged:);
    [self.view addSubview:historyLengthLabel];
    [self.view addSubview:historyLengthPopUpButton];
    [NSLayoutConstraint activateConstraints:@[
        [self.view.widthAnchor constraintEqualToConstant:700],
        [historyLengthLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [historyLengthLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [historyLengthPopUpButton.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [historyLengthPopUpButton.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                          constant:-20],
        [historyLengthPopUpButton.widthAnchor constraintGreaterThanOrEqualToConstant:100],
        [historyLengthLabel.firstBaselineAnchor constraintEqualToAnchor:historyLengthPopUpButton.firstBaselineAnchor],
        [historyLengthPopUpButton.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [historyLengthPopUpButton.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
    ]];
}

- (void)historyLengthChanged:(NSPopUpButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::history::Options options{ configuration.history_options() };
    options.set_length(static_cast<parabolic::history::Length>(sender.selectedItem.tag));
    configuration.set_history_options(options);
}

@end

@interface DownloadsViewController ()

@property(nonatomic, strong) NSTextField* maxActiveDownloadsTextField;
@property(nonatomic, strong) NSStepper* maxActiveDownloadsStepper;

@end

@implementation DownloadsViewController

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
    parabolic::download::Options downloadOptions{ configuration.download_options() };
    self.title = @(translator._p("Settings", "Downloads"));
    NSTextField* maxActiveDownloadsLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Maximum Active Downloads")).c_str())],
    };
    maxActiveDownloadsLabel.translatesAutoresizingMaskIntoConstraints = NO;
    maxActiveDownloadsLabel.alignment = NSTextAlignmentRight;
    NSNumberFormatter* maxActiveDownloadsFormatter{ [[NSNumberFormatter alloc] init] };
    maxActiveDownloadsFormatter.numberStyle = NSNumberFormatterNoStyle;
    maxActiveDownloadsFormatter.minimum = @1;
    maxActiveDownloadsFormatter.maximum = @50;
    NSTextField* maxActiveDownloadsTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    maxActiveDownloadsTextField.translatesAutoresizingMaskIntoConstraints = NO;
    maxActiveDownloadsTextField.formatter = maxActiveDownloadsFormatter;
    maxActiveDownloadsTextField.integerValue = downloadOptions.max_number_of_active_downloads();
    maxActiveDownloadsTextField.target = self;
    maxActiveDownloadsTextField.action = @selector(maxActiveDownloadsTextFieldChanged:);
    self.maxActiveDownloadsTextField = maxActiveDownloadsTextField;
    NSStepper* maxActiveDownloadsStepper{ [[NSStepper alloc] initWithFrame:NSZeroRect] };
    maxActiveDownloadsStepper.translatesAutoresizingMaskIntoConstraints = NO;
    maxActiveDownloadsStepper.minValue = 1;
    maxActiveDownloadsStepper.maxValue = 50;
    maxActiveDownloadsStepper.increment = 1;
    maxActiveDownloadsStepper.integerValue = downloadOptions.max_number_of_active_downloads();
    maxActiveDownloadsStepper.target = self;
    maxActiveDownloadsStepper.action = @selector(maxActiveDownloadsStepperChanged:);
    self.maxActiveDownloadsStepper = maxActiveDownloadsStepper;
    NSButton* overwriteExistingFilesCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Overwrite Existing Files"))
                             target:self
                             action:@selector(overwriteExistingFilesChanged:)]
    };
    overwriteExistingFilesCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    overwriteExistingFilesCheckbox.state =
            downloadOptions.overwrite_existing_files() ? NSControlStateValueOn : NSControlStateValueOff;
    NSButton* preventSuspendCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Prevent Sleep During Downloads"))
                             target:self
                             action:@selector(preventSuspendChanged:)]
    };
    preventSuspendCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    preventSuspendCheckbox.state = downloadOptions.prevent_suspend() ? NSControlStateValueOn : NSControlStateValueOff;
    NSTextField* ytdlpArgumentsLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Extra yt-dlp Arguments")).c_str())],
    };
    ytdlpArgumentsLabel.translatesAutoresizingMaskIntoConstraints = NO;
    ytdlpArgumentsLabel.alignment = NSTextAlignmentRight;
    NSTextField* ytdlpArgumentsTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    ytdlpArgumentsTextField.translatesAutoresizingMaskIntoConstraints = NO;
    ytdlpArgumentsTextField.stringValue = @(downloadOptions.ytdlp_arguments().c_str());
    ytdlpArgumentsTextField.target = self;
    ytdlpArgumentsTextField.action = @selector(ytdlpArgumentsChanged:);
    [self.view addSubview:maxActiveDownloadsLabel];
    [self.view addSubview:maxActiveDownloadsTextField];
    [self.view addSubview:maxActiveDownloadsStepper];
    [self.view addSubview:overwriteExistingFilesCheckbox];
    [self.view addSubview:preventSuspendCheckbox];
    [self.view addSubview:ytdlpArgumentsLabel];
    [self.view addSubview:ytdlpArgumentsTextField];
    [NSLayoutConstraint activateConstraints:@[
        [self.view.widthAnchor constraintEqualToConstant:700],
        [maxActiveDownloadsLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [maxActiveDownloadsLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor
                                                                           constant:20],
        [maxActiveDownloadsTextField.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [maxActiveDownloadsTextField.widthAnchor constraintEqualToConstant:40],
        [maxActiveDownloadsLabel.firstBaselineAnchor
                constraintEqualToAnchor:maxActiveDownloadsTextField.firstBaselineAnchor],
        [maxActiveDownloadsStepper.leadingAnchor constraintEqualToAnchor:maxActiveDownloadsTextField.trailingAnchor
                                                                constant:4],
        [maxActiveDownloadsStepper.centerYAnchor constraintEqualToAnchor:maxActiveDownloadsTextField.centerYAnchor],
        [maxActiveDownloadsTextField.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [overwriteExistingFilesCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [overwriteExistingFilesCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                constant:-20],
        [overwriteExistingFilesCheckbox.topAnchor constraintEqualToAnchor:maxActiveDownloadsTextField.bottomAnchor
                                                                 constant:16],
        [preventSuspendCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [preventSuspendCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [preventSuspendCheckbox.topAnchor constraintEqualToAnchor:overwriteExistingFilesCheckbox.bottomAnchor
                                                         constant:8],
        [ytdlpArgumentsLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [ytdlpArgumentsLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [ytdlpArgumentsTextField.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [ytdlpArgumentsTextField.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [ytdlpArgumentsLabel.firstBaselineAnchor constraintEqualToAnchor:ytdlpArgumentsTextField.firstBaselineAnchor],
        [ytdlpArgumentsTextField.topAnchor constraintEqualToAnchor:preventSuspendCheckbox.bottomAnchor constant:16],
        [ytdlpArgumentsTextField.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
    ]];
}

- (void)maxActiveDownloadsTextFieldChanged:(NSTextField*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::download::Options options{ configuration.download_options() };
    options.set_max_number_of_active_downloads(static_cast<int>(sender.integerValue));
    configuration.set_download_options(options);
    self.maxActiveDownloadsStepper.integerValue = sender.integerValue;
}

- (void)maxActiveDownloadsStepperChanged:(NSStepper*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::download::Options options{ configuration.download_options() };
    options.set_max_number_of_active_downloads(static_cast<int>(sender.integerValue));
    configuration.set_download_options(options);
    self.maxActiveDownloadsTextField.integerValue = sender.integerValue;
}

- (void)overwriteExistingFilesChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::download::Options options{ configuration.download_options() };
    options.set_overwrite_existing_files(sender.state == NSControlStateValueOn);
    configuration.set_download_options(options);
}

- (void)preventSuspendChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::download::Options options{ configuration.download_options() };
    options.set_prevent_suspend(sender.state == NSControlStateValueOn);
    configuration.set_download_options(options);
}

- (void)ytdlpArgumentsChanged:(NSTextField*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::download::Options options{ configuration.download_options() };
    options.set_ytdlp_arguments(sender.stringValue.UTF8String);
    configuration.set_download_options(options);
}

@end

@implementation DiscoveryViewController

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 700, 150)];
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
    self.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 700, 150)];
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
    self.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 700, 150)];
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
