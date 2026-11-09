#import "settings_downloads_view_controller.h"
#include "application.h"
#import "settings_model.h"
#include "translation.h"

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
    SettingsModel* model{ [SettingsModel sharedModel] };
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
    [maxActiveDownloadsTextField bind:NSValueBinding toObject:model withKeyPath:@"maxActiveDownloads" options:nil];
    NSStepper* maxActiveDownloadsStepper{ [[NSStepper alloc] initWithFrame:NSZeroRect] };
    maxActiveDownloadsStepper.translatesAutoresizingMaskIntoConstraints = NO;
    maxActiveDownloadsStepper.minValue = 1;
    maxActiveDownloadsStepper.maxValue = 10;
    maxActiveDownloadsStepper.increment = 1;
    [maxActiveDownloadsStepper bind:NSValueBinding toObject:model withKeyPath:@"maxActiveDownloads" options:nil];
    NSButton* overwriteExistingFilesCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Overwrite Existing Files")) target:nil action:nil],
    };
    overwriteExistingFilesCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [overwriteExistingFilesCheckbox bind:NSValueBinding
                                toObject:model
                             withKeyPath:@"overwriteExistingFiles"
                                 options:nil];
    NSButton* preventSuspendCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Prevent Sleep During Downloads")) target:nil action:nil],
    };
    preventSuspendCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [preventSuspendCheckbox bind:NSValueBinding toObject:model withKeyPath:@"preventSuspend" options:nil];
    NSTextField* ytdlpArgumentsLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Extra yt-dlp Download Arguments")).c_str())],
    };
    ytdlpArgumentsLabel.translatesAutoresizingMaskIntoConstraints = NO;
    ytdlpArgumentsLabel.alignment = NSTextAlignmentRight;
    NSTextField* ytdlpArgumentsTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    ytdlpArgumentsTextField.translatesAutoresizingMaskIntoConstraints = NO;
    [ytdlpArgumentsTextField bind:NSValueBinding toObject:model withKeyPath:@"downloadYtdlpArguments" options:nil];
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

@end
