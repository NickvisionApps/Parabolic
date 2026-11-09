#import "settings_postprocessing_view_controller.h"
#include "application.h"
#include "configuration.h"
#include "postprocessing.h"
#import "settings_postprocessing_arguments_view_controller.h"
#include "translation.h"

@interface SettingsPostprocessingViewController ()

@property(nonatomic, strong) NSButton* removeSourceDataCheckbox;
@property(nonatomic, strong) NSButton* cropAudioThumbnailsCheckbox;
@property(nonatomic, strong) NSTextField* threadsTextField;
@property(nonatomic, strong) NSStepper* threadsStepper;

@end

@implementation SettingsPostprocessingViewController

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
    parabolic::postprocessing::Options postprocessingOptions{ configuration.postprocessing_options() };
    self.title = @(translator._p("Settings", "Postprocessing"));
    NSButton* embedMetadataCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Embed Metadata"))
                             target:self
                             action:@selector(embedMetadataChanged:)],
    };
    embedMetadataCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    embedMetadataCheckbox.state =
            postprocessingOptions.embed_metadata() ? NSControlStateValueOn : NSControlStateValueOff;
    NSButton* removeSourceDataCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Remove Source Data"))
                             target:self
                             action:@selector(removeSourceDataChanged:)],
    };
    removeSourceDataCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    removeSourceDataCheckbox.state =
            postprocessingOptions.remove_source_data() ? NSControlStateValueOn : NSControlStateValueOff;
    removeSourceDataCheckbox.enabled = postprocessingOptions.embed_metadata();
    self.removeSourceDataCheckbox = removeSourceDataCheckbox;
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
        [NSButton checkboxWithTitle:@(translator._("Embed Thumbnails"))
                             target:self
                             action:@selector(embedThumbnailsChanged:)],
    };
    embedThumbnailsCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    embedThumbnailsCheckbox.state =
            postprocessingOptions.embed_thumbnails() ? NSControlStateValueOn : NSControlStateValueOff;
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
        [NSButton checkboxWithTitle:@(translator._("Crop Audio Thumbnails"))
                             target:self
                             action:@selector(cropAudioThumbnailsChanged:)],
    };
    cropAudioThumbnailsCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    cropAudioThumbnailsCheckbox.state =
            postprocessingOptions.crop_audio_thumbnails() ? NSControlStateValueOn : NSControlStateValueOff;
    cropAudioThumbnailsCheckbox.enabled = postprocessingOptions.embed_thumbnails();
    self.cropAudioThumbnailsCheckbox = cropAudioThumbnailsCheckbox;
    NSTextField* cropAudioThumbnailsSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Crop thumbnails of audio files to squares."))],
    };
    cropAudioThumbnailsSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    cropAudioThumbnailsSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    cropAudioThumbnailsSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* embedChaptersCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Embed Chapters"))
                             target:self
                             action:@selector(embedChaptersChanged:)],
    };
    embedChaptersCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    embedChaptersCheckbox.state =
            postprocessingOptions.embed_chapters() ? NSControlStateValueOn : NSControlStateValueOff;
    NSButton* embedSubtitlesCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Embed Subtitles"))
                             target:self
                             action:@selector(embedSubtitlesChanged:)],
    };
    embedSubtitlesCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    embedSubtitlesCheckbox.state =
            postprocessingOptions.embed_subtitles() ? NSControlStateValueOn : NSControlStateValueOff;
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
    threadsFormatter.maximum = @64;
    NSTextField* threadsTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    threadsTextField.translatesAutoresizingMaskIntoConstraints = NO;
    threadsTextField.formatter = threadsFormatter;
    threadsTextField.integerValue = postprocessingOptions.threads();
    threadsTextField.target = self;
    threadsTextField.action = @selector(threadsTextFieldChanged:);
    self.threadsTextField = threadsTextField;
    NSStepper* threadsStepper{ [[NSStepper alloc] initWithFrame:NSZeroRect] };
    threadsStepper.translatesAutoresizingMaskIntoConstraints = NO;
    threadsStepper.minValue = 1;
    threadsStepper.maxValue = 64;
    threadsStepper.increment = 1;
    threadsStepper.integerValue = postprocessingOptions.threads();
    threadsStepper.target = self;
    threadsStepper.action = @selector(threadsStepperChanged:);
    self.threadsStepper = threadsStepper;
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
        [self.view.widthAnchor constraintEqualToConstant:700],
        [embedMetadataCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [embedMetadataCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [embedMetadataCheckbox.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [removeSourceDataCheckbox.leadingAnchor constraintEqualToAnchor:embedMetadataCheckbox.leadingAnchor],
        [removeSourceDataCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                          constant:-20],
        [removeSourceDataCheckbox.topAnchor constraintEqualToAnchor:embedMetadataCheckbox.bottomAnchor constant:8],
        [removeSourceDataSubtitleLabel.leadingAnchor constraintEqualToAnchor:removeSourceDataCheckbox.leadingAnchor],
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
        [cropAudioThumbnailsCheckbox.leadingAnchor constraintEqualToAnchor:embedThumbnailsCheckbox.leadingAnchor],
        [cropAudioThumbnailsCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                             constant:-20],
        [cropAudioThumbnailsCheckbox.topAnchor constraintEqualToAnchor:embedThumbnailsSubtitleLabel.bottomAnchor
                                                              constant:8],
        [cropAudioThumbnailsSubtitleLabel.leadingAnchor constraintEqualToAnchor:cropAudioThumbnailsCheckbox.leadingAnchor],
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

- (void)embedMetadataChanged:(NSButton*)sender
{
    BOOL enabled{ sender.state == NSControlStateValueOn };
    self.removeSourceDataCheckbox.enabled = enabled;
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_embed_metadata(enabled);
    configuration.set_postprocessing_options(options);
}

- (void)removeSourceDataChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_remove_source_data(sender.state == NSControlStateValueOn);
    configuration.set_postprocessing_options(options);
}

- (void)embedThumbnailsChanged:(NSButton*)sender
{
    BOOL enabled{ sender.state == NSControlStateValueOn };
    self.cropAudioThumbnailsCheckbox.enabled = enabled;
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_embed_thumbnails(enabled);
    configuration.set_postprocessing_options(options);
}

- (void)cropAudioThumbnailsChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_crop_audio_thumbnails(sender.state == NSControlStateValueOn);
    configuration.set_postprocessing_options(options);
}

- (void)embedChaptersChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_embed_chapters(sender.state == NSControlStateValueOn);
    configuration.set_postprocessing_options(options);
}

- (void)embedSubtitlesChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_embed_subtitles(sender.state == NSControlStateValueOn);
    configuration.set_postprocessing_options(options);
}

- (void)threadsTextFieldChanged:(NSTextField*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_threads(static_cast<int>(sender.integerValue));
    configuration.set_postprocessing_options(options);
    self.threadsStepper.integerValue = sender.integerValue;
}

- (void)manageArguments:(NSButton*)sender
{
    [self presentViewControllerAsSheet:[[SettingsPostprocessingArgumentsViewController alloc] init]];
}

- (void)threadsStepperChanged:(NSStepper*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_threads(static_cast<int>(sender.integerValue));
    configuration.set_postprocessing_options(options);
    self.threadsTextField.integerValue = sender.integerValue;
}

@end
