#import "main_menu_controller.h"
#include "application.h"

@implementation MainMenuController

- (instancetype)init
{
    self = [super init];
    if (self != nil)
    {
        self.settingsDialogController = [[SettingsDialogController alloc] init];
    }
    return self;
}

- (void)setup
{
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    NSApplication* app{ [NSApplication sharedApplication] };
    app.nextResponder = self;
    NSMenu* main_menu{ [[NSMenu alloc] initWithTitle:@"MainMenu"] };
    NSMenuItem* app_menu_item{ [[NSMenuItem alloc] init] };
    [main_menu addItem:app_menu_item];
    NSMenu* app_menu{ [[NSMenu alloc] initWithTitle:@"App"] };
    [app_menu addItemWithTitle:@(translator._("About Parabolic"))
                        action:@selector(orderFrontStandardAboutPanel:)
                 keyEquivalent:@""];
    [app_menu addItemWithTitle:@(translator._("Check for Updates…"))
                        action:@selector(checkForUpdates:)
                 keyEquivalent:@""];
    [app_menu addItem:[NSMenuItem separatorItem]];
    NSMenuItem* settings_item
    {
        [app_menu addItemWithTitle:@(translator._("Settings…")) action:@selector(settings:) keyEquivalent:@","],
    };
    settings_item.image = [NSImage imageWithSystemSymbolName:@"gear" accessibilityDescription:nil];
    if (@available(macOS 27.0, *))
    {
        settings_item.preferredImageVisibility = NSMenuItemImageVisibilityVisible;
    }
    [app_menu addItem:[NSMenuItem separatorItem]];
    NSMenuItem* services_item
    {
        [app_menu addItemWithTitle:@(translator._("Services")) action:nil keyEquivalent:@""],
    };
    NSMenu* services_menu
    {
        [[NSMenu alloc] initWithTitle:@(translator._("Services"))],
    };
    services_item.submenu = services_menu;
    app.servicesMenu = services_menu;
    [app_menu addItem:[NSMenuItem separatorItem]];
    [app_menu addItemWithTitle:@(translator._("Hide Parabolic")) action:@selector(hide:) keyEquivalent:@"h"];
    NSMenuItem* hide_others_item
    {
        [app_menu addItemWithTitle:@(translator._("Hide Others"))
                            action:@selector(hideOtherApplications:)
                     keyEquivalent:@"h"],
    };
    hide_others_item.keyEquivalentModifierMask = NSEventModifierFlagCommand | NSEventModifierFlagOption;
    [app_menu addItemWithTitle:@(translator._("Show All")) action:@selector(unhideAllApplications:) keyEquivalent:@""];
    [app_menu addItem:[NSMenuItem separatorItem]];
    [app_menu addItemWithTitle:@(translator._("Quit Parabolic")) action:@selector(terminate:) keyEquivalent:@"q"];
    app_menu_item.submenu = app_menu;
    NSMenuItem* file_menu_item{ [[NSMenuItem alloc] init] };
    [main_menu addItem:file_menu_item];
    NSMenu* file_menu
    {
        [[NSMenu alloc] initWithTitle:@(translator._("File"))],
    };
    file_menu_item.submenu = file_menu;
    NSMenuItem* edit_menu_item{ [[NSMenuItem alloc] init] };
    [main_menu addItem:edit_menu_item];
    NSMenu* edit_menu
    {
        [[NSMenu alloc] initWithTitle:@(translator._("Edit"))],
    };
    [edit_menu addItemWithTitle:@(translator._("Undo")) action:@selector(undo:) keyEquivalent:@"z"];
    NSMenuItem* redo_item
    {
        [edit_menu addItemWithTitle:@(translator._("Redo")) action:@selector(redo:) keyEquivalent:@"Z"],
    };
    redo_item.keyEquivalentModifierMask = NSEventModifierFlagCommand | NSEventModifierFlagShift;
    [edit_menu addItem:[NSMenuItem separatorItem]];
    [edit_menu addItemWithTitle:@(translator._("Cut")) action:@selector(cut:) keyEquivalent:@"x"];
    [edit_menu addItemWithTitle:@(translator._("Copy")) action:@selector(copy:) keyEquivalent:@"c"];
    [edit_menu addItemWithTitle:@(translator._("Paste")) action:@selector(paste:) keyEquivalent:@"v"];
    [edit_menu addItemWithTitle:@(translator._("Delete")) action:@selector(delete:) keyEquivalent:@""];
    [edit_menu addItemWithTitle:@(translator._("Select All")) action:@selector(selectAll:) keyEquivalent:@"a"];
    edit_menu_item.submenu = edit_menu;
    NSMenuItem* view_menu_item{ [[NSMenuItem alloc] init] };
    [main_menu addItem:view_menu_item];
    NSMenu* view_menu
    {
        [[NSMenu alloc] initWithTitle:@(translator._("View"))],
    };
    NSMenuItem* full_screen_item
    {
        [view_menu addItemWithTitle:@(translator._("Enter Full Screen"))
                             action:@selector(toggleFullScreen:)
                      keyEquivalent:@"f"],
    };
    full_screen_item.keyEquivalentModifierMask = NSEventModifierFlagCommand | NSEventModifierFlagControl;
    view_menu_item.submenu = view_menu;
    NSMenuItem* window_menu_item{ [[NSMenuItem alloc] init] };
    [main_menu addItem:window_menu_item];
    NSMenu* window_menu
    {
        [[NSMenu alloc] initWithTitle:@(translator._("Window"))],
    };
    [window_menu addItemWithTitle:@(translator._("Minimize")) action:@selector(performMiniaturize:) keyEquivalent:@"m"];
    [window_menu addItemWithTitle:@(translator._("Zoom")) action:@selector(performZoom:) keyEquivalent:@""];
    [window_menu addItem:[NSMenuItem separatorItem]];
    [window_menu addItemWithTitle:@(translator._("Bring All to Front"))
                           action:@selector(arrangeInFront:)
                    keyEquivalent:@""];
    window_menu_item.submenu = window_menu;
    app.windowsMenu = window_menu;
    NSMenuItem* help_menu_item{ [[NSMenuItem alloc] init] };
    [main_menu addItem:help_menu_item];
    NSMenu* help_menu
    {
        [[NSMenu alloc] initWithTitle:@(translator._("Help"))],
    };
    [help_menu addItemWithTitle:@(translator._("What's New?")) action:@selector(whatsNew:) keyEquivalent:@""];
    [help_menu addItem:[NSMenuItem separatorItem]];
    [help_menu addItemWithTitle:@(translator._("GitHub Repository…"))
                         action:@selector(gitHubRepository:)
                  keyEquivalent:@""];
    [help_menu addItemWithTitle:@(translator._("Report an Issue…")) action:@selector(reportAnIssue:) keyEquivalent:@""];
    [help_menu addItemWithTitle:@(translator._("Discussions…")) action:@selector(discussions:) keyEquivalent:@""];
    help_menu_item.submenu = help_menu;
    app.helpMenu = help_menu;
    app.mainMenu = main_menu;
}

