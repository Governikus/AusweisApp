/*!
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#include "Backup.h"

#include <QLoggingCategory>

#if defined(Q_OS_IOS)
	#import <UIKit/UIKit.h>
#else
	#import <AppKit/AppKit.h>
#endif


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(settings)


void Backup::disable(const QSharedPointer<QSettings>& pSettings)
{
	const auto& fileName = pSettings->fileName();
	const NSURL* const fileURL = [NSURL fileURLWithPath: fileName.toNSString()];

	const bool fileExists = [[NSFileManager defaultManager] fileExistsAtPath: [fileURL path]];
	if (!fileExists)
	{
		qCDebug(settings) << "Currently no settings file exists. Expecting" << fileName;
		return;
	}

	NSError* error = nil;
	bool success = [fileURL setResourceValue : [NSNumber numberWithBool : YES]
			forKey : NSURLIsExcludedFromBackupKey
			error : &error];
	if (!success)
	{
		qCWarning(settings) << "Error excluding" << fileName << "from backup";
		if (error != nil)
		{
			qCWarning(settings) << "Exclude error:" << error.localizedDescription;
		}
	}
}
