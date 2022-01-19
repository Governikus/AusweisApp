/*!
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include <QLoggingCategory>

#import <StoreKit/StoreKit.h>
#import <UIKit/UIKit.h>

Q_DECLARE_LOGGING_CATEGORY(qml)
Q_DECLARE_LOGGING_CATEGORY(feedback)

using namespace governikus;

@interface VoiceOverObserver
	: NSObject
@property BOOL mRunning;
- (instancetype) init;
- (void) receiveNotification: (NSNotification*) notification;
@end

@implementation VoiceOverObserver


- (instancetype)init {
	self = [super init];
	if (!self)
	{
		return nil;
	}

	[[NSNotificationCenter defaultCenter]
	addObserver:self
	selector:@selector(receiveNotification:)
	name:UIAccessibilityVoiceOverStatusDidChangeNotification
	object:nil];

	self.mRunning = UIAccessibilityIsVoiceOverRunning();

	return self;
}


- (void)receiveNotification:(NSNotification*)notification {
	if ([notification.name
			isEqualToString:
			UIAccessibilityVoiceOverStatusDidChangeNotification])
	{
		self.mRunning = UIAccessibilityIsVoiceOverRunning();
	}
}


@end


ApplicationModel::Private::Private() : mObserver([[VoiceOverObserver alloc] init])
{
}


// It's important that the definition of the destructor is in a .mm file: Otherwise the compiler won't compile it in Objective-C++ mode and ARC won't work.
ApplicationModel::Private::~Private()
{
}


bool ApplicationModel::isScreenReaderRunning() const
{
	return mPrivate->mObserver.mRunning;
}


void ApplicationModel::keepScreenOn(bool pActive)
{
	if (pActive)
	{
		[[UIApplication sharedApplication]setIdleTimerDisabled:YES];
	}
	else
	{
		[[UIApplication sharedApplication]setIdleTimerDisabled:NO];
	}
}


void ApplicationModel::showAppStoreRatingDialog()
{
	qCDebug(feedback) << "Requesting iOS AppStore review";
	[SKStoreReviewController requestReview];
}


void ApplicationModel::showSettings(const ApplicationModel::Settings& pAction)
{
	if (pAction == Settings::SETTING_APP)
	{
		[[UIApplication sharedApplication] openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString] options:@{} completionHandler:nil];
	}
	else
	{
		qCWarning(qml) << "NOT IMPLEMENTED:" << pAction;
	}
}