- (void)checkForUpdates:(id)sender
{
    NSAlert* alert{ [[NSAlert alloc] init] };
    alert.messageText = @"Check for Updates";
    alert.informativeText = @"This feature is not yet implemented.";
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}

- (void)discussions:(id)sender
{
    NSURL* url
    {
        [NSURL URLWithString:@(parabolic::application::State::instance().product().discussions_url().c_str())],
    };
    if (url != nil)
    {
        [[NSWorkspace sharedWorkspace] openURL:url];
    }
}

- (void)gitHubRepository:(id)sender
{
    NSURL* url
    {
        [NSURL URLWithString:@(parabolic::application::State::instance().product().repo_url().c_str())],
    };
    if (url != nil)
    {
        [[NSWorkspace sharedWorkspace] openURL:url];
    }
}

- (void)reportAnIssue:(id)sender
{
    NSURL* url
    {
        [NSURL URLWithString:@(parabolic::application::State::instance().product().issues_url().c_str())],
    };
    if (url != nil)
    {
        [[NSWorkspace sharedWorkspace] openURL:url];
    }
}

- (void)settings:(id)sender
{
    [self.settingsDialogController show];
}

- (void)whatsNew:(id)sender
{
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    NSAlert* alert{ [[NSAlert alloc] init] };
    alert.messageText = @(translator._("What's New?"));
    alert.informativeText = @(parabolic::application::State::instance().product().changelog().c_str());
    [alert addButtonWithTitle:@(translator._("OK"))];
    [alert runModal];
}

@end
