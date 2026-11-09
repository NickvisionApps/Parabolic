#import "settings_general_view_controller.h"
#include "application.h"
#include "configuration.h"
#import "settings_model.h"
#include "translation.h"

@implementation SettingsGeneralViewController

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
    [themePopUpButton bind:NSSelectedTagBinding toObject:model withKeyPath:@"theme" options:nil];
    NSTextField* languageLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Translation Language")).c_str())],
    };
    languageLabel.translatesAutoresizingMaskIntoConstraints = NO;
    languageLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* languagePopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    languagePopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    [languagePopUpButton bind:NSContentValuesBinding toObject:model withKeyPath:@"languageNames" options:nil];
    [languagePopUpButton bind:NSSelectedIndexBinding toObject:model withKeyPath:@"languageIndex" options:nil];
    NSTextField* languageSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("An application restart is required for a change to take "
                                                            "effect."))],
    };
    languageSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    languageSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    languageSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* autoUpdateCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Automatically Check for Updates")) target:nil action:nil],
    };
    autoUpdateCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [autoUpdateCheckbox bind:NSValueBinding toObject:model withKeyPath:@"automaticallyCheckForUpdates" options:nil];
    NSButton* allowPreviewUpdatesCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Allow Preview Updates")) target:nil action:nil],
    };
    allowPreviewUpdatesCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [allowPreviewUpdatesCheckbox bind:NSValueBinding toObject:model withKeyPath:@"allowPreviewUpdates" options:nil];
    NSTextField* allowPreviewUpdatesSubtitleLabel
    {
        [NSTextField
                wrappingLabelWithString:@(translator._(
                                                "Update the app and dependencies, such as yt-dlp, to beta versions."))],
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

@end
