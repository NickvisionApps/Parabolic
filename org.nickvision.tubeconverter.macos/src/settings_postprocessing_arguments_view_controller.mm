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
@property(nonatomic, strong) NSButton* editArgumentButton;
@property(nonatomic, strong) NSButton* removeArgumentButton;
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
    NSButton* addArgumentButton
    {
        [NSButton buttonWithTitle:@(translator._("Add")) target:self action:@selector(addArgument:)],
    };
    addArgumentButton.translatesAutoresizingMaskIntoConstraints = NO;
    NSButton* editArgumentButton
    {
        [NSButton buttonWithTitle:@(translator._("Edit…")) target:self action:@selector(editArgument:)],
    };
    editArgumentButton.translatesAutoresizingMaskIntoConstraints = NO;
    editArgumentButton.enabled = NO;
    self.editArgumentButton = editArgumentButton;
    NSButton* removeArgumentButton
    {
        [NSButton buttonWithTitle:@(translator._("Remove")) target:self action:@selector(removeArgument:)],
    };
    removeArgumentButton.translatesAutoresizingMaskIntoConstraints = NO;
    removeArgumentButton.enabled = NO;
    self.removeArgumentButton = removeArgumentButton;
    NSButton* doneButton
    {
        [NSButton buttonWithTitle:@(translator._("Done")) target:self action:@selector(done:)],
    };
    doneButton.translatesAutoresizingMaskIntoConstraints = NO;
    doneButton.keyEquivalent = @"\r";
    [self.view addSubview:descriptionLabel];
    [self.view addSubview:argumentsScrollView];
    [self.view addSubview:addArgumentButton];
    [self.view addSubview:editArgumentButton];
    [self.view addSubview:removeArgumentButton];
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
        [addArgumentButton.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor constant:20],
        [addArgumentButton.topAnchor constraintEqualToAnchor:argumentsScrollView.bottomAnchor constant:12],
        [editArgumentButton.leadingAnchor constraintEqualToAnchor:addArgumentButton.trailingAnchor constant:8],
        [editArgumentButton.centerYAnchor constraintEqualToAnchor:addArgumentButton.centerYAnchor],
        [removeArgumentButton.leadingAnchor constraintEqualToAnchor:editArgumentButton.trailingAnchor constant:8],
        [removeArgumentButton.centerYAnchor constraintEqualToAnchor:addArgumentButton.centerYAnchor],
        [doneButton.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [doneButton.centerYAnchor constraintEqualToAnchor:addArgumentButton.centerYAnchor],
        [addArgumentButton.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
    ]];
}

- (void)done:(NSButton*)sender
{
    [self.presentingViewController dismissViewController:self];
}

- (void)addArgument:(NSButton*)sender
{
    [self presentArgumentEditorForRow:-1];
}

- (void)editArgument:(id)sender
{
    NSInteger row{
        sender == self.argumentsTableView ? self.argumentsTableView.clickedRow : self.argumentsTableView.selectedRow,
    };
    if (row >= 0 && std::cmp_less(row, _arguments.size()))
    {
        [self presentArgumentEditorForRow:row];
    }
}

- (void)removeArgument:(NSButton*)sender
{
    NSInteger row{ self.argumentsTableView.selectedRow };
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
    NSTextField* cell{ [tableView makeViewWithIdentifier:tableColumn.identifier owner:self] };
    if (cell == nil)
    {
        cell = [NSTextField labelWithString:@""];
        cell.identifier = tableColumn.identifier;
        cell.lineBreakMode = NSLineBreakByTruncatingTail;
    }
    const parabolic::postprocessing::Argument& argument{ _arguments[static_cast<size_t>(row)] };
    if ([tableColumn.identifier isEqualToString:@"name"])
    {
        cell.stringValue = @(argument.name().c_str());
    }
    else if ([tableColumn.identifier isEqualToString:@"step"])
    {
        cell.stringValue = self.stepTitles[static_cast<NSUInteger>(argument.step())];
    }
    else if ([tableColumn.identifier isEqualToString:@"executable"])
    {
        cell.stringValue = self.executableTitles[static_cast<NSUInteger>(argument.executable())];
    }
    else
    {
        cell.stringValue = @(argument.args().c_str());
    }
    return cell;
}

- (void)tableViewSelectionDidChange:(NSNotification*)notification
{
    BOOL hasSelection{ self.argumentsTableView.selectedRow >= 0 };
    self.editArgumentButton.enabled = hasSelection;
    self.removeArgumentButton.enabled = hasSelection;
}
@end
