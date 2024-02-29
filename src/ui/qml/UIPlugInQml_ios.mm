/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInQml.h"

#include "Env.h"
#include "UILoader.h"

#include <QLoggingCategory>

#import <UIKit/UIKit.h>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;

@interface FontChangeTracker
	: NSObject
- (instancetype) init;
- (void) receiveNotification: (NSNotification*) notification;
@end
@implementation FontChangeTracker
- (instancetype) init
{
	self = [super init];
	if (!self)
	{
		return nil;
	}
	[[NSNotificationCenter defaultCenter]
	addObserver:self
	selector:@selector(receiveNotification:)
	name:UIContentSizeCategoryDidChangeNotification
	object:nil];
	return self;
}


- (void)receiveNotification:(NSNotification*)notification
{
	if ([notification.name
			isEqualToString:
			UIContentSizeCategoryDidChangeNotification])
	{
		QMetaObject::invokeMethod(QCoreApplication::instance(), [] {
				if (auto* uiPlugIn = Env::getSingleton<UILoader>()->getLoaded<UIPlugInQml>())
				{
					Q_EMIT uiPlugIn->fireAppConfigChanged();
				}
			}, Qt::QueuedConnection);
	}
}


@end


UIPlugInQml::Private::Private() : mFontChangeTracker([[FontChangeTracker alloc] init])
{
}


// It's important that the definition of the destructor is in a .mm file: Otherwise the compiler won't compile it in Objective-C++ mode and ARC won't work.
UIPlugInQml::Private::~Private()
{
}


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


qreal UIPlugInQml::getSystemFontScaleFactor() const
{
	UIFont* font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
	return font.pointSize / 15.0;
}
