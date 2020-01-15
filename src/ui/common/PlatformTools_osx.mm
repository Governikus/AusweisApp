/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "PlatformTools.h"

#import <Cocoa/Cocoa.h>


using namespace governikus;


void PlatformTools::hideFromTaskbar()
{
	ProcessSerialNumber psn = {
		0, kCurrentProcess
	};
	TransformProcessType(&psn, kProcessTransformToBackgroundApplication);
}


void PlatformTools::restoreToTaskbar()
{
	ProcessSerialNumber psn = {
		0, kCurrentProcess
	};
	TransformProcessType(&psn, kProcessTransformToForegroundApplication);
	[NSApp activateIgnoringOtherApps: YES];
}
