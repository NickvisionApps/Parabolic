#import "settings_postprocessing_arguments_view_controller.h"
#include <cstddef>
#include <utility>
#include <vector>
#include "application.h"
#include "configuration.h"
#include "postprocessing.h"
#import "settings_postprocessing_argument_editor_view_controller.h"
#include "translation.h"

@interface SettingsPostprocessingArgumentsViewController () <NSTableViewDataSource, NSTableViewDelegate> {
    std::vector<parabolic::postprocessing::Argument> _arguments;
}

@property(nonatomic, strong) NSTableView* argumentsTableView;
@property(nonatomic, strong) NSSegmentedControl* argumentsSegmentedControl;
@property(nonatomic, strong) NSArray<NSString*>* stepTitles;
@property(nonatomic, strong) NSArray<NSString*>* executableTitles;

@end

@implementation SettingsPostprocessingArgumentsViewController

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
    self.title = @(translator._("Arguments"));
    self.stepTitles = @[
        @(translator._("None")),
        @(translator._("Merger")),
        @(translator._("Modify Chapters")),
        @(translator._("Split Chapters")),
        @(translator._("Extract Audio")),
        @(translator._("Video Remuxer")),
        @(translator._("Video Converter")),
        @(translator._("Metadata")),
        @(translator._("Embed Subtitle")),
        @(translator._("Embed Thumbnail")),
        @(translator._("Subtitles Converter")),
        @(translator._("Thumbnails Converter")),
        @(translator._("Fixup Stretched")),
        @(translator._("Fixup M4a")),
        @(translator._("Fixup M3u8")),
        @(translator._("Fixup Timestamp")),
        @(translator._("Fixup Duration")),
    ];
    self.executableTitles = @[
        @(translator._("None")),
        @(translator._("AtomicParsley")),
        @(translator._("FFmpeg")),
        @(translator._("FFprobe")),
    ];
    _arguments = configuration.postprocessing_options().arguments();
    NSTextField* descriptionLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("Arguments will be shown for selection in the download "
                                                            "discovery dialog."))],
    };
    descriptionLabel.translatesAutoresizingMaskIntoConstraints = NO;
    descriptionLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    descriptionLabel.textColor = NSColor.secondaryLabelColor;
    NSTableView* argumentsTableView{ [[NSTableView alloc] initWithFrame:NSZeroRect] };
    argumentsTableView.dataSource = self;
    argumentsTableView.delegate = self;
    argumentsTableView.usesAlternatingRowBackgroundColors = YES;
    argumentsTableView.allowsMultipleSelection = NO;
    argumentsTableView.allowsEmptySelection = YES;
    argumentsTableView.columnAutoresizingStyle = NSTableViewUniformColumnAutoresizingStyle;
    NSArray<NSString*>* columnIdentifiers = @[ @"name", @"step", @"executable", @"args" ];
    NSArray<NSString*>* columnTitles = @[
        @(translator._("Name")),
        @(translator._("Post Processor")),
        @(translator._("Executable")),
        @(translator._("Args")),
    ];
    for (NSUInteger i{ 0 }; i < columnIdentifiers.count; i++)
    {
        NSTableColumn* column{ [[NSTableColumn alloc] initWithIdentifier:columnIdentifiers[i]] };
        column.title = columnTitles[i];
        column.minWidth = 60;
        column.width = 130;
        column.resizingMask = NSTableColumnUserResizingMask | NSTableColumnAutoresizingMask;
        [argumentsTableView addTableColumn:column];
    }
    argumentsTableView.target = self;
    argumentsTableView.doubleAction = @selector(editArgument:);
    self.argumentsTableView = argumentsTableView;
    NSScrollView* argumentsScrollView{ [[NSScrollView alloc] initWithFrame:NSZeroRect] };
    argumentsScrollView.translatesAutoresizingMaskIntoConstraints = NO;
    argumentsScrollView.hasVerticalScroller = YES;
    argumentsScrollView.borderType = NSBezelBorder;
    argumentsScrollView.documentView = argumentsTableView;
    NSSegmentedControl* argumentsSegmentedControl
    {
        [NSSegmentedControl segmentedControlWithLabels:@[ @"", @"" ]
                                          trackingMode:NSSegmentSwitchTrackingMomentary
                                                target:self
                                                action:@selector(argumentsSegmentClicked:)],
    };
    argumentsSegmentedControl.translatesAutoresizingMaskIntoConstraints = NO;
    [argumentsSegmentedControl setImage:[NSImage imageWithSystemSymbolName:@"plus"
                                                  accessibilityDescription:@(translator._("Add"))]
                             forSegment:0];
    [argumentsSegmentedControl setImage:[NSImage imageWithSystemSymbolName:@"minus"
                                                  accessibilityDescription:@(translator._("Remove"))]
                             forSegment:1];
    [argumentsSegmentedControl setWidth:32 forSegment:0];
    [argumentsSegmentedControl setWidth:32 forSegment:1];
    [argumentsSegmentedControl setEnabled:NO forSegment:1];
    self.argumentsSegmentedControl = argumentsSegmentedControl;
    NSMenu* argumentsMenu{ [[NSMenu alloc] init] };
    NSMenuItem* editMenuItem
    {
        [argumentsMenu addItemWithTitle:@(translator._("Edit…")) action:@selector(editArgument:) keyEquivalent:@""]
    };
    editMenuItem.target = self;
    NSMenuItem* removeMenuItem
    {
        [argumentsMenu addItemWithTitle:@(translator._("Remove")) action:@selector(removeArgument:) keyEquivalent:@""]
    };
    removeMenuItem.target = self;
    argumentsTableView.menu = argumentsMenu;
    NSButton* doneButton
    {
        [NSButton buttonWithTitle:@(translator._("Done")) target:self action:@selector(done:)],
    };
    doneButton.translatesAutoresizingMaskIntoConstraints = NO;
    doneButton.keyEquivalent = @"\r";
    [self.view addSubview:descriptionLabel];
    [self.view addSubview:argumentsScrollView];
    [self.view addSubview:argumentsSegmentedControl];
    [self.view addSubview:doneButton];
    [NSLayoutConstraint activateConstraints:@[
        [self.view.widthAnchor constraintEqualToConstant:560],
        [descriptionLabel.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor constant:20],
        [descriptionLabel.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [descriptionLabel.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [argumentsScrollView.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor constant:20],
        [argumentsScrollView.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [argumentsScrollView.topAnchor constraintEqualToAnchor:descriptionLabel.bottomAnchor constant:12],
        [argumentsScrollView.heightAnchor constraintEqualToConstant:220],
        [argumentsSegmentedControl.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor constant:20],
        [argumentsSegmentedControl.topAnchor constraintEqualToAnchor:argumentsScrollView.bottomAnchor constant:12],
        [doneButton.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [doneButton.centerYAnchor constraintEqualToAnchor:argumentsSegmentedControl.centerYAnchor],
        [argumentsSegmentedControl.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
    ]];
}

- (void)done:(NSButton*)sender
{
    [self.presentingViewController dismissViewController:self];
}

- (void)argumentsSegmentClicked:(NSSegmentedControl*)sender
{
    if (sender.selectedSegment == 0)
    {
        [self addArgument:sender];
    }
    else
    {
        [self removeArgument:sender];
    }
}

- (void)addArgument:(id)sender
{
    [self presentArgumentEditorForRow:-1];
}

- (void)editArgument:(id)sender
{
    NSInteger row{
        sender == self.argumentsTableView || [sender isKindOfClass:[NSMenuItem class]]
                ? self.argumentsTableView.clickedRow
                : self.argumentsTableView.selectedRow,
    };
    if (row >= 0 && std::cmp_less(row, _arguments.size()))
    {
        [self presentArgumentEditorForRow:row];
    }
}

- (void)removeArgument:(id)sender
{
    NSInteger row{
        [sender isKindOfClass:[NSMenuItem class]] ? self.argumentsTableView.clickedRow
                                                  : self.argumentsTableView.selectedRow,
    };
    if (row < 0 || std::cmp_greater_equal(row, _arguments.size()))
    {
        return;
    }
    _arguments.erase(_arguments.begin() + row);
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_arguments(_arguments);
    configuration.set_postprocessing_options(options);
    [self.argumentsTableView reloadData];
}

- (void)presentArgumentEditorForRow:(NSInteger)row
{
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    NSMutableArray<NSString*>* existingNames{ [NSMutableArray array] };
    for (size_t i{ 0 }; i < _arguments.size(); i++)
    {
        if (std::cmp_not_equal(i, row))
        {
            [existingNames addObject:@(_arguments[i].name().c_str())];
        }
    }
    NSString* name{ @"" };
    NSInteger step{ 0 };
    NSInteger executable{ 0 };
    NSString* args{ @"" };
    if (row >= 0)
    {
        const parabolic::postprocessing::Argument& argument{ _arguments[static_cast<size_t>(row)] };
        name = @(argument.name().c_str());
        step = static_cast<NSInteger>(argument.step());
        executable = static_cast<NSInteger>(argument.executable());
        args = @(argument.args().c_str());
    }
    SettingsPostprocessingArgumentEditorViewController* editor =
            [[SettingsPostprocessingArgumentEditorViewController alloc]
                     initWithName:name
                             step:step
                       executable:executable
                             args:args
                    existingNames:existingNames
                       completion:^(NSString* newName, NSInteger newStep, NSInteger newExecutable, NSString* newArgs) {
                         if (row < 0)
                         {
                             self->_arguments.emplace_back(newName.UTF8String,
                                                           static_cast<parabolic::postprocessing::Step>(newStep),
                                                           static_cast<parabolic::postprocessing::Executable>(
                                                                   newExecutable),
                                                           newArgs.UTF8String);
                         }
                         else
                         {
                             parabolic::postprocessing::Argument& argument{
                                 self->_arguments[static_cast<size_t>(row)],
                             };
                             argument.set_name(newName.UTF8String);
                             argument.set_step(static_cast<parabolic::postprocessing::Step>(newStep));
                             argument.set_executable(static_cast<parabolic::postprocessing::Executable>(newExecutable));
                             argument.set_args(newArgs.UTF8String);
                         }
                         parabolic::configuration::Configuration& configuration{
                             parabolic::application::State::instance().configuration(),
                         };
                         parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
                         options.set_arguments(self->_arguments);
                         configuration.set_postprocessing_options(options);
                         [self.argumentsTableView reloadData];
                         [self.argumentsTableView
                                     selectRowIndexes:[NSIndexSet
                                                              indexSetWithIndex:row < 0 ? self->_arguments.size() - 1
                                                                                        : static_cast<NSUInteger>(row)]
                                 byExtendingSelection:NO];
                       }];
    editor.title = @(row < 0 ? translator._("Add Argument") : translator._("Edit Argument"));
    [self presentViewControllerAsSheet:editor];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView
{
    return static_cast<NSInteger>(_arguments.size());
}

- (NSView*)tableView:(NSTableView*)tableView viewForTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)row
{
    NSTableCellView* cell{ [tableView makeViewWithIdentifier:tableColumn.identifier owner:self] };
    if (cell == nil)
    {
        cell = [[NSTableCellView alloc] initWithFrame:NSZeroRect];
        cell.identifier = tableColumn.identifier;
        NSTextField* textField{ [NSTextField labelWithString:@""] };
        textField.translatesAutoresizingMaskIntoConstraints = NO;
        textField.lineBreakMode = NSLineBreakByTruncatingTail;
        [cell addSubview:textField];
        cell.textField = textField;
        [NSLayoutConstraint activateConstraints:@[
            [textField.leadingAnchor constraintEqualToAnchor:cell.leadingAnchor constant:4],
            [textField.trailingAnchor constraintEqualToAnchor:cell.trailingAnchor constant:-4],
            [textField.centerYAnchor constraintEqualToAnchor:cell.centerYAnchor],
        ]];
    }
    const parabolic::postprocessing::Argument& argument{ _arguments[static_cast<size_t>(row)] };
    if ([tableColumn.identifier isEqualToString:@"name"])
    {
        cell.textField.stringValue = @(argument.name().c_str());
    }
    else if ([tableColumn.identifier isEqualToString:@"step"])
    {
        cell.textField.stringValue = self.stepTitles[static_cast<NSUInteger>(argument.step())];
    }
    else if ([tableColumn.identifier isEqualToString:@"executable"])
    {
        cell.textField.stringValue = self.executableTitles[static_cast<NSUInteger>(argument.executable())];
    }
    else
    {
        cell.textField.stringValue = @(argument.args().c_str());
    }
    return cell;
}

- (void)tableViewSelectionDidChange:(NSNotification*)notification
{
    BOOL hasSelection{ self.argumentsTableView.selectedRow >= 0 };
    [self.argumentsSegmentedControl setEnabled:hasSelection forSegment:1];
}

- (BOOL)validateMenuItem:(NSMenuItem*)menuItem
{
    return self.argumentsTableView.clickedRow >= 0;
}
@end
