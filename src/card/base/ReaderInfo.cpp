/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderInfo.h"

#include "Initializer.h"
#include "ReaderDetector.h"

using namespace governikus;

INIT_FUNCTION([] {
			qRegisterMetaType<ReaderInfo>("ReaderInfo");
		})

ReaderInfo::ReaderInfo(const QString& pName,
		ReaderManagerPlugInType pPlugInType,
		const CardInfo& pCardInfo)
	: mPlugInType(pPlugInType)
	, mName(pName)
	, mBasicReader(true)
	, mCardInfo(pCardInfo)
	, mConnected(false)
	, mMaxApduLength(500)
{
#ifdef Q_OS_ANDROID
	if (pPlugInType == ReaderManagerPlugInType::NFC)
	{
		mMaxApduLength = -1;
	}
#endif
}


ReaderConfigurationInfo ReaderInfo::getReaderConfigurationInfo() const
{
	return Env::getSingleton<ReaderDetector>()->getReaderConfigurationInfo(mName);
}
