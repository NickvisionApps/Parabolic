#import <Cocoa/Cocoa.h>
#import "delegate.h"

int main()
{
    @autoreleasepool {
        NSApplication* app{ [NSApplication sharedApplication] };
        app.activationPolicy = NSApplicationActivationPolicyRegular;
        AppDelegate* delegate{ [[AppDelegate alloc] init] };
        app.delegate = delegate;
        [app run];
    }
}