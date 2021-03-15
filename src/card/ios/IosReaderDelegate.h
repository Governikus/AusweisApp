/*!
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

#import <CoreNFC/NFCReaderSession.h>
#import <CoreNFC/NFCTagReaderSession.h>
#import <Foundation/Foundation.h>

namespace governikus
{
class IosReader;
} // namespace governikus

API_AVAILABLE(ios(13.0))
@interface IosReaderDelegate
	: NSObject<NFCTagReaderSessionDelegate>

@property governikus::IosReader* mListener;
@property bool mSessionStoppedByApplication;
@property (nonatomic, strong) NSString* mMessage;
@property (nonatomic, strong) NFCTagReaderSession* mSession;

- (instancetype)initWithListener:(governikus::IosReader*)pListener;

- (void)startSession;
- (void)stopSession:(QString)pMessage;

- (void)alertMessage:(QString)pMessage;

@end
