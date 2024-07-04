/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginQml.h"

#include "Env.h"
#include "UiLoader.h"

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
				if (auto* uiPlugin = Env::getSingleton<UiLoader>()->getLoaded<UiPluginQml>())
				{
					Q_EMIT uiPlugin->fireAppConfigChanged();
				}
			}, Qt::QueuedConnection);
	}
}


@end


UiPluginQml::Private::Private() : mFontChangeTracker([[FontChangeTracker alloc] init])
{
}


// It's important that the definition of the destructor is in a .mm file: Otherwise the compiler won't compile it in Objective-C++ mode and ARC won't work.
UiPluginQml::Private::~Private()
{
}


QVariantMap UiPluginQml::getSafeAreaMargins() const
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


qreal UiPluginQml::getSystemFontScaleFactor() const
{
	UIFont* font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
	return font.pointSize / 15.0;
}
