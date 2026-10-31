#import "menu.h"
#include "application.h"

@implementation MainMenuController

- (void)setup {
    const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
    NSMenu* main_menu{ [[NSMenu alloc] initWithTitle:@"MainMenu"] };
    NSMenuItem* app_menu_item{ [[NSMenuItem alloc] init] };
    [main_menu addItem:app_menu_item];
    NSMenu* app_menu{ [[NSMenu alloc] initWithTitle:@"App"] };
    [app_menu addItemWithTitle:@(translator._("About Parabolic")) action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
    NSMenuItem* update_item{ [app_menu addItemWithTitle:@(translator._("Check for Updates...")) action:@selector(checkForUpdates:) keyEquivalent:@""] };
    update_item.target = self;
    [app_menu addItem:[NSMenuItem separatorItem]];
    NSMenuItem* settings_item{ [app_menu addItemWithTitle:@(translator._("Settings...")) action:@selector(settings:) keyEquivalent:@","] };
    settings_item.target = self;
    [app_menu addItem:[NSMenuItem separatorItem]];
    [app_menu addItemWithTitle:@(translator._("Quit Parabolic")) action:@selector(terminate:) keyEquivalent:@"q"];
    app_menu_item.submenu = app_menu;
    NSApplication* app{ [NSApplication sharedApplication] };
    app.mainMenu = main_menu;
}

- (void)checkForUpdates:(id)sender {
    NSAlert* alert{ [[NSAlert alloc] init] };
    alert.messageText = @"Check for Updates";
    alert.informativeText = @"This feature is not yet implemented.";
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}

- (void)settings:(id)sender {
    NSAlert* alert{ [[NSAlert alloc] init] };
    alert.messageText = @"Settings";
    alert.informativeText = @"This feature is not yet implemented.";
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}

@end