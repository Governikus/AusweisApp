/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"
#include "PlatformTools.h"

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
		BOOL isRunning = UIAccessibilityIsVoiceOverRunning();
		if (self.mRunning != isRunning)
		{
			self.mRunning = isRunning;
			ApplicationModel::notifyScreenReaderChangedThreadSafe();
		}
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


void ApplicationModel::keepScreenOn(bool pActive) const
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


void ApplicationModel::showAppStoreRatingDialog() const
{
	UIWindowScene* windowScene = PlatformTools::getFirstWindowScene();
	if (!windowScene)
	{
		qCCritical(qml) << "Could not get window scene for store feedback.";
		return;
	}

	qCDebug(feedback) << "Requesting iOS AppStore review";
	[SKStoreReviewController requestReviewInScene: windowScene];
}


void ApplicationModel::showSettings(const ApplicationModel::Settings& pAction) const
{
	if (pAction == Settings::APP)
	{
		[[UIApplication sharedApplication] openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString] options:@{} completionHandler:nil];
	}
	else
	{
		qCWarning(qml) << "NOT IMPLEMENTED:" << pAction;
	}
}
