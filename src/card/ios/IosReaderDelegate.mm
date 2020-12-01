/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#include "IosReaderDelegate.h"

#include "IosCardPointer.h"
#include "IosReader.h"

#include <QLoggingCategory>

#import <CoreNFC/NFCError.h>
#import <CoreNFC/NFCTag.h>


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


@implementation IosReaderDelegate

- (instancetype)initWithListener: (governikus::IosReader*)pListener {
	self = [super init];
	if (self)
	{
		self.mListener = pListener;
		self.mSessionStoppedByApplication = false;
		self.mMessage = nil;
		self.mSession = nil;
	}

	return self;
}


- (void)startSession {
	if (self.mSessionStoppedByApplication)
	{
		Q_EMIT self.mListener->fireDidInvalidateWithError(true);
		return;
	}

	if (self.mSession)
	{
		qCDebug(card_nfc) << "Restart session" << self.mSession;
		[self.mSession restartPolling];
		return;
	}

	qCDebug(card_nfc) << "Requesting new session";
	self.mSession = [[NFCTagReaderSession alloc] initWithPollingOption:NFCPollingISO14443 delegate:self queue:nil];
	if (self.mSession)
	{
		qCDebug(card_nfc) << "Start session" << self.mSession;
		if (self.mMessage)
		{
			self.mSession.alertMessage = self.mMessage;
		}
		[self.mSession beginSession];
	}
	else
	{
		qCWarning(card_nfc) << "Failed to initialize a new session";
		Q_EMIT self.mListener->fireDidInvalidateWithError(true);
	}
}


- (void)stopSession: (QString)message {
	qCDebug(card_nfc) << "Stop session" << self.mSession;

	if (self.mSession)
	{
		if (self.mSession.ready)
		{
			if (message.isNull())
			{
				[self.mSession invalidateSession];
			}
			else
			{
				[self.mSession invalidateSessionWithErrorMessage:message.toNSString()];
			}
			self.mSessionStoppedByApplication = true;
		}
		else
		{
			self.mSession = nil;
		}
	}
}


- (void)alertMessage: (QString)message {
	if (self.mSession)
	{
		self.mSession.alertMessage = message.toNSString();
	}
	else
	{
		self.mMessage = message.toNSString();
	}
}


- (void)tagReaderSessionDidBecomeActive: (NFCTagReaderSession*)session {
	qCDebug(card_nfc) << "Session activated" << session;

	if (session != self.mSession)
	{
		qCWarning(card_nfc) << "An unexpected session became active; mSession" << self.mSession;
		[session invalidateSession];
		return;
	}
}


- (void)tagReaderSession: (NFCTagReaderSession*)session didInvalidateWithError: (NSError*)error {
	qCDebug(card_nfc) << "Session" << session << "did invalidate with error:" << error;

	if (session != self.mSession)
	{
		qCWarning(card_nfc) << "An unexpected session was invalidated; mSession" << self.mSession;
		return;
	}

	self.mSession = nil;
	if (self.mSessionStoppedByApplication)
	{
		self.mSessionStoppedByApplication = false;
		return;
	}

	const bool doRestart = !(error.code == NFCReaderError::NFCReaderSessionInvalidationErrorUserCanceled ||
			error.code == NFCReaderError::NFCReaderErrorUnsupportedFeature);
	Q_EMIT self.mListener->fireDidInvalidateWithError(doRestart);
}


- (void)tagReaderSession: (NFCTagReaderSession*)session didDetectTags: (NSArray<__kindof id<NFCTag>>*)tags {
	qCDebug(card_nfc) << "New tag detected on session" << session;

	if (session != self.mSession)
	{
		qCWarning(card_nfc) << "Detected a new tag from an unexpected session; mSession" << self.mSession;
		return;
	}

	bool foundTag = false;
	for (id<NFCTag> tag in tags)
	{
		if (tag.type == NFCTagTypeISO7816Compatible)
		{
			foundTag = true;
			Q_EMIT self.mListener->fireTagDiscovered(new governikus::IosCardPointer{tag});
		}
		else
		{
			qCWarning(card_nfc) << "Detected a new tag with an unexpected type:" << tags[0].type << "Restarting scan";
		}
	}

	if (!foundTag)
	{
		[session restartPolling];
	}
}


@end
