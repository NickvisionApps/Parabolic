#import "settings_extractor_view_controller.h"
#include "application.h"
#import "settings_model.h"
#include "translation.h"

@implementation SettingsExtractorViewController

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSZeroRect];
    self.view.translatesAutoresizingMaskIntoConstraints = NO;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    SettingsModel* model{ [SettingsModel sharedModel] };
    self.title = @(translator._p("Settings", "Extractor"));
    NSButton* usePartFilesCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Use Part Files")) target:nil action:nil],
    };
    usePartFilesCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [usePartFilesCheckbox bind:NSValueBinding toObject:model withKeyPath:@"usePartFiles" options:nil];
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
        [NSButton checkboxWithTitle:@(translator._("Use SponsorBlock for YouTube")) target:nil action:nil],
    };
    youtubeSponsorBlockCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [youtubeSponsorBlockCheckbox bind:NSValueBinding toObject:model withKeyPath:@"youtubeSponsorBlock" options:nil];
    NSTextField* youtubeSponsorBlockSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Try to remove sponsored segments from videos."))],
    };
    youtubeSponsorBlockSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    youtubeSponsorBlockSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    youtubeSponsorBlockSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* translateMetadataAndChaptersCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Translate Metadata and Chapters")) target:nil action:nil],
    };
    translateMetadataAndChaptersCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [translateMetadataAndChaptersCheckbox bind:NSValueBinding
                                      toObject:model
                                   withKeyPath:@"translateMetadataAndChapters"
                                       options:nil];
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
        [NSButton checkboxWithTitle:@(translator._("Limit Download Speed")) target:nil action:nil],
    };
    limitSpeedCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [limitSpeedCheckbox bind:NSValueBinding toObject:model withKeyPath:@"limitSpeed" options:nil];
    NSTextField* speedLimitLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Speed Limit (KiB/s)")).c_str())],
    };
    speedLimitLabel.translatesAutoresizingMaskIntoConstraints = NO;
    speedLimitLabel.alignment = NSTextAlignmentRight;
    NSNumberFormatter* speedLimitFormatter{ [[NSNumberFormatter alloc] init] };
    speedLimitFormatter.numberStyle = NSNumberFormatterNoStyle;
    speedLimitFormatter.minimum = @512;
    speedLimitFormatter.maximum = @10240;
    NSTextField* speedLimitTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    speedLimitTextField.translatesAutoresizingMaskIntoConstraints = NO;
    speedLimitTextField.formatter = speedLimitFormatter;
    [speedLimitTextField bind:NSValueBinding toObject:model withKeyPath:@"speedLimit" options:nil];
    [speedLimitTextField bind:NSEnabledBinding toObject:model withKeyPath:@"limitSpeed" options:nil];
    NSStepper* speedLimitStepper{ [[NSStepper alloc] initWithFrame:NSZeroRect] };
    speedLimitStepper.translatesAutoresizingMaskIntoConstraints = NO;
    speedLimitStepper.minValue = 512;
    speedLimitStepper.maxValue = 10240;
    speedLimitStepper.increment = 256;
    [speedLimitStepper bind:NSValueBinding toObject:model withKeyPath:@"speedLimit" options:nil];
    [speedLimitStepper bind:NSEnabledBinding toObject:model withKeyPath:@"limitSpeed" options:nil];
    NSTextField* proxyUrlLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Proxy URL")).c_str())],
    };
    proxyUrlLabel.translatesAutoresizingMaskIntoConstraints = NO;
    proxyUrlLabel.alignment = NSTextAlignmentRight;
    NSTextField* proxyUrlTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    proxyUrlTextField.translatesAutoresizingMaskIntoConstraints = NO;
    [proxyUrlTextField bind:NSValueBinding toObject:model withKeyPath:@"proxyUrl" options:nil];
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
    [cookiesBrowserPopUpButton bind:NSSelectedTagBinding toObject:model withKeyPath:@"cookiesBrowser" options:nil];
    NSTextField* cookiesFileLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Cookies File")).c_str())],
    };
    cookiesFileLabel.translatesAutoresizingMaskIntoConstraints = NO;
    cookiesFileLabel.alignment = NSTextAlignmentRight;
    NSTextField* cookiesFileTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    cookiesFileTextField.translatesAutoresizingMaskIntoConstraints = NO;
    [cookiesFileTextField bind:NSValueBinding toObject:model withKeyPath:@"cookiesPath" options:nil];
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
        [self.view.widthAnchor constraintEqualToConstant:600],
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

- (void)browseCookiesFile:(NSButton*)sender
{
    NSOpenPanel* panel{ [NSOpenPanel openPanel] };
    panel.canChooseFiles = YES;
    panel.canChooseDirectories = NO;
    panel.allowsMultipleSelection = NO;
    NSWindow* window{ self.view.window };
    if (window == nil)
    {
        return;
    }
    [panel beginSheetModalForWindow:window
                  completionHandler:^(NSModalResponse response) {
                    if (response != NSModalResponseOK)
                    {
                        return;
                    }
                    [SettingsModel sharedModel].cookiesPath = @(panel.URL.fileSystemRepresentation);
                  }];
}

- (void)clearCookiesFile:(NSButton*)sender
{
    [SettingsModel sharedModel].cookiesPath = @"";
}

@end
