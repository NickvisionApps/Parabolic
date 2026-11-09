#import "settings_postprocessing_view_controller.h"
#include <algorithm>
#include <thread>
#include "application.h"
#import "settings_model.h"
#import "settings_postprocessing_arguments_view_controller.h"
#include "translation.h"

@implementation SettingsPostprocessingViewController

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
    self.title = @(translator._p("Settings", "Postprocessing"));
    NSButton* embedMetadataCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Embed Metadata")) target:nil action:nil],
    };
    embedMetadataCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [embedMetadataCheckbox bind:NSValueBinding toObject:model withKeyPath:@"embedMetadata" options:nil];
    NSButton* removeSourceDataCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Remove Source Data")) target:nil action:nil],
    };
    removeSourceDataCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [removeSourceDataCheckbox bind:NSValueBinding toObject:model withKeyPath:@"removeSourceData" options:nil];
    [removeSourceDataCheckbox bind:NSEnabledBinding toObject:model withKeyPath:@"embedMetadata" options:nil];
    NSTextField* removeSourceDataSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Clear metadata fields containing identifying download "
                                                            "information."))],
    };
    removeSourceDataSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    removeSourceDataSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    removeSourceDataSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* embedThumbnailsCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Embed Thumbnails")) target:nil action:nil],
    };
    embedThumbnailsCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [embedThumbnailsCheckbox bind:NSValueBinding toObject:model withKeyPath:@"embedThumbnails" options:nil];
    NSTextField* embedThumbnailsSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("If the file type does not support embedding, the "
                                                            "thumbnail will be written to a separate image file."))],
    };
    embedThumbnailsSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    embedThumbnailsSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    embedThumbnailsSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* cropAudioThumbnailsCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Crop Audio Thumbnails")) target:nil action:nil],
    };
    cropAudioThumbnailsCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [cropAudioThumbnailsCheckbox bind:NSValueBinding toObject:model withKeyPath:@"cropAudioThumbnails" options:nil];
    [cropAudioThumbnailsCheckbox bind:NSEnabledBinding toObject:model withKeyPath:@"embedThumbnails" options:nil];
    NSTextField* cropAudioThumbnailsSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Crop thumbnails of audio files to squares."))],
    };
    cropAudioThumbnailsSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    cropAudioThumbnailsSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    cropAudioThumbnailsSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* embedChaptersCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Embed Chapters")) target:nil action:nil],
    };
    embedChaptersCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [embedChaptersCheckbox bind:NSValueBinding toObject:model withKeyPath:@"embedChapters" options:nil];
    NSButton* embedSubtitlesCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Embed Subtitles")) target:nil action:nil],
    };
    embedSubtitlesCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [embedSubtitlesCheckbox bind:NSValueBinding toObject:model withKeyPath:@"embedSubtitles" options:nil];
    NSTextField* embedSubtitlesSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("If disabled or if embedding is not supported, "
                                                            "downloaded subtitles will be saved to separate "
                                                            "files."))],
    };
    embedSubtitlesSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    embedSubtitlesSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    embedSubtitlesSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSTextField* threadsLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("FFmpeg Threads")).c_str())],
    };
    threadsLabel.translatesAutoresizingMaskIntoConstraints = NO;
    threadsLabel.alignment = NSTextAlignmentRight;
    NSNumberFormatter* threadsFormatter{ [[NSNumberFormatter alloc] init] };
    threadsFormatter.numberStyle = NSNumberFormatterNoStyle;
    threadsFormatter.minimum = @1;
    threadsFormatter.maximum = @(std::max(1U, std::thread::hardware_concurrency()));
    NSTextField* threadsTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    threadsTextField.translatesAutoresizingMaskIntoConstraints = NO;
    threadsTextField.formatter = threadsFormatter;
    [threadsTextField bind:NSValueBinding toObject:model withKeyPath:@"postprocessingThreads" options:nil];
    NSStepper* threadsStepper{ [[NSStepper alloc] initWithFrame:NSZeroRect] };
    threadsStepper.translatesAutoresizingMaskIntoConstraints = NO;
    threadsStepper.minValue = 1;
    threadsStepper.maxValue = std::max(1U, std::thread::hardware_concurrency());
    threadsStepper.increment = 1;
    [threadsStepper bind:NSValueBinding toObject:model withKeyPath:@"postprocessingThreads" options:nil];
    NSTextField* threadsSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Limit the number of threads used by ffmpeg."))],
    };
    threadsSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    threadsSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    threadsSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* manageArgumentsButton
    {
        [NSButton buttonWithTitle:@(translator._("Manage Arguments…")) target:self action:@selector(manageArguments:)],
    };
    manageArgumentsButton.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:embedMetadataCheckbox];
    [self.view addSubview:removeSourceDataCheckbox];
    [self.view addSubview:removeSourceDataSubtitleLabel];
    [self.view addSubview:embedThumbnailsCheckbox];
    [self.view addSubview:embedThumbnailsSubtitleLabel];
    [self.view addSubview:cropAudioThumbnailsCheckbox];
    [self.view addSubview:cropAudioThumbnailsSubtitleLabel];
    [self.view addSubview:embedChaptersCheckbox];
    [self.view addSubview:embedSubtitlesCheckbox];
    [self.view addSubview:embedSubtitlesSubtitleLabel];
    [self.view addSubview:threadsLabel];
    [self.view addSubview:threadsTextField];
    [self.view addSubview:threadsStepper];
    [self.view addSubview:threadsSubtitleLabel];
    [self.view addSubview:manageArgumentsButton];
    [NSLayoutConstraint activateConstraints:@[
        [self.view.widthAnchor constraintEqualToConstant:600],
        [embedMetadataCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [embedMetadataCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [embedMetadataCheckbox.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [removeSourceDataCheckbox.leadingAnchor constraintEqualToAnchor:embedMetadataCheckbox.leadingAnchor
                                                               constant:20],
        [removeSourceDataCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                          constant:-20],
        [removeSourceDataCheckbox.topAnchor constraintEqualToAnchor:embedMetadataCheckbox.bottomAnchor constant:8],
        [removeSourceDataSubtitleLabel.leadingAnchor constraintEqualToAnchor:removeSourceDataCheckbox.leadingAnchor
                                                                    constant:20],
        [removeSourceDataSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                               constant:-20],
        [removeSourceDataSubtitleLabel.topAnchor constraintEqualToAnchor:removeSourceDataCheckbox.bottomAnchor
                                                                constant:6],
        [embedThumbnailsCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [embedThumbnailsCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                         constant:-20],
        [embedThumbnailsCheckbox.topAnchor constraintEqualToAnchor:removeSourceDataSubtitleLabel.bottomAnchor
                                                          constant:16],
        [embedThumbnailsSubtitleLabel.leadingAnchor constraintEqualToAnchor:embedThumbnailsCheckbox.leadingAnchor],
        [embedThumbnailsSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                              constant:-20],
        [embedThumbnailsSubtitleLabel.topAnchor constraintEqualToAnchor:embedThumbnailsCheckbox.bottomAnchor
                                                               constant:6],
        [cropAudioThumbnailsCheckbox.leadingAnchor constraintEqualToAnchor:embedThumbnailsCheckbox.leadingAnchor
                                                                  constant:20],
        [cropAudioThumbnailsCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                             constant:-20],
        [cropAudioThumbnailsCheckbox.topAnchor constraintEqualToAnchor:embedThumbnailsSubtitleLabel.bottomAnchor
                                                              constant:8],
        [cropAudioThumbnailsSubtitleLabel.leadingAnchor
                constraintEqualToAnchor:cropAudioThumbnailsCheckbox.leadingAnchor
                               constant:20],
        [cropAudioThumbnailsSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                  constant:-20],
        [cropAudioThumbnailsSubtitleLabel.topAnchor constraintEqualToAnchor:cropAudioThumbnailsCheckbox.bottomAnchor
                                                                   constant:6],
        [embedChaptersCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [embedChaptersCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [embedChaptersCheckbox.topAnchor constraintEqualToAnchor:cropAudioThumbnailsSubtitleLabel.bottomAnchor
                                                        constant:16],
        [embedSubtitlesCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [embedSubtitlesCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [embedSubtitlesCheckbox.topAnchor constraintEqualToAnchor:embedChaptersCheckbox.bottomAnchor constant:8],
        [embedSubtitlesSubtitleLabel.leadingAnchor constraintEqualToAnchor:embedSubtitlesCheckbox.leadingAnchor],
        [embedSubtitlesSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                             constant:-20],
        [embedSubtitlesSubtitleLabel.topAnchor constraintEqualToAnchor:embedSubtitlesCheckbox.bottomAnchor constant:6],
        [threadsLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [threadsLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [threadsTextField.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [threadsTextField.widthAnchor constraintEqualToConstant:40],
        [threadsLabel.firstBaselineAnchor constraintEqualToAnchor:threadsTextField.firstBaselineAnchor],
        [threadsStepper.leadingAnchor constraintEqualToAnchor:threadsTextField.trailingAnchor constant:4],
        [threadsStepper.centerYAnchor constraintEqualToAnchor:threadsTextField.centerYAnchor],
        [threadsTextField.topAnchor constraintEqualToAnchor:embedSubtitlesSubtitleLabel.bottomAnchor constant:16],
        [threadsSubtitleLabel.leadingAnchor constraintEqualToAnchor:threadsTextField.leadingAnchor],
        [threadsSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [threadsSubtitleLabel.topAnchor constraintEqualToAnchor:threadsTextField.bottomAnchor constant:6],
        [manageArgumentsButton.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor constant:20],
        [manageArgumentsButton.topAnchor constraintEqualToAnchor:threadsSubtitleLabel.bottomAnchor constant:16],
        [manageArgumentsButton.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
    ]];
}

- (void)manageArguments:(NSButton*)sender
{
    [self presentViewControllerAsSheet:[[SettingsPostprocessingArgumentsViewController alloc] init]];
}

@end
