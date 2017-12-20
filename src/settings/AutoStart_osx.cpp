/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "AutoStart.h"

#include <QCoreApplication>
#include <QDebug>
#include <QRegularExpression>

#import <Cocoa/Cocoa.h>


using namespace governikus;


static bool checkAndRemoveAutoStart(bool pRemove)
{
	qDebug() << "Loading OSX login items";

	QRegularExpression regex("/Contents/Resources$");
	NSString* appPath = QCoreApplication::applicationDirPath().remove(regex).toNSString();
	CFURLRef url = static_cast<CFURLRef>([NSURL fileURLWithPath: appPath]);

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
			if (LSSharedFileListItemResolve(itemRef, 0, static_cast<CFURLRef*>(&url), NULL) == noErr)
			{
				NSURL* nsUrl = static_cast<NSURL*>(url);
				NSString* urlPath = [nsUrl path];
				if ([urlPath compare : appPath] == NSOrderedSame)
				{
					if (pRemove)
					{
						LSSharedFileListItemRemove(loginItems, itemRef);
					}

					return true;
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
						qDebug() << "Removed the unresolvable application with diplay name" << displayName << "from OSX login items.";
					}
				}
			}
		}
		[loginItemsArray release];
	}

	return false;
}


bool AutoStart::enabled()
{
	return checkAndRemoveAutoStart(false);
}


void AutoStart::set(bool pEnabled)
{
	if (pEnabled)
	{
		QRegularExpression regex("/Contents/Resources$");
		NSString* path = QCoreApplication::applicationDirPath().remove(regex).toNSString();
		CFURLRef url = static_cast<CFURLRef>([NSURL fileURLWithPath: path]);

		LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL, kLSSharedFileListSessionLoginItems, NULL);
		if (loginItems)
		{
			LSSharedFileListItemRef item = LSSharedFileListInsertItemURL(loginItems, kLSSharedFileListItemLast, NULL, NULL, url, NULL, NULL);
			if (item)
			{
				CFRelease(item);
				return;
			}
		}
		return;
	}

	checkAndRemoveAutoStart(true);
}
