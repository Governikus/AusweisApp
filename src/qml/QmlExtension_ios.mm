/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "QmlExtension.h"

#include <QLoggingCategory>
#import <UIKit/UIKit.h>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;


void QmlExtension::showSettings(const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED";
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
	qCWarning(qml) << "NOT IMPLEMENTED";
}


void QmlExtension::showFeedback(const QString& pMessage)
{
	NSString* msg = pMessage.toNSString();

	UIAlertController* alert = [UIAlertController
			alertControllerWithTitle:msg
			message:@""
			preferredStyle:UIAlertControllerStyleAlert];

	UIViewController* rootController = [[UIApplication sharedApplication].keyWindow rootViewController];
	[rootController presentViewController:alert animated:YES completion:nil];
	dispatch_after(dispatch_time(DISPATCH_TIME_NOW, static_cast<int64_t>(3.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
				[alert dismissViewControllerAnimated:YES completion:nil];
			});
}


bool QmlExtension::exportHistory(const QString&) const
{
	qCWarning(qml) << "NOT IMPLEMENTED";
	return false;
}


void QmlExtension::keepScreenOn(bool)
{
	qCWarning(qml) << "NOT IMPLEMENTED";
}


#include "moc_QmlExtension.cpp"
