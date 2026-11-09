#import "settings_downloads_view_controller.h"
#include "application.h"
#include "configuration.h"
#include "download.h"
#include "translation.h"

@interface SettingsDownloadsViewController ()

@property(nonatomic, strong) NSTextField* maxActiveDownloadsTextField;
@property(nonatomic, strong) NSStepper* maxActiveDownloadsStepper;

@end

@implementation SettingsDownloadsViewController

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSZeroRect];
    self.view.translatesAutoresizingMaskIntoConstraints = NO;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
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
    maxActiveDownloadsFormatter.maximum = @10;
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
    maxActiveDownloadsStepper.maxValue = 10;
    maxActiveDownloadsStepper.increment = 1;
    maxActiveDownloadsStepper.integerValue = downloadOptions.max_number_of_active_downloads();
    maxActiveDownloadsStepper.target = self;
    maxActiveDownloadsStepper.action = @selector(maxActiveDownloadsStepperChanged:);
    self.maxActiveDownloadsStepper = maxActiveDownloadsStepper;
    NSButton* overwriteExistingFilesCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Overwrite Existing Files"))
                             target:self
                             action:@selector(overwriteExistingFilesChanged:)],
    };
    overwriteExistingFilesCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    overwriteExistingFilesCheckbox.state =
            downloadOptions.overwrite_existing_files() ? NSControlStateValueOn : NSControlStateValueOff;
    NSButton* preventSuspendCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Prevent Sleep During Downloads"))
                             target:self
                             action:@selector(preventSuspendChanged:)],
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
        [self.view.widthAnchor constraintEqualToConstant:600],
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
    if (sender.stringValue.length == 0 || sender.integerValue < 1 || sender.integerValue > 10)
    {
        sender.integerValue = self.maxActiveDownloadsStepper.integerValue;
        return;
    }
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
