#import "settings_postprocessing_view_controller.h"
#include "application.h"
#include "translation.h"

@implementation SettingsPostprocessingViewController

- (void)loadView
{
    self.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 700, 150)];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.title = @(parabolic::application::State::instance().translator()._p("Settings", "Postprocessing"));
}

@end
