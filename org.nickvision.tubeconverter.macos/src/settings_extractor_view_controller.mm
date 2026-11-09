#import "settings_extractor_view_controller.h"
#include <optional>
#include "application.h"
#include "configuration.h"
#include "cookies.h"
#include "extractor.h"
#include "translation.h"

@interface SettingsExtractorViewController ()

@property(nonatomic, strong) NSTextField* speedLimitTextField;
@property(nonatomic, strong) NSStepper* speedLimitStepper;
@property(nonatomic, strong) NSTextField* cookiesFileTextField;

@end

@implementation SettingsExtractorViewController

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
                             action:@selector(usePartFilesChanged:)],
    };
    usePartFilesCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    usePartFilesCheckbox.state = extractorOptions.use_part_files() ? NSControlStateValueOn : NSControlStateValueOff;
    NSTextField* usePartFilesSubtitleLabel
    {
        [NSTextField
                wrappingLabelWithString:@(translator._("Download media in separate .part files instead of directly "
                                                       "into the output file."))],
    };
    usePartFilesSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    usePartFilesSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    usePartFilesSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* youtubeSponsorBlockCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Use SponsorBlock for YouTube"))
                             target:self
                             action:@selector(youtubeSponsorBlockChanged:)],
    };
    youtubeSponsorBlockCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    youtubeSponsorBlockCheckbox.state =
            extractorOptions.youtube_sponsor_block() ? NSControlStateValueOn : NSControlStateValueOff;
    NSTextField* youtubeSponsorBlockSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Try to remove sponsored segments from videos."))],
    };
    youtubeSponsorBlockSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    youtubeSponsorBlockSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    youtubeSponsorBlockSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* translateMetadataAndChaptersCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Translate Metadata and Chapters"))
                             target:self
                             action:@selector(translateMetadataAndChaptersChanged:)],
    };
    translateMetadataAndChaptersCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    translateMetadataAndChaptersCheckbox.state =
            extractorOptions.translate_metadata_and_chapters() ? NSControlStateValueOn : NSControlStateValueOff;
    NSTextField* translateMetadataAndChaptersSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Automatically translate embedded metadata and chapters "
                                                            "using the app's set language on supported sites and "
                                                            "media."))],
    };
    translateMetadataAndChaptersSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    translateMetadataAndChaptersSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    translateMetadataAndChaptersSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* limitSpeedCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Limit Download Speed"))
                             target:self
                             action:@selector(limitSpeedChanged:)],
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
    for (NSInteger i{ 0 }; i < cookiesBrowserPopUpButton.numberOfItems; i++)
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
        [NSButton buttonWithTitle:@(translator._("Browse…")) target:self action:@selector(browseCookiesFile:)],
    };
    browseCookiesFileButton.translatesAutoresizingMaskIntoConstraints = NO;
    NSButton* clearCookiesFileButton
    {
        [NSButton buttonWithTitle:@(translator._("Clear")) target:self action:@selector(clearCookiesFile:)],
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
        NSString* path
        {
            @(panel.URL.fileSystemRepresentation),
        };
        self.cookiesFileTextField.stringValue = path;
        parabolic::configuration::Configuration& configuration{
            parabolic::application::State::instance().configuration(),
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
