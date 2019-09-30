/*!
 * \brief Implementation of \ref IosReaderDelegateImpl for iOS.
 *
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IosReaderDelegate.h"

#import <CoreNFC/CoreNFC.h>


@interface IosReaderDelegateImpl
	: NSObject<NFCTagReaderSessionDelegate>

@property governikus::IosReaderDelegate* mDelegate;
@property (retain) NFCTagReaderSession* mSession;

+ (IosReaderDelegateImpl*) initWithDelegate: (governikus::IosReaderDelegate*) pDelegate;

- (void) startSession: (QString)pMessage;

- (void) stopSession: (QString)pMessage isError:(bool) pIsError;


@end
