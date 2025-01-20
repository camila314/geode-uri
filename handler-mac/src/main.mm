#import <Cocoa/Cocoa.h>
#include <geode-ipc.hpp>
#include <filesystem>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation AppDelegate

- (void)applicationWillFinishLaunching:(NSNotification *)notification {
    NSAppleEventManager *appleEventManager = [NSAppleEventManager sharedAppleEventManager];
    [appleEventManager setEventHandler:self
                       andSelector:@selector(handleGetURLEvent:withReplyEvent:)
                       forEventClass:kInternetEventClass
                       andEventID:kAEGetURL];
}

- (void)handleGetURLEvent:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent {
    NSString* uriString = [[event paramDescriptorForKeyword:keyDirectObject] stringValue];
    if ([uriString hasPrefix:@"geode://"]) {
        uriString = [uriString substringFromIndex:8];
    } else if ([uriString hasPrefix:@"geode:"]) {
        uriString = [uriString substringFromIndex:6];
    } else {
        return;
    }

    if (!GeodeIPC::send("geode.loader", "ipc-test", {})) {
        // launch gd
        auto locPath = std::filesystem::path([[NSBundle mainBundle] bundlePath].UTF8String).parent_path() / ".gd-loc";

        if (!std::filesystem::exists(locPath)) {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert setMessageText:@"Unable to find Geometry Dash."];
            [alert runModal];
            [NSApp terminate:nil];
        }

        auto contents = [NSString stringWithContentsOfFile:[NSString stringWithUTF8String:locPath.c_str()]
                                  encoding:NSUTF8StringEncoding
                                  error:nil];

        auto unescaped = [uriString stringByRemovingPercentEncoding];
        auto escaped = [unescaped stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLPathAllowedCharacterSet]];

        auto arg = [NSString stringWithFormat:@"--geode:uri-path=%@", escaped];
        [[NSWorkspace sharedWorkspace] launchApplicationAtURL: [NSURL fileURLWithPath:contents]
                                       options: NSWorkspaceLaunchDefault
                                        configuration: @{NSWorkspaceLaunchConfigurationArguments: @[arg]}
                                        error: nil];

        [NSApp terminate:nil];
    }

    if (!GeodeIPC::send("camila314.geode-uri", "handle", uriString.UTF8String)) {
        NSLog(@"Failed to send IPC message");

        NSAlert *alert = [[NSAlert alloc] init];
        [alert setMessageText:@"Failed to communicate."];
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
