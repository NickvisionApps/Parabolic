#import "settings_general_view_controller.h"
#include <string>
#include "application.h"
#include "configuration.h"
#include "translation.h"
#include "update.h"

@implementation SettingsGeneralViewController

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
    self.title = @(translator._p("Settings", "General"));
    NSTextField* themeLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Theme")).c_str())],
    };
    themeLabel.translatesAutoresizingMaskIntoConstraints = NO;
    themeLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* themePopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    themePopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    [themePopUpButton addItemsWithTitles:@[
        @(translator._p("Theme", "Light")),
        @(translator._p("Theme", "Dark")),
        @(translator._p("Theme", "System")),
    ]];
    [[themePopUpButton itemAtIndex:0] setTag:static_cast<NSInteger>(parabolic::configuration::Theme::Light)];
    [[themePopUpButton itemAtIndex:1] setTag:static_cast<NSInteger>(parabolic::configuration::Theme::Dark)];
    [[themePopUpButton itemAtIndex:2] setTag:static_cast<NSInteger>(parabolic::configuration::Theme::System)];
    [themePopUpButton selectItemWithTag:static_cast<NSInteger>(configuration.theme())];
    themePopUpButton.target = self;
    themePopUpButton.action = @selector(themeChanged:);
    NSTextField* languageLabel
    {
        [NSTextField labelWithString:@(translator._("{}:", translator._("Translation Language")).c_str())],
    };
    languageLabel.translatesAutoresizingMaskIntoConstraints = NO;
    languageLabel.alignment = NSTextAlignmentRight;
    NSPopUpButton* languagePopUpButton{ [[NSPopUpButton alloc] initWithFrame:NSZeroRect pullsDown:NO] };
    languagePopUpButton.translatesAutoresizingMaskIntoConstraints = NO;
    NSMenuItem* followSystemLanguageItem
    {
        [[NSMenuItem alloc] initWithTitle:@(translator._p("Language", "System")) action:nil keyEquivalent:@""],
    };
    followSystemLanguageItem.representedObject =
            @(parabolic::translation::Translator::follow_system_language().c_str());
    [languagePopUpButton.menu addItem:followSystemLanguageItem];
    NSLocale* currentLocale{ [NSLocale currentLocale] };
    for (const std::string& language : translator.available_languages())
    {
        NSString* identifier
        {
            @(language.c_str()),
        };
        NSString* displayName{ [currentLocale localizedStringForLocaleIdentifier:identifier] };
        NSMenuItem* languageItem{
            [[NSMenuItem alloc] initWithTitle:displayName.length > 0 ? displayName : identifier
                                       action:nil
                                keyEquivalent:@""],
        };
        languageItem.representedObject = identifier;
        [languagePopUpButton.menu addItem:languageItem];
    }
    NSString* currentLanguage
    {
        @(configuration.translation_options().language().c_str()),
    };
    for (NSMenuItem* languageItem in languagePopUpButton.itemArray)
    {
        if ([languageItem.representedObject isEqualToString:currentLanguage])
        {
            [languagePopUpButton selectItem:languageItem];
            break;
        }
    }
    languagePopUpButton.target = self;
    languagePopUpButton.action = @selector(languageChanged:);
    NSTextField* languageSubtitleLabel
    {
        [NSTextField wrappingLabelWithString:@(translator._("An application restart is required for a change to take "
                                                            "effect."))],
    };
    languageSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    languageSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    languageSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    NSButton* autoUpdateCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Automatically Check for Updates"))
                             target:self
                             action:@selector(autoUpdateChanged:)],
    };
    autoUpdateCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    autoUpdateCheckbox.state =
            configuration.automatically_check_for_updates() ? NSControlStateValueOn : NSControlStateValueOff;
    NSButton* allowPreviewUpdatesCheckbox
    {
        [NSButton checkboxWithTitle:@(translator._("Allow Preview Updates"))
                             target:self
                             action:@selector(allowPreviewUpdatesChanged:)],
    };
    allowPreviewUpdatesCheckbox.translatesAutoresizingMaskIntoConstraints = NO;
    allowPreviewUpdatesCheckbox.state =
            configuration.update_options().allow_preview_updates() ? NSControlStateValueOn : NSControlStateValueOff;
    NSTextField* allowPreviewUpdatesSubtitleLabel
    {
        [NSTextField
                wrappingLabelWithString:@(translator._(
                                                "Update the app and dependencies, such as yt-dlp, to beta versions."))],
    };
    allowPreviewUpdatesSubtitleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    allowPreviewUpdatesSubtitleLabel.font = [NSFont systemFontOfSize:NSFont.smallSystemFontSize];
    allowPreviewUpdatesSubtitleLabel.textColor = NSColor.secondaryLabelColor;
    [self.view addSubview:themeLabel];
    [self.view addSubview:themePopUpButton];
    [self.view addSubview:languageLabel];
    [self.view addSubview:languagePopUpButton];
    [self.view addSubview:languageSubtitleLabel];
    [self.view addSubview:autoUpdateCheckbox];
    [self.view addSubview:allowPreviewUpdatesCheckbox];
    [self.view addSubview:allowPreviewUpdatesSubtitleLabel];
    [NSLayoutConstraint activateConstraints:@[
        [self.view.widthAnchor constraintEqualToConstant:700],
        [themeLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [themeLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [themePopUpButton.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [themePopUpButton.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [themePopUpButton.widthAnchor constraintGreaterThanOrEqualToConstant:100],
        [themeLabel.firstBaselineAnchor constraintEqualToAnchor:themePopUpButton.firstBaselineAnchor],
        [themePopUpButton.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [languageLabel.trailingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:-4],
        [languageLabel.leadingAnchor constraintGreaterThanOrEqualToAnchor:self.view.leadingAnchor constant:20],
        [languagePopUpButton.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [languagePopUpButton.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [languagePopUpButton.widthAnchor constraintGreaterThanOrEqualToConstant:100],
        [languageLabel.firstBaselineAnchor constraintEqualToAnchor:languagePopUpButton.firstBaselineAnchor],
        [languagePopUpButton.topAnchor constraintEqualToAnchor:themePopUpButton.bottomAnchor constant:12],
        [languageSubtitleLabel.leadingAnchor constraintEqualToAnchor:languagePopUpButton.leadingAnchor],
        [languageSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [languageSubtitleLabel.topAnchor constraintEqualToAnchor:languagePopUpButton.bottomAnchor constant:6],
        [autoUpdateCheckbox.leadingAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:4],
        [autoUpdateCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor constant:-20],
        [autoUpdateCheckbox.topAnchor constraintEqualToAnchor:languageSubtitleLabel.bottomAnchor constant:16],
        [allowPreviewUpdatesCheckbox.leadingAnchor constraintEqualToAnchor:autoUpdateCheckbox.leadingAnchor],
        [allowPreviewUpdatesCheckbox.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                             constant:-20],
        [allowPreviewUpdatesCheckbox.topAnchor constraintEqualToAnchor:autoUpdateCheckbox.bottomAnchor constant:8],
        [allowPreviewUpdatesSubtitleLabel.leadingAnchor
                constraintEqualToAnchor:allowPreviewUpdatesCheckbox.leadingAnchor],
        [allowPreviewUpdatesSubtitleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:self.view.trailingAnchor
                                                                                  constant:-20],
        [allowPreviewUpdatesSubtitleLabel.topAnchor constraintEqualToAnchor:allowPreviewUpdatesCheckbox.bottomAnchor
                                                                   constant:6],
        [allowPreviewUpdatesSubtitleLabel.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
    ]];
}

- (void)themeChanged:(NSPopUpButton*)sender
{
    parabolic::configuration::Theme theme{ static_cast<parabolic::configuration::Theme>(sender.selectedItem.tag) };
    parabolic::application::State::instance().configuration().set_theme(theme);
    switch (theme)
    {
    case parabolic::configuration::Theme::Light:
        NSApp.appearance = [NSAppearance appearanceNamed:NSAppearanceNameAqua];
        break;
    case parabolic::configuration::Theme::Dark:
        NSApp.appearance = [NSAppearance appearanceNamed:NSAppearanceNameDarkAqua];
        break;
    case parabolic::configuration::Theme::System:
    default:
        NSApp.appearance = nil;
        break;
    }
}

- (void)languageChanged:(NSPopUpButton*)sender
{
    NSString* identifier{ sender.selectedItem.representedObject };
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::translation::Options options{ configuration.translation_options() };
    options.set_language(identifier.UTF8String);
    configuration.set_translation_options(options);
}

- (void)autoUpdateChanged:(NSButton*)sender
{
    parabolic::application::State::instance().configuration().set_automatically_check_for_updates(
            sender.state == NSControlStateValueOn);
}

- (void)allowPreviewUpdatesChanged:(NSButton*)sender
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::update::Options options{ configuration.update_options() };
    options.set_allow_preview_updates(sender.state == NSControlStateValueOn);
    configuration.set_update_options(options);
}

@end
