/*
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include <AppKit/AppKit.h>
#include <Foundation/Foundation.h>

@interface AutostartDelegate
	: NSObject<NSApplicationDelegate>
- (void) applicationWillFinishLaunching: (NSNotification*) pNotification;
@end

@implementation AutostartDelegate
- (void) applicationWillFinishLaunching: (NSNotification*) pNotification
{
	(void) pNotification;
	BOOL alreadyRunning = false;

	NSArray* runningApplications = [[NSWorkspace sharedWorkspace] runningApplications];
	for (NSRunningApplication* application in runningApplications)
	{
		if ([[application bundleIdentifier] isEqualToString:@"com.governikus.AusweisApp2"])
		{
			NSLog(@"Application is already running.");
			alreadyRunning = true;
		}
	}

	if (!alreadyRunning)
	{
		NSString* helperBundlePath = [[NSBundle mainBundle] bundlePath];
		NSArray* helperBundlePathComponents = [helperBundlePath pathComponents];
		// Remove last 4 components from helper path to get main path, current bundle is located at "main.app/Contents/Library/LoginItems/helper.app":
		NSArray* mainBundleComponents = [helperBundlePathComponents subarrayWithRange:NSMakeRange(0, [helperBundlePathComponents count] - 4)];
		NSString* mainBundlePath = [NSString pathWithComponents:mainBundleComponents];
		NSLog(@"Launching application at: %@", mainBundlePath);
		BOOL result = [[NSWorkspace sharedWorkspace] launchApplication:mainBundlePath];
		if (!result)
		{
			NSLog(@"Launching failed");
		}
	}
	[NSApp terminate:nil];
}


@end

int main(int argc, const char** argv)
{
	[NSApplication sharedApplication];
	[NSApp setDelegate: [AutostartDelegate new]];

	return NSApplicationMain(argc, argv);
}
