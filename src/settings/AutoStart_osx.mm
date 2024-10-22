/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
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
	QT_WARNING_PUSH
	QT_WARNING_DISABLE_DEPRECATED
	// Allow warning for SMCopyAllJobDictionaries, Apple will provide an alternative, until then we are stuck with this deprecated method
	auto jobDictionaries = static_cast<NSArray*>(SMCopyAllJobDictionaries(kSMDomainUserLaunchd));
	QT_WARNING_POP

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
	QT_WARNING_PUSH
	QT_WARNING_DISABLE_DEPRECATED
	const bool callSucceeded = SMLoginItemSetEnabled(static_cast<CFStringRef>(autostartBundleIdentifier), pEnabled);
	QT_WARNING_POP
	if (callSucceeded)
	{
		qCDebug(settings) << "Setting autostart to" << pEnabled << "succeeded";
		return true;
	}

	qCCritical(settings) << "Setting autostart failed";
	return false;
}
