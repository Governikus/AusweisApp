/*!
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#import <CoreNFC/NFCTag.h>

namespace governikus
{

/// Implementation detail which can only be used by Object-C++ since it stores a pointer to an Objective-C protocol.
struct IosCardPointer
{
	id<NFCTag> mNfcTag;
};

} // namespace governikus
