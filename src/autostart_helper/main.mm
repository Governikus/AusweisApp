/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
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
		if ([[application bundleIdentifier] isCaseInsensitiveLike:@"com.governikus.ausweisapp2"])
		{
			NSLog(@"Application is already running.");
			alreadyRunning = true;
		}
	}

	if (!alreadyRunning)
	{
		NSString* helperBundlePath = [[NSBundle mainBundle] bundlePath];
		NSArray* helperBundlePathComponents = [helperBundlePath pathComponents];
		// Remove last 4 components from bundle path to get main path, current bundle is located at "AusweisApp.app/Contents/Library/LoginItems/AusweisAppAutostartHelper.app"
		NSArray* mainBundleComponents = [helperBundlePathComponents subarrayWithRange:NSMakeRange(0, [helperBundlePathComponents count] - 4)];
		NSString* applicationUrl = [NSString stringWithFormat:@"file:///%@", [NSString pathWithComponents:mainBundleComponents]];
		NSLog(@"Launching application at: %@", applicationUrl);
		NSWorkspaceOpenConfiguration* config = [[NSWorkspaceOpenConfiguration alloc] init];
		[[NSWorkspace sharedWorkspace] openApplicationAtURL:[NSURL URLWithString:applicationUrl] configuration:config completionHandler:^(NSRunningApplication* app, NSError* error){
			if (error)
			{
				NSLog(@"Launching failed: %@", error.localizedDescription);
			}
			else
			{
				NSLog(@"Started application: %@", app);
			}
			[NSApp terminate:nil];
		}];
	}
}


@end

int main(int argc, const char** argv)
{
	[NSApplication sharedApplication];
	[NSApp setDelegate: [AutostartDelegate new]];

	return NSApplicationMain(argc, argv);
}
