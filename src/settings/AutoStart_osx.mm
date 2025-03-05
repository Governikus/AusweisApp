/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
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
	SMAppService* appService = [SMAppService loginItemServiceWithIdentifier: autostartBundleIdentifier];

	if (!appService)
	{
		qCCritical(settings) << "Unable to get status of AutoStartHelper app.";
		return false;
	}

	return appService.status == SMAppServiceStatusEnabled;
}


bool AutoStart::isSetByAdmin()
{
	return false;
}


bool AutoStart::setInternal(bool pEnabled)
{
	SMAppService* appService = [SMAppService loginItemServiceWithIdentifier: autostartBundleIdentifier];

	if (!appService)
	{
		qCCritical(settings) << "Unable to get status of AutoStartHelper app.";
		return false;
	}

	NSError* error = nil;
	const bool result = pEnabled ? [appService registerAndReturnError: &error] : [appService unregisterAndReturnError: &error];
	if (!result)
	{
		qCCritical(settings) << "Setting autostart to" << pEnabled << "failed.";
		if (error)
		{
			qCCritical(settings) << "Autostart registration error message:" << error.localizedDescription;
		}
		return false;
	}

	qCDebug(settings) << "Setting autostart to" << pEnabled << "succeeded";
	return true;
}
