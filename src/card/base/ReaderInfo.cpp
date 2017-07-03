/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ReaderInfo.h"

using namespace governikus;


ReaderInfo::ReaderInfo(ReaderManagerPlugInType pPlugInType,
		const QString& pName,
		ReaderType pReaderType,
		const CardInfo& pCardInfo)
	: mPlugInType(pPlugInType)
	, mName(pName)
	, mReaderType(pReaderType)
	, mBasicReader(true)
	, mCardInfo(pCardInfo)
	, mConnected(false)
	, mExtendedLengthApduSupportCode(pPlugInType == ReaderManagerPlugInType::NFC ? ExtendedLengthApduSupportCode::UNKNOWN : ExtendedLengthApduSupportCode::SUPPORTED)
{
}
