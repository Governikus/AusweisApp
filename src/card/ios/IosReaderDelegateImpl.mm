/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#include "IosReaderDelegateImpl.h"

#include "IosCard.h"
#include "IosCardPointer.h"

#include <QLoggingCategory>

#import <CoreNFC/NFCTag.h>


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


@implementation IosReaderDelegateImpl

+ (IosReaderDelegateImpl*) initWithDelegate:(governikus::IosReaderDelegate*)pDelegate {
	IosReaderDelegateImpl* dlgt = [[IosReaderDelegateImpl alloc] init];
	dlgt.mDelegate = pDelegate;
	return dlgt;

}


- (instancetype) init {
	self = [super init];
	return self;
}


- (void) startSession: (QString)pMessage {
	qCDebug(card_nfc) << "Start session";

	if (self.mSession)
	{
		[self.mSession restartPolling];
	}
	else
	{
		self.mSession = [[NFCTagReaderSession alloc] initWithPollingOption: NFCPollingISO14443 delegate: self queue: nil];
		if (self.mSession)
		{
			self.mSession.alertMessage = pMessage.toNSString();
			[self.mSession beginSession];
		}
	}
}


- (void) stopSession: (QString)pMessage isError:(bool) pIsError {
	qCDebug(card_nfc) << "Stop session";

	if (self.mSession)
	{
		if (pIsError)
		{
			[self.mSession invalidateSessionWithErrorMessage: pMessage.toNSString()];
		}
		else
		{
			self.mSession.alertMessage = pMessage.toNSString();
			[self.mSession invalidateSession];
		}
		self.mSession = nil;
	}
}


- (void) tagReaderSessionDidBecomeActive: (NFCTagReaderSession*)session {
	qCDebug(card_nfc) << "Session activated";

	if (session != self.mSession)
	{
		qCWarning(card_nfc) << "An unexpected session became active";
		return;
	}
}


- (void) tagReaderSession: (NFCTagReaderSession*)session didInvalidateWithError:(NSError*)error {
	qCDebug(card_nfc) << "Session did invalidate with error:" << error;

	if (session != self.mSession)
	{
		qCWarning(card_nfc) << "An unexpected session was invalidated";
		return;
	}

	const bool doRestart = !(error.code == NFCReaderError::NFCReaderSessionInvalidationErrorUserCanceled ||
			error.code == NFCReaderError::NFCReaderErrorUnsupportedFeature);
	self.mDelegate->onDidInvalidateWithError(QString::fromNSString(error.localizedDescription), doRestart);
}


- (void) tagReaderSession: (NFCTagReaderSession*)session didDetectTags:(NSArray<__kindof id<NFCTag>>*)tags {
	qCDebug(card_nfc) << "New tag detected";

	if (session != self.mSession)
	{
		qCWarning(card_nfc) << "Detected a new tag from an unexpected session";
		return;
	}

	if (tags.count > 1)
	{
		qCWarning(card_nfc) << "Detected more than one tag. Restarting scan";
		[session restartPolling];
		return;
	}

	if (tags[0].type != NFCTagTypeISO7816Compatible)
	{
		qCWarning(card_nfc) << "Detected a new tag with an unexpected type:" << tags[0].type << "Restarting scan";
		[session restartPolling];
		return;
	}

	self.mDelegate->onTagDiscovered(new governikus::IosCard(new governikus::IosCardPointer{tags[0]}));
}


@end
