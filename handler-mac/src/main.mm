#import <Cocoa/Cocoa.h>
#include <geode-ipc.hpp>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation AppDelegate

- (void)applicationWillFinishLaunching:(NSNotification *)notification {
	NSAppleEventManager *appleEventManager = [NSAppleEventManager sharedAppleEventManager];
	[appleEventManager setEventHandler:self andSelector:@selector(handleGetURLEvent:withReplyEvent:) forEventClass:kInternetEventClass andEventID:kAEGetURL];
}

- (void)handleGetURLEvent:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent {
	NSString* urlString = [[event paramDescriptorForKeyword:keyDirectObject] stringValue];
	if ([urlString hasPrefix:@"geode://"]) {
		urlString = [urlString substringFromIndex:8];
	} else if ([urlString hasPrefix:@"geode:"]) {
		urlString = [urlString substringFromIndex:6];
	} else {
		return;
	}

	if (!GeodeIPC::send("camila314.custom-uri", "handle", urlString.UTF8String)) {
		NSLog(@"Failed to send IPC message");

		NSAlert *alert = [[NSAlert alloc] init];
		[alert setMessageText:@"Failed to communicate. Please make sure Geometry Dash is open."];
		[alert runModal];
	}

	[NSApp terminate:nil];
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
	[NSApp terminate:nil];
}
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        AppDelegate *delegate = [[AppDelegate alloc] init];
        app.delegate = delegate;
        return NSApplicationMain(argc, argv);
    }
}
