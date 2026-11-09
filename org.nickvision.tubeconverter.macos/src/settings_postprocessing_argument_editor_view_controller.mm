#import "settings_postprocessing_argument_editor_view_controller.h"
#include "application.h"
#include "translation.h"

@interface SettingsPostprocessingArgumentEditorViewController () <NSTextFieldDelegate>

@property(nonatomic, copy) NSString* initialName;
@property(nonatomic, assign) NSInteger initialStep;
@property(nonatomic, assign) NSInteger initialExecutable;
@property(nonatomic, copy) NSString* initialArgs;
@property(nonatomic, copy) NSArray<NSString*>* existingNames;
@property(nonatomic, copy) void (^completion)(NSString* name, NSInteger step, NSInteger executable, NSString* args);
@property(nonatomic, strong) NSTextField* nameTextField;
@property(nonatomic, strong) NSTextField* nameErrorLabel;
@property(nonatomic, strong) NSPopUpButton* stepPopUpButton;
@property(nonatomic, strong) NSPopUpButton* executablePopUpButton;
@property(nonatomic, strong) NSTextField* argsTextField;
@property(nonatomic, strong) NSButton* saveButton;

@end

@implementation SettingsPostprocessingArgumentEditorViewController

- (instancetype)initWithName:(NSString*)name
                        step:(NSInteger)step
                  executable:(NSInteger)executable
                        args:(NSString*)args
               existingNames:(NSArray<NSString*>*)existingNames
                  completion:(void (^)(NSString* name, NSInteger step, NSInteger executable, NSString* args))completion
{
    self = [super initWithNibName:nil bundle:nil];
    if (self != nil)
    {
        self.initialName = name;
        self.initialStep = step;
        self.initialExecutable = executable;
        self.initialArgs = args;
        self.existingNames = existingNames;
        self.completion = completion;
    }
    return self;
}

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSZeroRect];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.translatesAutoresizingMaskIntoConstraints = NO;
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    NSTextField* nameLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Name")).c_str())],
    };
    nameLabel.translatesAutoresizingMaskIntoConstraints = NO;
    nameLabel.alignment = NSTextAlignmentRight;
    NSTextField* nameTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    nameTextField.translatesAutoresizingMaskIntoConstraints = NO;
    nameTextField.stringValue = self.initialName;
    nameTextField.delegate = self;
    self.nameTextField = nameTextField;
    NSTextField* nameErrorLabel{ [NSTextField labelWithString:@" "] };
    nameErrorLabel.translatesAutoresizingMaskIntoConstraints = NO;
    nameErrorLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    nameErrorLabel.textColor = NSColor.systemRedColor;
    self.nameErrorLabel = nameErrorLabel;
    NSTextField* stepLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Post Processor")).c_str())],
    };
    stepLabel.translatesAutoresizingMaskIntoConstraints = NO;
    stepLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* stepPopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    stepPopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    [stepPopUpButton addItemsWithTitles:@[
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
    ]];
    for (NSInteger i{ 0 }; i < stepPopUpButton.numberOfItems; i++)
    {
        [[stepPopUpButton itemAtIndex:i] setTag:i];
    }
    [stepPopUpButton selectItemWithTag:self.initialStep];
    self.stepPopUpButton = stepPopUpButton;
    NSTextField* executableLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Executable")).c_str())],
    };
    executableLabel.translatesAutoresizingMaskIntoConstraints = NO;
    executableLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* executablePopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    executablePopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    [executablePopUpButton addItemsWithTitles:@[
        @(translator._("None")),
        @(translator._("AtomicParsley")),
        @(translator._("FFmpeg")),
        @(translator._("FFprobe")),
    ]];
    for (NSInteger i{ 0 }; i < executablePopUpButton.numberOfItems; i++)
    {
        [[executablePopUpButton itemAtIndex:i] setTag:i];
    }
    [executablePopUpButton selectItemWithTag:self.initialExecutable];
    self.executablePopUpButton = executablePopUpButton;
    NSTextField* argsLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Args")).c_str())],
    };
    argsLabel.translatesAutoresizingMaskIntoConstraints = NO;
    argsLabel.alignment = NSTextAlignmentRight;
    NSTextField* argsTextField{ [[NSTextField alloc] initWithFrame:NSZeroRect] };
    argsTextField.translatesAutoresizingMaskIntoConstraints = NO;
    argsTextField.stringValue = self.initialArgs;
    self.argsTextField = argsTextField;
    NSButton* cancelButton
    {
        [NSButton buttonWithTitle:@(translator._("Cancel")) target:self action:@selector(cancel:)],
    };
    cancelButton.translatesAutoresizingMaskIntoConstraints = NO;
    cancelButton.keyEquivalent = @"\033";
    NSButton* saveButton
    {
        [NSButton buttonWithTitle:@(self.initialName.length > 0 ? translator._("Save") : translator._("Add"))
                           target:self
                           action:@selector(save:)],
    };
    saveButton.translatesAutoresizingMaskIntoConstraints = NO;
    saveButton.keyEquivalent = @"\r";
    self.saveButton = saveButton;
    [self.view addSubview:nameLabel];
    [self.view addSubview:nameTextField];
    [self.view addSubview:nameErrorLabel];
    [self.view addSubview:stepLabel];
    [self.view addSubview:stepPopUpButton];
    [self.view addSubview:executableLabel];
    [self.view addSubview:executablePopUpButton];
    [self.view addSubview:argsLabel];
    [self.view addSubview:argsTextField];
    [self.view addSubview:cancelButton];
    [self.view addSubview:saveButton];
    [NSLayoutConstraint activateConstraints:@[
        [self.view.widthAnchor constraintEqualToConstant:480],
        [nameLabel.trailingAnchor constraintEqualToAnchor:self.view.leadingAnchor constant:130],
        [nameLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [nameTextField.leadingAnchor constraintEqualToAnchor:nameLabel.trailingAnchor constant:8],
        [nameTextField.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [nameLabel.firstBaselineAnchor constraintEqualToAnchor:nameTextField.firstBaselineAnchor],
        [nameTextField.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [nameErrorLabel.leadingAnchor constraintEqualToAnchor:nameTextField.leadingAnchor],
        [nameErrorLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [nameErrorLabel.topAnchor constraintEqualToAnchor:nameTextField.bottomAnchor constant:4],
        [stepLabel.trailingAnchor constraintEqualToAnchor:nameLabel.trailingAnchor],
        [stepLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [stepPopUpButton.leadingAnchor constraintEqualToAnchor:nameTextField.leadingAnchor],
        [stepPopUpButton.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [stepLabel.firstBaselineAnchor constraintEqualToAnchor:stepPopUpButton.firstBaselineAnchor],
        [stepPopUpButton.topAnchor constraintEqualToAnchor:nameErrorLabel.bottomAnchor constant:8],
        [executableLabel.trailingAnchor constraintEqualToAnchor:nameLabel.trailingAnchor],
        [executableLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [executablePopUpButton.leadingAnchor constraintEqualToAnchor:nameTextField.leadingAnchor],
        [executablePopUpButton.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [executableLabel.firstBaselineAnchor constraintEqualToAnchor:executablePopUpButton.firstBaselineAnchor],
        [executablePopUpButton.topAnchor constraintEqualToAnchor:stepPopUpButton.bottomAnchor constant:12],
        [argsLabel.trailingAnchor constraintEqualToAnchor:nameLabel.trailingAnchor],
        [argsLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [argsTextField.leadingAnchor constraintEqualToAnchor:nameTextField.leadingAnchor],
        [argsTextField.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [argsLabel.firstBaselineAnchor constraintEqualToAnchor:argsTextField.firstBaselineAnchor],
        [argsTextField.topAnchor constraintEqualToAnchor:executablePopUpButton.bottomAnchor constant:12],
        [saveButton.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [saveButton.topAnchor constraintEqualToAnchor:argsTextField.bottomAnchor constant:24],
        [saveButton.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
        [cancelButton.trailingAnchor constraintEqualToAnchor:saveButton.leadingAnchor constant:-8],
        [cancelButton.centerYAnchor constraintEqualToAnchor:saveButton.centerYAnchor],
    ]];
    [self controlTextDidChange:[NSNotification notificationWithName:NSControlTextDidChangeNotification
                                                             object:nameTextField]];
}

- (void)viewDidAppear
{
    [super viewDidAppear];
    [self.view.window makeFirstResponder:self.nameTextField];
}

- (void)cancel:(NSButton*)sender
{
    [self.presentingViewController dismissViewController:self];
}

- (void)save:(NSButton*)sender
{
    self.completion([self.nameTextField.stringValue
                            stringByTrimmingCharactersInSet:NSCharacterSet.whitespaceAndNewlineCharacterSet],
                    self.stepPopUpButton.selectedItem.tag, self.executablePopUpButton.selectedItem.tag,
                    self.argsTextField.stringValue);
    [self.presentingViewController dismissViewController:self];
}

- (void)controlTextDidChange:(NSNotification*)notification
{
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    NSString* name{
        [self.nameTextField.stringValue
                stringByTrimmingCharactersInSet:NSCharacterSet.whitespaceAndNewlineCharacterSet],
    };
    BOOL isDuplicate{ NO };
    for (NSString* existingName in self.existingNames)
    {
        if ([existingName caseInsensitiveCompare:name] == NSOrderedSame)
        {
            isDuplicate = YES;
            break;
        }
    }
    self.saveButton.enabled = name.length > 0 && !isDuplicate;
    self.nameErrorLabel.stringValue =
            isDuplicate ? @(translator._("An argument with this name already exists.")) : @" ";
}

@end
