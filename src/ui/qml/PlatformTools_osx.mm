/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "PlatformTools.h"

#include <QDebug>
#include <QLoggingCategory>

#include <functional>

#import <Cocoa/Cocoa.h>
#import <UserNotifications/UserNotifications.h>

Q_DECLARE_LOGGING_CATEGORY(gui)

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


void ensureNotificationPermission(std::function<void()> pCallback)
{
#ifdef QT_NO_DEBUG
	if (@available(macOS 10.14, *))
	{
		UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
		[center getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings* _Nonnull settings){
			if (settings.authorizationStatus == UNAuthorizationStatusAuthorized)
			{
				pCallback();
				return;
			}
			const auto& requestedOptions = UNAuthorizationOptionProvidesAppNotificationSettings & UNAuthorizationOptionAlert;
			[center requestAuthorizationWithOptions:requestedOptions completionHandler:^(BOOL granted, NSError* _Nullable error){
				if (error != nil)
				{
					qCDebug(gui) << "Error when requesting authorization:" << error.localizedDescription;
				}
				if (granted)
				{
					pCallback();
				}
			}];
		}];
	}
#else
	Q_UNUSED(pCallback)
#endif
}


void PlatformTools::postNotification(const QString& pTitle, const QString& pMessage)
{
#ifdef QT_NO_DEBUG
	if (@available(macOS 10.14, *))
	{
		ensureNotificationPermission([pTitle, pMessage]{
				UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
				UNMutableNotificationContent* content = [[UNMutableNotificationContent alloc] init];
				content.title = pTitle.toNSString();
				content.body = pMessage.toNSString();
				UNNotificationRequest* request = [UNNotificationRequest requestWithIdentifier:@"AA2Notification" content:content trigger:nil];
				[center addNotificationRequest:request withCompletionHandler:^(NSError* _Nullable error){
					if (error != nil)
					{
						qCDebug(gui) << "Failed to post notification:" << error.localizedDescription;
					}
				}];
			});
	}
#else
	Q_UNUSED(pTitle)
	Q_UNUSED(pMessage)
#endif
}
