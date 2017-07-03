/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "QmlExtension.h"

#include <QLoggingCategory>
#import <UIKit/UIKit.h>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;


void QmlExtension::showSettings(const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED YET";
}


void QmlExtension::shareText(const QString& pText, const QString& pChooserTitle)
{
	Q_UNUSED(pChooserTitle);
	NSMutableArray* shareItemArray = [NSMutableArray new];
	[shareItemArray addObject : pText.toNSString()];
	UIViewController* rootController = [[UIApplication sharedApplication].keyWindow rootViewController];
	UIActivityViewController* activityController = [[UIActivityViewController alloc] initWithActivityItems:shareItemArray applicationActivities:nil];
	if ([activityController respondsToSelector : @selector(popoverPresentationController)])
	{
		activityController.popoverPresentationController.sourceView = rootController.view;
	}
	[rootController presentViewController : activityController animated : YES completion : nil];
}


void QmlExtension::mailLog(const QString&, const QString&, const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED YET";
}


void QmlExtension::showFeedback(const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED YET";
}


bool QmlExtension::exportHistory(const QString&) const
{
	qCWarning(qml) << "NOT IMPLEMENTED YET";
	return false;
}


#include "moc_QmlExtension.cpp"
