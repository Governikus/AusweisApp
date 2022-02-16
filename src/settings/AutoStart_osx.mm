/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "AutoStart.h"

#include <QLoggingCategory>
#include <QScopeGuard>

#import <Cocoa/Cocoa.h>
#import <ServiceManagement/ServiceManagement.h>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(settings)

static NSString* autostartBundleIdentifier = @"com.governikus.AusweisApp2.AutostartHelper";


bool AutoStart::enabled()
{
	auto jobDictionaries = static_cast<NSArray*>(SMCopyAllJobDictionaries(kSMDomainUserLaunchd));
	if (jobDictionaries == nullptr)
	{
		qCCritical(settings) << "Getting autostart entries failed";
		return false;
	}

	const auto dictFree = qScopeGuard([jobDictionaries] {
			CFRelease(jobDictionaries);
		});

	for (NSDictionary* job in jobDictionaries)
	{
		if ([autostartBundleIdentifier isEqualToString:job[@"Label"]] == YES)
		{
			qCDebug(settings) << "Autostart entry found";
			return true;
		}
	}

	qCDebug(settings) << "No autostart entry found";
	return false;
}


bool AutoStart::isSetByAdmin()
{
	return false;
}


bool AutoStart::setInternal(bool pEnabled)
{
	if (SMLoginItemSetEnabled(static_cast<CFStringRef>(autostartBundleIdentifier), pEnabled))
	{
		qCDebug(settings) << "Setting autostart to" << pEnabled << "succeeded";
		return true;
	}

	qCCritical(settings) << "Setting autostart failed";
	return false;
}
