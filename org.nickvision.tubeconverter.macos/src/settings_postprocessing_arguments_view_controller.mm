#import "settings_postprocessing_arguments_view_controller.h"
#include <algorithm>
#include <utility>
#include "application.h"
#import "settings_model.h"
#import "settings_postprocessing_argument_editor_view_controller.h"
#include "translation.h"

@interface SettingsPostprocessingArgumentsViewController ()

@property(nonatomic, strong) NSTableView* argumentsTableView;
@property(nonatomic, strong) NSSegmentedControl* argumentsSegmentedControl;
@property(nonatomic, strong) NSArray<NSString*>* stepTitles;
@property(nonatomic, strong) NSArray<NSString*>* executableTitles;

@end

@implementation SettingsPostprocessingArgumentsViewController

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSZeroRect];
    self.view.translatesAutoresizingMaskIntoConstraints = NO;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
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
    argumentsTableView.columnAutoresizingStyle = NSTableViewLastColumnOnlyAutoresizingStyle;
    [argumentsTableView setAccessibilityLabel:@(translator._("Arguments"))];
    NSArray<NSString*>* columnIdentifiers = @[ @"name", @"step", @"executable", @"args" ];
    NSArray<NSString*>* columnTitles = @[
        @(translator._("Name")),
        @(translator._("Post Processor")),
        @(translator._("Executable")),
        @(translator._("Args")),
    ];
    NSArray<NSNumber*>* columnWidths = @[ @100, @130, @100, @135 ];
    for (NSUInteger i{ 0 }; i < columnIdentifiers.count; i++)
    {
        NSTableColumn* column{ [[NSTableColumn alloc] initWithIdentifier:columnIdentifiers[i]] };
        column.title = columnTitles[i];
        column.minWidth = 60;
        column.width = columnWidths[i].doubleValue;
        column.resizingMask = i == columnIdentifiers.count - 1
                                      ? NSTableColumnUserResizingMask | NSTableColumnAutoresizingMask
                                      : NSTableColumnUserResizingMask;
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
    [argumentsSegmentedControl setToolTip:@(translator._("Add")) forSegment:0];
    [argumentsSegmentedControl setToolTip:@(translator._("Remove")) forSegment:1];
    [argumentsSegmentedControl setWidth:32 forSegment:0];
    [argumentsSegmentedControl setWidth:32 forSegment:1];
    [argumentsSegmentedControl setEnabled:NO forSegment:1];
    self.argumentsSegmentedControl = argumentsSegmentedControl;
    NSMenu* argumentsMenu{ [[NSMenu alloc] init] };
    NSMenuItem* editMenuItem
    {
        [argumentsMenu addItemWithTitle:@(translator._("Edit…")) action:@selector(editArgument:) keyEquivalent:@""],
    };
    editMenuItem.target = self;
    [argumentsMenu addItem:[NSMenuItem separatorItem]];
    NSMenuItem* removeMenuItem
    {
        [argumentsMenu addItemWithTitle:@(translator._("Remove")) action:@selector(removeArgument:) keyEquivalent:@""],
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

- (NSInteger)rowForSender:(id)sender
{
    return (sender == self.argumentsTableView || [sender isKindOfClass:[NSMenuItem class]]) &&
                           self.argumentsTableView.clickedRow >= 0
                   ? self.argumentsTableView.clickedRow
                   : self.argumentsTableView.selectedRow;
}

- (void)editArgument:(id)sender
{
    NSInteger row{ [self rowForSender:sender] };
    if (row >= 0 && std::cmp_less(row, [[SettingsModel sharedModel] argumentCount]))
    {
        [self presentArgumentEditorForRow:row];
    }
}

- (void)removeArgument:(id)sender
{
    SettingsModel* model{ [SettingsModel sharedModel] };
    NSInteger row{ [self rowForSender:sender] };
    if (row < 0 || std::cmp_greater_equal(row, [model argumentCount]))
    {
        return;
    }
    [model removeArgumentAtIndex:static_cast<NSUInteger>(row)];
    [self.argumentsTableView removeRowsAtIndexes:[NSIndexSet indexSetWithIndex:static_cast<NSUInteger>(row)]
                                   withAnimation:NSTableViewAnimationEffectFade];
    if ([model argumentCount] > 0)
    {
        [self.argumentsTableView selectRowIndexes:[NSIndexSet indexSetWithIndex:std::min(static_cast<NSUInteger>(row),
                                                                                         [model argumentCount] - 1)]
                             byExtendingSelection:NO];
    }
}

- (void)deleteBackward:(id)sender
{
    [self removeArgument:sender];
}

- (void)presentArgumentEditorForRow:(NSInteger)row
{
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    SettingsModel* model{ [SettingsModel sharedModel] };
    NSMutableArray<NSString*>* existingNames{ [NSMutableArray array] };
    for (NSUInteger i{ 0 }; i < [model argumentCount]; i++)
    {
        if (std::cmp_not_equal(i, row))
        {
            [existingNames addObject:[model argumentNameAtIndex:i]];
        }
    }
    NSString* name{ @"" };
    NSInteger step{ 0 };
    NSInteger executable{ 0 };
    NSString* args{ @"" };
    if (row >= 0)
    {
        name = [model argumentNameAtIndex:static_cast<NSUInteger>(row)];
        step = [model argumentStepAtIndex:static_cast<NSUInteger>(row)];
        executable = [model argumentExecutableAtIndex:static_cast<NSUInteger>(row)];
        args = [model argumentArgsAtIndex:static_cast<NSUInteger>(row)];
    }
    SettingsPostprocessingArgumentEditorViewController* editor = [[SettingsPostprocessingArgumentEditorViewController
            alloc]
             initWithName:name
                     step:step
               executable:executable
                     args:args
            existingNames:existingNames
               completion:^(NSString* newName, NSInteger newStep, NSInteger newExecutable, NSString* newArgs) {
                 if (row >= 0 && std::cmp_greater_equal(row, [model argumentCount]))
                 {
                     return;
                 }
                 if (row < 0)
                 {
                     [model addArgumentWithName:newName step:newStep executable:newExecutable args:newArgs];
                 }
                 else
                 {
                     [model replaceArgumentAtIndex:static_cast<NSUInteger>(row)
                                          withName:newName
                                              step:newStep
                                        executable:newExecutable
                                              args:newArgs];
                 }
                 NSUInteger selectedIndex{ row < 0 ? [model argumentCount] - 1 : static_cast<NSUInteger>(row) };
                 if (row < 0)
                 {
                     [self.argumentsTableView insertRowsAtIndexes:[NSIndexSet indexSetWithIndex:selectedIndex]
                                                    withAnimation:NSTableViewAnimationSlideDown];
                 }
                 else
                 {
                     [self.argumentsTableView
                             reloadDataForRowIndexes:[NSIndexSet indexSetWithIndex:selectedIndex]
                                       columnIndexes:[NSIndexSet
                                                             indexSetWithIndexesInRange:
                                                                     NSMakeRange(0,
                                                                                 static_cast<NSUInteger>(
                                                                                         self.argumentsTableView
                                                                                                 .numberOfColumns))]];
                 }
                 [self.argumentsTableView selectRowIndexes:[NSIndexSet indexSetWithIndex:selectedIndex]
                                      byExtendingSelection:NO];
                 [self.argumentsTableView scrollRowToVisible:static_cast<NSInteger>(selectedIndex)];
               }];
    editor.title = @(row < 0 ? translator._("Add Argument") : translator._("Edit Argument"));
    [self presentViewControllerAsSheet:editor];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView
{
    return static_cast<NSInteger>([[SettingsModel sharedModel] argumentCount]);
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
    SettingsModel* model{ [SettingsModel sharedModel] };
    NSString* value{ @"" };
    if ([tableColumn.identifier isEqualToString:@"name"])
    {
        value = [model argumentNameAtIndex:static_cast<NSUInteger>(row)];
    }
    else if ([tableColumn.identifier isEqualToString:@"step"])
    {
        NSInteger step{ [model argumentStepAtIndex:static_cast<NSUInteger>(row)] };
        value = step >= 0 && std::cmp_less(step, self.stepTitles.count) ? self.stepTitles[static_cast<NSUInteger>(step)]
                                                                        : @"";
    }
    else if ([tableColumn.identifier isEqualToString:@"executable"])
    {
        NSInteger executable{ [model argumentExecutableAtIndex:static_cast<NSUInteger>(row)] };
        value = executable >= 0 && std::cmp_less(executable, self.executableTitles.count)
                        ? self.executableTitles[static_cast<NSUInteger>(executable)]
                        : @"";
    }
    else
    {
        value = [model argumentArgsAtIndex:static_cast<NSUInteger>(row)];
    }
    cell.textField.stringValue = value;
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
