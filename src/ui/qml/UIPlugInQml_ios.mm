/*!
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInQml.h"

#import <UIKit/UIKit.h>

using namespace governikus;


bool UIPlugInQml::isTablet() const
{
	return [UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad;
}


QVariantMap UIPlugInQml::getSafeAreaMargins() const
{
	UIViewController* rootController = [UIApplication sharedApplication].windows[0].rootViewController;

	UIEdgeInsets safeAreaInsets = rootController.view.safeAreaInsets;

	QVariantMap insetMap;

	insetMap[QStringLiteral("top")] = safeAreaInsets.top;
	insetMap[QStringLiteral("right")] = safeAreaInsets.right;
	insetMap[QStringLiteral("bottom")] = safeAreaInsets.bottom;
	insetMap[QStringLiteral("left")] = safeAreaInsets.left;

	return insetMap;
}
