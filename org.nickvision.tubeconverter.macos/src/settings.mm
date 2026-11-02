#import "settings.h"
#include "application.h"
#include "configuration.h"
#include "cookies.h"
#include "download.h"
#include "extractor.h"
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
    NSTextField* languageSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("An application restart is required for a change to take "
                                                            "effect."))]
    };
    languageSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    languageSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    languageSubtitleLabel.textColor = NSColor.secondaryLabelColor;
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
    NSTextField* allowPreviewUpdatesSubtitleLabel
    {
        [NSTextField
                wrappingLabelWithString:@(translator._(
                                                "Update the app and dependencies, such as yt-dlp, to beta versions."))]
    };
    allowPreviewUpdatesSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    allowPreviewUpdatesSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    allowPreviewUpdatesSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    [self.view addSubview:themeLabel];
    [self.view addSubview:themePopUpButton];
    [self.view addSubview:languageLabel];
    [self.view addSubview:languagePopUpButton];
    [self.view addSubview:languageSubtitleLabel];
    [self.view addSubview:autoUpdateCheckbox];
    [self.view addSubview:allowPreviewUpdatesCheckbox];
    [self.view addSubview:allowPreviewUpdatesSubtitleLabel];
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
        [languageSubtitleLabel.leadingAnchor constraintEqualToAnchor:languagePopUpButton.leadingAnchor],
        [languageSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [languageSubtitleLabel.topAnchor constraintEqualToAnchor:languagePopUpButton.bottomAnchor constant:6],
        [autoUpdateCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [autoUpdateCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [autoUpdateCheckbox.topAnchor constraintEqualToAnchor:languageSubtitleLabel.bottomAnchor constant:16],
        [allowPreviewUpdatesCheckbox.leadingAnchor constraintEqualToAnchor:autoUpdateCheckbox.leadingAnchor],
        [allowPreviewUpdatesCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                             constant:-20],
        [allowPreviewUpdatesCheckbox.topAnchor constraintEqualToAnchor:autoUpdateCheckbox.bottomAnchor constant:8],
        [allowPreviewUpdatesSubtitleLabel.leadingAnchor
                constraintEqualToAnchor:allowPreviewUpdatesCheckbox.leadingAnchor],
        [allowPreviewUpdatesSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                  constant:-20],
        [allowPreviewUpdatesSubtitleLabel.topAnchor constraintEqualToAnchor:allowPreviewUpdatesCheckbox.bottomAnchor
                                                                   constant:6],
        [allowPreviewUpdatesSubtitleLabel.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
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
        [NSTextField labelWithString:@(translator._("{}:", translator._("Keep Download History For")).c_str())],
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
        [NSTextField labelWithString:@(translator._("{}:", translator._("Extra yt-dlp Download Arguments")).c_str())],
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
    self.view = [[NSView alloc] initWithFrame:NSZeroRect];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.translatesAutoresizingMaskIntoConstraints = NO;
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options discoveryOptions{ configuration.discovery_options() };
    self.title = @(translator._p("Settings", "Discovery"));
    NSButton* includeAutoGeneratedSubtitlesCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Show Auto-Generated Subtitles"))
                             target:self
                             action:@selector(includeAutoGeneratedSubtitlesChanged:)]
    };
    includeAutoGeneratedSubtitlesCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    includeAutoGeneratedSubtitlesCheckbox.state =
            discoveryOptions.include_auto_generated_subtitles() ? NSControlStateValueOn : NSControlStateValueOff;
    NSButton* includeMediaIdInTitleCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Include Media Id in Title"))
                             target:self
                             action:@selector(includeMediaIdInTitleChanged:)]
    };
    includeMediaIdInTitleCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    includeMediaIdInTitleCheckbox.state =
            discoveryOptions.include_media_id_in_title() ? NSControlStateValueOn : NSControlStateValueOff;
    NSTextField* includeMediaIdInTitleSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Add the media entry's yt-dlp id to its default title."))]
    };
    includeMediaIdInTitleSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    includeMediaIdInTitleSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    includeMediaIdInTitleSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* includeSuperResolutionsCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Show Super Resolution Formats"))
                             target:self
                             action:@selector(includeSuperResolutionsChanged:)]
    };
    includeSuperResolutionsCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    includeSuperResolutionsCheckbox.state =
            discoveryOptions.include_super_resolutions() ? NSControlStateValueOn : NSControlStateValueOff;
    NSTextField* includeSuperResolutionsSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Super resolutions are resolutions that have been enhanced "
                                                            "with artificial intelligence."))]
    };
    includeSuperResolutionsSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    includeSuperResolutionsSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    includeSuperResolutionsSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* limitCharactersCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Limit Filename Characters"))
                             target:self
                             action:@selector(limitCharactersChanged:)]
    };
    limitCharactersCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    limitCharactersCheckbox.state =
            discoveryOptions.limit_characters() ? NSControlStateValueOn : NSControlStateValueOff;
    NSTextField* limitCharactersSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Restrict filename characters to those supported only by "
                                                            "Windows."))]
    };
    limitCharactersSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    limitCharactersSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    limitCharactersSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSTextField* preferredAudioCodecLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Preferred Audio Codec")).c_str())],
    };
    preferredAudioCodecLabel.translatesAutoresizingMaskIntoConstraints = NO;
    preferredAudioCodecLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* preferredAudioCodecPopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    preferredAudioCodecPopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    [preferredAudioCodecPopUpButton addItemsWithTitles:@[
        @(translator._("Any")),
        @(translator._("FLAC")),
        @(translator._("WAV")),
        @(translator._("Opus")),
        @(translator._("AAC")),
        @(translator._("MP4A")),
        @(translator._("MP3")),
    ]];
    for (NSInteger i{ 0 }; i < preferredAudioCodecPopUpButton.itemArray.count; i++)
    {
        [[preferredAudioCodecPopUpButton itemAtIndex:i] setTag:i];
    }
    [preferredAudioCodecPopUpButton selectItemWithTag:static_cast<NSInteger>(discoveryOptions.preferred_audio_codec())];
    preferredAudioCodecPopUpButton.target = self;
    preferredAudioCodecPopUpButton.action = @selector(preferredAudioCodecChanged:);
    NSTextField* preferredAudioCodecSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Prefer this codec when parsing audio formats to show "
                                                            "for download."))]
    };
    preferredAudioCodecSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    preferredAudioCodecSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    preferredAudioCodecSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSTextField* preferredVideoCodecLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Preferred Video Codec")).c_str())],
    };
    preferredVideoCodecLabel.translatesAutoresizingMaskIntoConstraints = NO;
    preferredVideoCodecLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* preferredVideoCodecPopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    preferredVideoCodecPopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    [preferredVideoCodecPopUpButton addItemsWithTitles:@[
        @(translator._("Any")),
        @(translator._("H.265")),
        @(translator._("VP9")),
        @(translator._("AV1")),
        @(translator._("H.264")),
    ]];
    for (NSInteger i{ 0 }; i < preferredVideoCodecPopUpButton.itemArray.count; i++)
    {
        [[preferredVideoCodecPopUpButton itemAtIndex:i] setTag:i];
    }
    [preferredVideoCodecPopUpButton selectItemWithTag:static_cast<NSInteger>(discoveryOptions.preferred_video_codec())];
    preferredVideoCodecPopUpButton.target = self;
    preferredVideoCodecPopUpButton.action = @selector(preferredVideoCodecChanged:);
    NSTextField* preferredVideoCodecSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Prefer this codec when parsing video formats to show "
                                                            "for download."))]
    };
    preferredVideoCodecSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    preferredVideoCodecSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    preferredVideoCodecSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSTextField* preferredSubtitleFormatLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Preferred Subtitle Format")).c_str())],
    };
    preferredSubtitleFormatLabel.translatesAutoresizingMaskIntoConstraints = NO;
    preferredSubtitleFormatLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* preferredSubtitleFormatPopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    preferredSubtitleFormatPopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    [preferredSubtitleFormatPopUpButton addItemsWithTitles:@[
        @(translator._("Any")),
        @(translator._("VTT")),
        @(translator._("SRT")),
        @(translator._("ASS")),
        @(translator._("LRC")),
    ]];
    for (NSInteger i{ 0 }; i < preferredSubtitleFormatPopUpButton.itemArray.count; i++)
    {
        [[preferredSubtitleFormatPopUpButton itemAtIndex:i] setTag:i];
    }
    [preferredSubtitleFormatPopUpButton
            selectItemWithTag:static_cast<NSInteger>(discoveryOptions.preferred_subtitle_format())];
    preferredSubtitleFormatPopUpButton.target = self;
    preferredSubtitleFormatPopUpButton.action = @selector(preferredSubtitleFormatChanged:);
    NSTextField* preferredSubtitleFormatSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Prefer this subtitle file format for download."))]
    };
    preferredSubtitleFormatSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    preferredSubtitleFormatSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    preferredSubtitleFormatSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSTextField* preferredFrameRateLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Preferred Frame Rate")).c_str())],
    };
    preferredFrameRateLabel.translatesAutoresizingMaskIntoConstraints = NO;
    preferredFrameRateLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* preferredFrameRatePopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    preferredFrameRatePopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    [preferredFrameRatePopUpButton addItemsWithTitles:@[
        @(translator._("Any")),
        @(translator._("24 FPS")),
        @(translator._("30 FPS")),
        @(translator._("60 FPS")),
    ]];
    for (NSInteger i{ 0 }; i < preferredFrameRatePopUpButton.itemArray.count; i++)
    {
        [[preferredFrameRatePopUpButton itemAtIndex:i] setTag:i];
    }
    [preferredFrameRatePopUpButton selectItemWithTag:static_cast<NSInteger>(discoveryOptions.preferred_frame_rate())];
    preferredFrameRatePopUpButton.target = self;
    preferredFrameRatePopUpButton.action = @selector(preferredFrameRateChanged:);
    NSTextField* preferredFrameRateSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Prefer this frame rate when parsing video formats to show "
                                                            "for download."))]
    };
    preferredFrameRateSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    preferredFrameRateSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    preferredFrameRateSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSTextField* ytdlpArgumentsLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Extra yt-dlp Discovery Arguments")).c_str())],
    };
    ytdlpArgumentsLabel.translatesAutoresizingMaskIntoConstraints = NO;
    ytdlpArgumentsLabel.alignment = NSTextAlignmentRight;
    NSTextField* ytdlpArgumentsTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    ytdlpArgumentsTextField.translatesAutoresizingMaskIntoConstraints = NO;
    ytdlpArgumentsTextField.stringValue = @(discoveryOptions.ytdlp_arguments().c_str());
    ytdlpArgumentsTextField.target = self;
    ytdlpArgumentsTextField.action = @selector(ytdlpArgumentsChanged:);
    [self.view addSubview:includeAutoGeneratedSubtitlesCheckbox];
    [self.view addSubview:includeMediaIdInTitleCheckbox];
    [self.view addSubview:includeMediaIdInTitleSubtitleLabel];
    [self.view addSubview:includeSuperResolutionsCheckbox];
    [self.view addSubview:includeSuperResolutionsSubtitleLabel];
    [self.view addSubview:limitCharactersCheckbox];
    [self.view addSubview:limitCharactersSubtitleLabel];
    [self.view addSubview:preferredAudioCodecLabel];
    [self.view addSubview:preferredAudioCodecPopUpButton];
    [self.view addSubview:preferredAudioCodecSubtitleLabel];
    [self.view addSubview:preferredVideoCodecLabel];
    [self.view addSubview:preferredVideoCodecPopUpButton];
    [self.view addSubview:preferredVideoCodecSubtitleLabel];
    [self.view addSubview:preferredSubtitleFormatLabel];
    [self.view addSubview:preferredSubtitleFormatPopUpButton];
    [self.view addSubview:preferredSubtitleFormatSubtitleLabel];
    [self.view addSubview:preferredFrameRateLabel];
    [self.view addSubview:preferredFrameRatePopUpButton];
    [self.view addSubview:preferredFrameRateSubtitleLabel];
    [self.view addSubview:ytdlpArgumentsLabel];
    [self.view addSubview:ytdlpArgumentsTextField];
    [NSLayoutConstraint activateConstraints:@[
        [self.view.widthAnchor constraintEqualToConstant:700],
        [includeAutoGeneratedSubtitlesCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor
                                                                            constant:4],
        [includeAutoGeneratedSubtitlesCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                       constant:-20],
        [includeAutoGeneratedSubtitlesCheckbox.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [includeMediaIdInTitleCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [includeMediaIdInTitleCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                               constant:-20],
        [includeMediaIdInTitleCheckbox.topAnchor
                constraintEqualToAnchor:includeAutoGeneratedSubtitlesCheckbox.bottomAnchor
                               constant:8],
        [includeMediaIdInTitleSubtitleLabel.leadingAnchor
                constraintEqualToAnchor:includeMediaIdInTitleCheckbox.leadingAnchor],
        [includeMediaIdInTitleSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                    constant:-20],
        [includeMediaIdInTitleSubtitleLabel.topAnchor constraintEqualToAnchor:includeMediaIdInTitleCheckbox.bottomAnchor
                                                                     constant:6],
        [includeSuperResolutionsCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [includeSuperResolutionsCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                 constant:-20],
        [includeSuperResolutionsCheckbox.topAnchor
                constraintEqualToAnchor:includeMediaIdInTitleSubtitleLabel.bottomAnchor
                               constant:8],
        [includeSuperResolutionsSubtitleLabel.leadingAnchor
                constraintEqualToAnchor:includeSuperResolutionsCheckbox.leadingAnchor],
        [includeSuperResolutionsSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                      constant:-20],
        [includeSuperResolutionsSubtitleLabel.topAnchor
                constraintEqualToAnchor:includeSuperResolutionsCheckbox.bottomAnchor
                               constant:6],
        [limitCharactersCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [limitCharactersCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                         constant:-20],
        [limitCharactersCheckbox.topAnchor constraintEqualToAnchor:includeSuperResolutionsSubtitleLabel.bottomAnchor
                                                          constant:8],
        [limitCharactersSubtitleLabel.leadingAnchor constraintEqualToAnchor:limitCharactersCheckbox.leadingAnchor],
        [limitCharactersSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                              constant:-20],
        [limitCharactersSubtitleLabel.topAnchor constraintEqualToAnchor:limitCharactersCheckbox.bottomAnchor
                                                               constant:6],
        [preferredAudioCodecLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [preferredAudioCodecLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor
                                                                            constant:20],
        [preferredAudioCodecPopUpButton.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [preferredAudioCodecPopUpButton.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                constant:-20],
        [preferredAudioCodecPopUpButton.widthAnchor constraintGreaterThanOrEqualToConstant:100],
        [preferredAudioCodecLabel.firstBaselineAnchor
                constraintEqualToAnchor:preferredAudioCodecPopUpButton.firstBaselineAnchor],
        [preferredAudioCodecPopUpButton.topAnchor constraintEqualToAnchor:limitCharactersSubtitleLabel.bottomAnchor
                                                                 constant:16],
        [preferredAudioCodecSubtitleLabel.leadingAnchor
                constraintEqualToAnchor:preferredAudioCodecPopUpButton.leadingAnchor],
        [preferredAudioCodecSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                  constant:-20],
        [preferredAudioCodecSubtitleLabel.topAnchor constraintEqualToAnchor:preferredAudioCodecPopUpButton.bottomAnchor
                                                                   constant:6],
        [preferredVideoCodecLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [preferredVideoCodecLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor
                                                                            constant:20],
        [preferredVideoCodecPopUpButton.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [preferredVideoCodecPopUpButton.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                constant:-20],
        [preferredVideoCodecPopUpButton.widthAnchor constraintGreaterThanOrEqualToConstant:100],
        [preferredVideoCodecLabel.firstBaselineAnchor
                constraintEqualToAnchor:preferredVideoCodecPopUpButton.firstBaselineAnchor],
        [preferredVideoCodecPopUpButton.topAnchor constraintEqualToAnchor:preferredAudioCodecSubtitleLabel.bottomAnchor
                                                                 constant:12],
        [preferredVideoCodecSubtitleLabel.leadingAnchor
                constraintEqualToAnchor:preferredVideoCodecPopUpButton.leadingAnchor],
        [preferredVideoCodecSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                  constant:-20],
        [preferredVideoCodecSubtitleLabel.topAnchor constraintEqualToAnchor:preferredVideoCodecPopUpButton.bottomAnchor
                                                                   constant:6],
        [preferredSubtitleFormatLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [preferredSubtitleFormatLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor
                                                                                constant:20],
        [preferredSubtitleFormatPopUpButton.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [preferredSubtitleFormatPopUpButton.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                    constant:-20],
        [preferredSubtitleFormatPopUpButton.widthAnchor constraintGreaterThanOrEqualToConstant:100],
        [preferredSubtitleFormatLabel.firstBaselineAnchor
                constraintEqualToAnchor:preferredSubtitleFormatPopUpButton.firstBaselineAnchor],
        [preferredSubtitleFormatPopUpButton.topAnchor
                constraintEqualToAnchor:preferredVideoCodecSubtitleLabel.bottomAnchor
                               constant:12],
        [preferredSubtitleFormatSubtitleLabel.leadingAnchor
                constraintEqualToAnchor:preferredSubtitleFormatPopUpButton.leadingAnchor],
        [preferredSubtitleFormatSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                      constant:-20],
        [preferredSubtitleFormatSubtitleLabel.topAnchor
                constraintEqualToAnchor:preferredSubtitleFormatPopUpButton.bottomAnchor
                               constant:6],
        [preferredFrameRateLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [preferredFrameRateLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor
                                                                           constant:20],
        [preferredFrameRatePopUpButton.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [preferredFrameRatePopUpButton.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                               constant:-20],
        [preferredFrameRatePopUpButton.widthAnchor constraintGreaterThanOrEqualToConstant:100],
        [preferredFrameRateLabel.firstBaselineAnchor
                constraintEqualToAnchor:preferredFrameRatePopUpButton.firstBaselineAnchor],
        [preferredFrameRatePopUpButton.topAnchor
                constraintEqualToAnchor:preferredSubtitleFormatSubtitleLabel.bottomAnchor
                               constant:12],
        [preferredFrameRateSubtitleLabel.leadingAnchor
                constraintEqualToAnchor:preferredFrameRatePopUpButton.leadingAnchor],
        [preferredFrameRateSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                 constant:-20],
        [preferredFrameRateSubtitleLabel.topAnchor constraintEqualToAnchor:preferredFrameRatePopUpButton.bottomAnchor
                                                                  constant:6],
        [ytdlpArgumentsLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [ytdlpArgumentsLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [ytdlpArgumentsTextField.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [ytdlpArgumentsTextField.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [ytdlpArgumentsLabel.firstBaselineAnchor constraintEqualToAnchor:ytdlpArgumentsTextField.firstBaselineAnchor],
        [ytdlpArgumentsTextField.topAnchor constraintEqualToAnchor:preferredFrameRateSubtitleLabel.bottomAnchor
                                                          constant:16],
        [ytdlpArgumentsTextField.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
    ]];
}

- (void)includeAutoGeneratedSubtitlesChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_include_auto_generated_subtitles(sender.state == NSControlStateValueOn);
    configuration.set_discovery_options(options);
}

- (void)includeMediaIdInTitleChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_include_media_id_in_title(sender.state == NSControlStateValueOn);
    configuration.set_discovery_options(options);
}

- (void)includeSuperResolutionsChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_include_super_resolutions(sender.state == NSControlStateValueOn);
    configuration.set_discovery_options(options);
}

- (void)limitCharactersChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_limit_characters(sender.state == NSControlStateValueOn);
    configuration.set_discovery_options(options);
}

- (void)preferredAudioCodecChanged:(NSPopUpButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_preferred_audio_codec(static_cast<parabolic::audio::Codec>(sender.selectedItem.tag));
    configuration.set_discovery_options(options);
}

- (void)preferredVideoCodecChanged:(NSPopUpButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_preferred_video_codec(static_cast<parabolic::video::Codec>(sender.selectedItem.tag));
    configuration.set_discovery_options(options);
}

- (void)preferredSubtitleFormatChanged:(NSPopUpButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_preferred_subtitle_format(static_cast<parabolic::subtitle::Type>(sender.selectedItem.tag));
    configuration.set_discovery_options(options);
}

- (void)preferredFrameRateChanged:(NSPopUpButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_preferred_frame_rate(static_cast<parabolic::video::FrameRate>(sender.selectedItem.tag));
    configuration.set_discovery_options(options);
}

- (void)ytdlpArgumentsChanged:(NSTextField*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_ytdlp_arguments(sender.stringValue.UTF8String);
    configuration.set_discovery_options(options);
}

@end

@interface ExtractorViewController ()

@property(nonatomic, strong) NSTextField* speedLimitTextField;
@property(nonatomic, strong) NSStepper* speedLimitStepper;
@property(nonatomic, strong) NSTextField* cookiesFileTextField;

@end

@implementation ExtractorViewController

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
    parabolic::extractor::Options extractorOptions{ configuration.extractor_options() };
    self.title = @(translator._p("Settings", "Extractor"));
    NSButton* usePartFilesCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Use Part Files"))
                             target:self
                             action:@selector(usePartFilesChanged:)]
    };
    usePartFilesCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    usePartFilesCheckbox.state = extractorOptions.use_part_files() ? NSControlStateValueOn : NSControlStateValueOff;
    NSTextField* usePartFilesSubtitleLabel
    {
        [NSTextField
                wrappingLabelWithString:@(translator._("Download media in separate .part files instead of directly "
                                                       "into the output file."))]
    };
    usePartFilesSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    usePartFilesSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    usePartFilesSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* youtubeSponsorBlockCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Use SponsorBlock for YouTube"))
                             target:self
                             action:@selector(youtubeSponsorBlockChanged:)]
    };
    youtubeSponsorBlockCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    youtubeSponsorBlockCheckbox.state =
            extractorOptions.youtube_sponsor_block() ? NSControlStateValueOn : NSControlStateValueOff;
    NSTextField* youtubeSponsorBlockSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Try to remove sponsored segments from videos."))]
    };
    youtubeSponsorBlockSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    youtubeSponsorBlockSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    youtubeSponsorBlockSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* translateMetadataAndChaptersCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Translate Metadata and Chapters"))
                             target:self
                             action:@selector(translateMetadataAndChaptersChanged:)]
    };
    translateMetadataAndChaptersCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    translateMetadataAndChaptersCheckbox.state =
            extractorOptions.translate_metadata_and_chapters() ? NSControlStateValueOn : NSControlStateValueOff;
    NSTextField* translateMetadataAndChaptersSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Automatically translate embedded metadata and chapters "
                                                            "using the app's set language on supported sites and "
                                                            "media."))]
    };
    translateMetadataAndChaptersSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    translateMetadataAndChaptersSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    translateMetadataAndChaptersSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* limitSpeedCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Limit Download Speed"))
                             target:self
                             action:@selector(limitSpeedChanged:)]
    };
    limitSpeedCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    limitSpeedCheckbox.state =
            extractorOptions.speed_limit().has_value() ? NSControlStateValueOn : NSControlStateValueOff;
    NSTextField* speedLimitLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Speed Limit (KiB/s)")).c_str())],
    };
    speedLimitLabel.translatesAutoresizingMaskIntoConstraints = NO;
    speedLimitLabel.alignment = NSTextAlignmentRight;
    NSInteger speedLimitValue{ extractorOptions.speed_limit().value_or(1024) };
    NSNumberFormatter* speedLimitFormatter{ [[NSNumberFormatter alloc] init] };
    speedLimitFormatter.numberStyle = NSNumberFormatterNoStyle;
    speedLimitFormatter.minimum = @1;
    speedLimitFormatter.maximum = @1048576;
    NSTextField* speedLimitTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    speedLimitTextField.translatesAutoresizingMaskIntoConstraints = NO;
    speedLimitTextField.formatter = speedLimitFormatter;
    speedLimitTextField.integerValue = speedLimitValue;
    speedLimitTextField.enabled = extractorOptions.speed_limit().has_value();
    speedLimitTextField.target = self;
    speedLimitTextField.action = @selector(speedLimitTextFieldChanged:);
    self.speedLimitTextField = speedLimitTextField;
    NSStepper* speedLimitStepper{ [[NSStepper alloc] initWithFrame:NSZeroRect] };
    speedLimitStepper.translatesAutoresizingMaskIntoConstraints = NO;
    speedLimitStepper.minValue = 1;
    speedLimitStepper.maxValue = 1048576;
    speedLimitStepper.increment = 128;
    speedLimitStepper.integerValue = speedLimitValue;
    speedLimitStepper.enabled = extractorOptions.speed_limit().has_value();
    speedLimitStepper.target = self;
    speedLimitStepper.action = @selector(speedLimitStepperChanged:);
    self.speedLimitStepper = speedLimitStepper;
    NSTextField* proxyUrlLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Proxy URL")).c_str())],
    };
    proxyUrlLabel.translatesAutoresizingMaskIntoConstraints = NO;
    proxyUrlLabel.alignment = NSTextAlignmentRight;
    NSTextField* proxyUrlTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    proxyUrlTextField.translatesAutoresizingMaskIntoConstraints = NO;
    proxyUrlTextField.stringValue = @(extractorOptions.proxy_url().c_str());
    proxyUrlTextField.target = self;
    proxyUrlTextField.action = @selector(proxyUrlChanged:);
    NSTextField* cookiesBrowserLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Cookies Browser")).c_str())],
    };
    cookiesBrowserLabel.translatesAutoresizingMaskIntoConstraints = NO;
    cookiesBrowserLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* cookiesBrowserPopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    cookiesBrowserPopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    [cookiesBrowserPopUpButton addItemsWithTitles:@[
        @(translator._("None")),
        @(translator._("Brave")),
        @(translator._("Chrome")),
        @(translator._("Chromium")),
        @(translator._("Edge")),
        @(translator._("Firefox")),
        @(translator._("Opera")),
        @(translator._("Vivaldi")),
        @(translator._("Whale")),
    ]];
    for (NSInteger i{ 0 }; i < cookiesBrowserPopUpButton.itemArray.count; i++)
    {
        [[cookiesBrowserPopUpButton itemAtIndex:i] setTag:i];
    }
    [cookiesBrowserPopUpButton selectItemWithTag:static_cast<NSInteger>(extractorOptions.cookies_browser())];
    cookiesBrowserPopUpButton.target = self;
    cookiesBrowserPopUpButton.action = @selector(cookiesBrowserChanged:);
    NSTextField* cookiesFileLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Cookies File")).c_str())],
    };
    cookiesFileLabel.translatesAutoresizingMaskIntoConstraints = NO;
    cookiesFileLabel.alignment = NSTextAlignmentRight;
    NSTextField* cookiesFileTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    cookiesFileTextField.translatesAutoresizingMaskIntoConstraints = NO;
    cookiesFileTextField.stringValue = @(extractorOptions.cookies_path().c_str());
    cookiesFileTextField.target = self;
    cookiesFileTextField.action = @selector(cookiesFileTextFieldChanged:);
    self.cookiesFileTextField = cookiesFileTextField;
    NSButton* browseCookiesFileButton
    {
        [NSButton buttonWithTitle:@(translator._("Browse…")) target:self action:@selector(browseCookiesFile:)]
    };
    browseCookiesFileButton.translatesAutoresizingMaskIntoConstraints = NO;
    NSButton* clearCookiesFileButton
    {
        [NSButton buttonWithTitle:@(translator._("Clear")) target:self action:@selector(clearCookiesFile:)]
    };
    clearCookiesFileButton.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:usePartFilesCheckbox];
    [self.view addSubview:usePartFilesSubtitleLabel];
    [self.view addSubview:youtubeSponsorBlockCheckbox];
    [self.view addSubview:youtubeSponsorBlockSubtitleLabel];
    [self.view addSubview:translateMetadataAndChaptersCheckbox];
    [self.view addSubview:translateMetadataAndChaptersSubtitleLabel];
    [self.view addSubview:limitSpeedCheckbox];
    [self.view addSubview:speedLimitLabel];
    [self.view addSubview:speedLimitTextField];
    [self.view addSubview:speedLimitStepper];
    [self.view addSubview:proxyUrlLabel];
    [self.view addSubview:proxyUrlTextField];
    [self.view addSubview:cookiesBrowserLabel];
    [self.view addSubview:cookiesBrowserPopUpButton];
    [self.view addSubview:cookiesFileLabel];
    [self.view addSubview:cookiesFileTextField];
    [self.view addSubview:browseCookiesFileButton];
    [self.view addSubview:clearCookiesFileButton];
    [NSLayoutConstraint activateConstraints:@[
        [self.view.widthAnchor constraintEqualToConstant:700],
        [usePartFilesCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [usePartFilesCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [usePartFilesCheckbox.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [usePartFilesSubtitleLabel.leadingAnchor constraintEqualToAnchor:usePartFilesCheckbox.leadingAnchor],
        [usePartFilesSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                           constant:-20],
        [usePartFilesSubtitleLabel.topAnchor constraintEqualToAnchor:usePartFilesCheckbox.bottomAnchor constant:6],
        [youtubeSponsorBlockCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [youtubeSponsorBlockCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                             constant:-20],
        [youtubeSponsorBlockCheckbox.topAnchor constraintEqualToAnchor:usePartFilesSubtitleLabel.bottomAnchor
                                                              constant:8],
        [youtubeSponsorBlockSubtitleLabel.leadingAnchor
                constraintEqualToAnchor:youtubeSponsorBlockCheckbox.leadingAnchor],
        [youtubeSponsorBlockSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                  constant:-20],
        [youtubeSponsorBlockSubtitleLabel.topAnchor constraintEqualToAnchor:youtubeSponsorBlockCheckbox.bottomAnchor
                                                                   constant:6],
        [translateMetadataAndChaptersCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [translateMetadataAndChaptersCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                      constant:-20],
        [translateMetadataAndChaptersCheckbox.topAnchor
                constraintEqualToAnchor:youtubeSponsorBlockSubtitleLabel.bottomAnchor
                               constant:8],
        [translateMetadataAndChaptersSubtitleLabel.leadingAnchor
                constraintEqualToAnchor:translateMetadataAndChaptersCheckbox.leadingAnchor],
        [translateMetadataAndChaptersSubtitleLabel.trailingAnchor
                constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                         constant:-20],
        [translateMetadataAndChaptersSubtitleLabel.topAnchor
                constraintEqualToAnchor:translateMetadataAndChaptersCheckbox.bottomAnchor
                               constant:6],
        [limitSpeedCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [limitSpeedCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [limitSpeedCheckbox.topAnchor constraintEqualToAnchor:translateMetadataAndChaptersSubtitleLabel.bottomAnchor
                                                     constant:16],
        [speedLimitLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [speedLimitLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [speedLimitTextField.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [speedLimitTextField.widthAnchor constraintEqualToConstant:60],
        [speedLimitLabel.firstBaselineAnchor constraintEqualToAnchor:speedLimitTextField.firstBaselineAnchor],
        [speedLimitStepper.leadingAnchor constraintEqualToAnchor:speedLimitTextField.trailingAnchor constant:4],
        [speedLimitStepper.centerYAnchor constraintEqualToAnchor:speedLimitTextField.centerYAnchor],
        [speedLimitTextField.topAnchor constraintEqualToAnchor:limitSpeedCheckbox.bottomAnchor constant:8],
        [proxyUrlLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [proxyUrlLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [proxyUrlTextField.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [proxyUrlTextField.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [proxyUrlLabel.firstBaselineAnchor constraintEqualToAnchor:proxyUrlTextField.firstBaselineAnchor],
        [proxyUrlTextField.topAnchor constraintEqualToAnchor:speedLimitTextField.bottomAnchor constant:16],
        [cookiesBrowserLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [cookiesBrowserLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [cookiesBrowserPopUpButton.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [cookiesBrowserPopUpButton.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                           constant:-20],
        [cookiesBrowserPopUpButton.widthAnchor constraintGreaterThanOrEqualToConstant:100],
        [cookiesBrowserLabel.firstBaselineAnchor constraintEqualToAnchor:cookiesBrowserPopUpButton.firstBaselineAnchor],
        [cookiesBrowserPopUpButton.topAnchor constraintEqualToAnchor:proxyUrlTextField.bottomAnchor constant:12],
        [cookiesFileLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [cookiesFileLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [cookiesFileTextField.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [cookiesFileTextField.trailingAnchor constraintEqualToAnchor:browseCookiesFileButton.leadingAnchor constant:-4],
        [cookiesFileLabel.firstBaselineAnchor constraintEqualToAnchor:cookiesFileTextField.firstBaselineAnchor],
        [browseCookiesFileButton.trailingAnchor constraintEqualToAnchor:clearCookiesFileButton.leadingAnchor
                                                               constant:-4],
        [browseCookiesFileButton.centerYAnchor constraintEqualToAnchor:cookiesFileTextField.centerYAnchor],
        [clearCookiesFileButton.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [clearCookiesFileButton.centerYAnchor constraintEqualToAnchor:cookiesFileTextField.centerYAnchor],
        [cookiesFileTextField.topAnchor constraintEqualToAnchor:cookiesBrowserPopUpButton.bottomAnchor constant:12],
        [cookiesFileTextField.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
    ]];
}

- (void)usePartFilesChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_use_part_files(sender.state == NSControlStateValueOn);
    configuration.set_extractor_options(options);
}

- (void)youtubeSponsorBlockChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_youtube_sponsor_block(sender.state == NSControlStateValueOn);
    configuration.set_extractor_options(options);
}

- (void)translateMetadataAndChaptersChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_translate_metadata_and_chapters(sender.state == NSControlStateValueOn);
    configuration.set_extractor_options(options);
}

- (void)limitSpeedChanged:(NSButton*)sender
{
    BOOL enabled{ sender.state == NSControlStateValueOn };
    self.speedLimitTextField.enabled = enabled;
    self.speedLimitStepper.enabled = enabled;
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_speed_limit(enabled ? std::optional<int>{ static_cast<int>(self.speedLimitTextField.integerValue) }
                                    : std::nullopt);
    configuration.set_extractor_options(options);
}

- (void)speedLimitTextFieldChanged:(NSTextField*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_speed_limit(static_cast<int>(sender.integerValue));
    configuration.set_extractor_options(options);
    self.speedLimitStepper.integerValue = sender.integerValue;
}

- (void)speedLimitStepperChanged:(NSStepper*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_speed_limit(static_cast<int>(sender.integerValue));
    configuration.set_extractor_options(options);
    self.speedLimitTextField.integerValue = sender.integerValue;
}

- (void)proxyUrlChanged:(NSTextField*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_proxy_url(sender.stringValue.UTF8String);
    configuration.set_extractor_options(options);
}

- (void)cookiesBrowserChanged:(NSPopUpButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_cookies_browser(static_cast<parabolic::cookies::Browser>(sender.selectedItem.tag));
    configuration.set_extractor_options(options);
}

- (void)cookiesFileTextFieldChanged:(NSTextField*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_cookies_path(sender.stringValue.UTF8String);
    configuration.set_extractor_options(options);
}

- (void)browseCookiesFile:(NSButton*)sender
{
    NSOpenPanel* panel{ [NSOpenPanel openPanel] };
    panel.canChooseFiles = YES;
    panel.canChooseDirectories = NO;
    panel.allowsMultipleSelection = NO;
    if ([panel runModal] == NSModalResponseOK)
    {
        NSString* path{ panel.URL.path };
        self.cookiesFileTextField.stringValue = path;
        parabolic::configuration::Configuration& configuration{
            parabolic::application::State::instance().configuration()
        };
        parabolic::extractor::Options options{ configuration.extractor_options() };
        options.set_cookies_path(path.UTF8String);
        configuration.set_extractor_options(options);
    }
}

- (void)clearCookiesFile:(NSButton*)sender
{
    self.cookiesFileTextField.stringValue = @"";
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_cookies_path("");
    configuration.set_extractor_options(options);
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
