#import "settings_aria2_view_controller.h"
#include "application.h"
#import "settings_model.h"
#include "translation.h"

@implementation SettingsAria2ViewController

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
    self.title = @(translator._p("Settings", "aria2"));
    NSButton* useAriaCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Use aria2")) target:nil action:nil],
    };
    useAriaCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    [useAriaCheckbox bind:NSValueBinding toObject:model withKeyPath:@"useAria" options:nil];
    NSMutableAttributedString* useAriaSubtitle
    {
        [[NSMutableAttributedString alloc]
                initWithString:@(translator._(
                                       "An alternative downloader that may be faster in some regions compared to "
                                       "yt-dlp's native downloader."))
                    attributes:@{
                        NSFontAttributeName : [NSFont systemFontOfSize:NSFont.smallSystemFontSize],
                        NSForegroundColorAttributeName : NSColor.secondaryLabelColor,
                    }],
    };
    [useAriaSubtitle appendAttributedString:[[NSAttributedString alloc]
                                                    initWithString:@" "
                                                        attributes:@{
                                                            NSFontAttributeName : [NSFont
                                                                    systemFontOfSize:NSFont.smallSystemFontSize],
                                                        }]];
    [useAriaSubtitle appendAttributedString:[[NSAttributedString alloc]
                                                    initWithString:@(translator._("Learn More…"))
                                                        attributes:@{
                                                            NSFontAttributeName : [NSFont
                                                                    systemFontOfSize:NSFont.smallSystemFontSize],
                                                            NSLinkAttributeName : @"https://aria2.github.io",
                                                        }]];
    NSTextField* useAriaSubtitleLabel{ [NSTextField wrappingLabelWithString:@""] };
    useAriaSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    useAriaSubtitleLabel.allowsEditingTextAttributes = YES;
    useAriaSubtitleLabel.attributedStringValue = useAriaSubtitle;
    NSTextField* maxConnectionsLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Maximum Connections Per Server")).c_str())],
    };
    maxConnectionsLabel.translatesAutoresizingMaskIntoConstraints = NO;
    maxConnectionsLabel.alignment = NSTextAlignmentRight;
    NSNumberFormatter* maxConnectionsFormatter{ [[NSNumberFormatter alloc] init] };
    maxConnectionsFormatter.numberStyle = NSNumberFormatterNoStyle;
    maxConnectionsFormatter.minimum = @1;
    maxConnectionsFormatter.maximum = @16;
    NSTextField* maxConnectionsTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    maxConnectionsTextField.translatesAutoresizingMaskIntoConstraints = NO;
    maxConnectionsTextField.formatter = maxConnectionsFormatter;
    [maxConnectionsTextField bind:NSValueBinding toObject:model withKeyPath:@"maxConnectionsPerServer" options:nil];
    [maxConnectionsTextField bind:NSEnabledBinding toObject:model withKeyPath:@"useAria" options:nil];
    NSStepper* maxConnectionsStepper{ [[NSStepper alloc] initWithFrame:NSZeroRect] };
    maxConnectionsStepper.translatesAutoresizingMaskIntoConstraints = NO;
    maxConnectionsStepper.minValue = 1;
    maxConnectionsStepper.maxValue = 16;
    maxConnectionsStepper.increment = 1;
    [maxConnectionsStepper bind:NSValueBinding toObject:model withKeyPath:@"maxConnectionsPerServer" options:nil];
    [maxConnectionsStepper bind:NSEnabledBinding toObject:model withKeyPath:@"useAria" options:nil];
    NSTextField* minSplitSizeLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Minimum Split Size (MiB)")).c_str())],
    };
    minSplitSizeLabel.translatesAutoresizingMaskIntoConstraints = NO;
    minSplitSizeLabel.alignment = NSTextAlignmentRight;
    NSNumberFormatter* minSplitSizeFormatter{ [[NSNumberFormatter alloc] init] };
    minSplitSizeFormatter.numberStyle = NSNumberFormatterNoStyle;
    minSplitSizeFormatter.minimum = @1;
    minSplitSizeFormatter.maximum = @1024;
    NSTextField* minSplitSizeTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    minSplitSizeTextField.translatesAutoresizingMaskIntoConstraints = NO;
    minSplitSizeTextField.formatter = minSplitSizeFormatter;
    [minSplitSizeTextField bind:NSValueBinding toObject:model withKeyPath:@"minSplitSize" options:nil];
    [minSplitSizeTextField bind:NSEnabledBinding toObject:model withKeyPath:@"useAria" options:nil];
    NSStepper* minSplitSizeStepper{ [[NSStepper alloc] initWithFrame:NSZeroRect] };
    minSplitSizeStepper.translatesAutoresizingMaskIntoConstraints = NO;
    minSplitSizeStepper.minValue = 1;
    minSplitSizeStepper.maxValue = 1024;
    minSplitSizeStepper.increment = 1;
    [minSplitSizeStepper bind:NSValueBinding toObject:model withKeyPath:@"minSplitSize" options:nil];
    [minSplitSizeStepper bind:NSEnabledBinding toObject:model withKeyPath:@"useAria" options:nil];
    [self.view addSubview:useAriaCheckbox];
    [self.view addSubview:useAriaSubtitleLabel];
    [self.view addSubview:maxConnectionsLabel];
    [self.view addSubview:maxConnectionsTextField];
    [self.view addSubview:maxConnectionsStepper];
    [self.view addSubview:minSplitSizeLabel];
    [self.view addSubview:minSplitSizeTextField];
    [self.view addSubview:minSplitSizeStepper];
    [NSLayoutConstraint activateConstraints:@[
        [self.view.widthAnchor constraintEqualToConstant:600],
        [useAriaCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [useAriaCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [useAriaCheckbox.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [useAriaSubtitleLabel.leadingAnchor constraintEqualToAnchor:useAriaCheckbox.leadingAnchor],
        [useAriaSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [useAriaSubtitleLabel.topAnchor constraintEqualToAnchor:useAriaCheckbox.bottomAnchor constant:6],
        [maxConnectionsLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [maxConnectionsLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [maxConnectionsTextField.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [maxConnectionsTextField.widthAnchor constraintEqualToConstant:40],
        [maxConnectionsLabel.firstBaselineAnchor constraintEqualToAnchor:maxConnectionsTextField.firstBaselineAnchor],
        [maxConnectionsStepper.leadingAnchor constraintEqualToAnchor:maxConnectionsTextField.trailingAnchor constant:4],
        [maxConnectionsStepper.centerYAnchor constraintEqualToAnchor:maxConnectionsTextField.centerYAnchor],
        [maxConnectionsTextField.topAnchor constraintEqualToAnchor:useAriaSubtitleLabel.bottomAnchor constant:16],
        [minSplitSizeLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [minSplitSizeLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [minSplitSizeTextField.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [minSplitSizeTextField.widthAnchor constraintEqualToConstant:60],
        [minSplitSizeLabel.firstBaselineAnchor constraintEqualToAnchor:minSplitSizeTextField.firstBaselineAnchor],
        [minSplitSizeStepper.leadingAnchor constraintEqualToAnchor:minSplitSizeTextField.trailingAnchor constant:4],
        [minSplitSizeStepper.centerYAnchor constraintEqualToAnchor:minSplitSizeTextField.centerYAnchor],
        [minSplitSizeTextField.topAnchor constraintEqualToAnchor:maxConnectionsTextField.bottomAnchor constant:12],
        [minSplitSizeTextField.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
    ]];
}

@end
