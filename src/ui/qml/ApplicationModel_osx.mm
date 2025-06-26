/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include <AppKit/NSAccessibility.h>
#include <AppKit/NSWorkspace.h>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(qml)

@interface VoiceOverObserver
	: NSObject
@property BOOL mRunning;
- (instancetype) init;
- (void) observeValueForKeyPath:(NSString*) keyPath
		ofObject:(id) object
		change:(NSDictionary<NSString*, id>*) change
		context:(void*) context;
@end

@implementation VoiceOverObserver


- (instancetype)init {
	self = [super init];
	if (!self)
	{
		return nil;
	}

	[[NSWorkspace sharedWorkspace]
	addObserver:self
	forKeyPath:@"voiceOverEnabled"
	options:(NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld)
	context:nil];

	self.mRunning = NSWorkspace.sharedWorkspace.voiceOverEnabled;

	return self;
}


- (void) observeValueForKeyPath:(NSString*) keyPath
		ofObject:(id) object
		change:(NSDictionary<NSString*, id>*) change
		context:(void*) context {
	if ([keyPath isEqualToString:@"voiceOverEnabled"])
	{
		BOOL isRunning = NSWorkspace.sharedWorkspace.voiceOverEnabled;
		if (self.mRunning != isRunning)
		{
			self.mRunning = isRunning;
			ApplicationModel::notifyScreenReaderChangedThreadSafe();
		}
		return;
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


void ApplicationModel::showSettings(const ApplicationModel::Settings& pAction) const
{
	qCWarning(qml) << "NOT IMPLEMENTED:" << pAction;
}
