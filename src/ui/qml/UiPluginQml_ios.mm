/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginQml.h"

#include "Env.h"
#include "PlatformTools.h"
#include "SettingsModel.h"
#include "UiLoader.h"

#include <QLoggingCategory>

#import <UIKit/UIKit.h>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace Qt::Literals::StringLiterals;
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

@interface QIOSViewController
	: UIViewController
@property (nonatomic, assign) UIStatusBarStyle preferredStatusBarStyle;
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
	UIWindow* window = PlatformTools::getFirstWindow();
	if (!window)
	{
		return {
			{"top"_L1, 0},
			{"right"_L1, 0},
			{"bottom"_L1, 0},
			{"left"_L1, 0}
		};
	}
	UIEdgeInsets safeAreaInsets = window.safeAreaInsets;

	return {
		{"top"_L1, safeAreaInsets.top},
		{"right"_L1, safeAreaInsets.right},
		{"bottom"_L1, safeAreaInsets.bottom},
		{"left"_L1, safeAreaInsets.left}
	};
}


qreal UiPluginQml::getSystemFontScaleFactor() const
{
	UIFont* font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
	return font.pointSize / 15.0;
}


void UiPluginQml::onUserDarkModeChanged() const
{
	UIWindow* window = PlatformTools::getFirstWindow();
	if (!window)
	{
		qCWarning(qml) << "Invalid window, can't set Status Bar color";
		return;
	}

	UIViewController* rootController = window.rootViewController;
	QIOSViewController* qtViewController = static_cast<QIOSViewController*>(rootController);
	if (!qtViewController)
	{
		qCWarning(qml) << "Invalid rootController, can't set Status Bar color";
		return;
	}

	qtViewController.preferredStatusBarStyle = isDarkModeEnabled() ? UIStatusBarStyleLightContent : UIStatusBarStyleDarkContent;
	[qtViewController setNeedsStatusBarAppearanceUpdate];
}
