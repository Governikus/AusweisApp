/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "AutoStart.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QRegularExpression>

#import <ServiceManagement/ServiceManagement.h>
#import <Cocoa/Cocoa.h>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(settings)

static void cleanupOldAutoStart()
{
	const QRegularExpression regex(QStringLiteral("/Contents/Resources$"));
	const QRegularExpression regex2(QStringLiteral("/Contents/MacOS$"));
	NSString* appPath = QCoreApplication::applicationDirPath().remove(regex).remove(regex2).toNSString();

	// Create a reference to the shared file list.
	LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL,
			kLSSharedFileListSessionLoginItems, NULL);

	if (loginItems)
	{
		//Retrieve the list of Login Items and cast them to
		// a NSArray so that it will be easier to iterate.
		NSArray* loginItemsArray = static_cast<NSArray*>(LSSharedFileListCopySnapshot(loginItems, NULL));
		for (NSUInteger i = 0; i < [loginItemsArray count]; ++i)
		{
			LSSharedFileListItemRef itemRef = static_cast<LSSharedFileListItemRef>([loginItemsArray objectAtIndex:i]);
			//Resolve the item with URL
			CFURLRef url = LSSharedFileListItemCopyResolvedURL(itemRef, 0, nullptr);
			if (url)
			{
				NSURL* nsUrl = static_cast<NSURL*>(url);
				NSString* urlPath = [nsUrl path];
				if ([urlPath compare : appPath] == NSOrderedSame)
				{
					LSSharedFileListItemRemove(loginItems, itemRef);
					qCDebug(settings) << "Removed old autostart entry";
				}
			}
			else
			{
				const CFStringRef stringRef = LSSharedFileListItemCopyDisplayName(itemRef);
				if (stringRef)
				{
					const QString displayName = QString::fromCFString(stringRef);
					if (displayName.startsWith(QCoreApplication::applicationName()))
					{
						LSSharedFileListItemRemove(loginItems, itemRef);
						qCDebug(settings) << "Removed the unresolvable application with diplay name" << displayName << "from OSX login items.";
					}
				}
			}
		}
		[loginItemsArray release];
	}
}


bool AutoStart::enabled()
{
	cleanupOldAutoStart();
	CFStringRef autostartBundleName = CFSTR("com.governikus.AusweisApp2.AutostartHelper");
	CFStringRef dictionaryKey = CFSTR("Label");
	CFArrayRef jobDictioniaries = SMCopyAllJobDictionaries(kSMDomainUserLaunchd);
	if (jobDictioniaries == nullptr)
	{
		qCCritical(settings) << "Getting autostart entries failed";
		return false;
	}
	for (int i = 0; i < CFArrayGetCount(jobDictioniaries); ++i)
	{
		CFDictionaryRef jobDictionary = static_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(jobDictioniaries, i));
		if (jobDictionary == nullptr)
		{
			continue;
		}

		CFStringRef jobLabel = static_cast<CFStringRef>(CFDictionaryGetValue(jobDictionary, dictionaryKey));
		if (jobLabel != nullptr && CFStringCompare(jobLabel, autostartBundleName, 0) == kCFCompareEqualTo)
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


void AutoStart::set(bool pEnabled)
{
	CFStringRef autostartBundleName = CFSTR("com.governikus.AusweisApp2.AutostartHelper");
	if (SMLoginItemSetEnabled(autostartBundleName, pEnabled))
	{
		qCCritical(settings) << "Setting autostart succeded:" << pEnabled;
	}
	else
	{
		qCCritical(settings) << "Setting autostart failed";
	}
}
