/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "ShareUtil.h"

#import <UIKit/UIKit.h>


using namespace governikus;


void ShareUtil::shareText(const QString& pText, const QString& pChooserTitle)
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


#include "moc_ShareUtil.cpp"
