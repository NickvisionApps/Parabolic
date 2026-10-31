#import <Cocoa/Cocoa.h>
#import "delegate.h"

int main()
{
    @autoreleasepool {
        NSApplication* app{ [NSApplication sharedApplication] };
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        AppDelegate* delegate{ [[AppDelegate alloc] init] };
        [app setDelegate:delegate];
        [app run];
    }
}