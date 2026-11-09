#import "settings_history_view_controller.h"
#include "application.h"
#include "configuration.h"
#include "history.h"
#include "translation.h"

@implementation SettingsHistoryViewController

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
