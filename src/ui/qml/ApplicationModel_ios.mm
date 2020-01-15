/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include <QLoggingCategory>

#import <StoreKit/StoreKit.h>
#import <UIKit/UIKit.h>

Q_DECLARE_LOGGING_CATEGORY(feedback)

using namespace governikus;

@interface VoiceOverObserver
	: NSObject
@property BOOL mRunning;
- (instancetype) init;
- (void) dealloc;
- (void) receiveNotification: (NSNotification*) notification;
@end

@implementation VoiceOverObserver

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super dealloc];
}


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


ApplicationModel::Private::~Private()
{
	[mObserver dealloc];
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
