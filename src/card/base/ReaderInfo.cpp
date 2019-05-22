/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderInfo.h"

#include "Initializer.h"
#include "ReaderDetector.h"

using namespace governikus;

static Initializer::Entry X([] {
			qRegisterMetaType<ReaderInfo>("ReaderInfo");
		});

ReaderInfo::ReaderInfo(const QString& pName,
		ReaderManagerPlugInType pPlugInType,
		const CardInfo& pCardInfo)
	: mPlugInType(pPlugInType)
	, mName(pName)
	, mBasicReader(true)
	, mCardInfo(pCardInfo)
	, mConnected(false)
	, mMaxApduLength(pPlugInType == ReaderManagerPlugInType::NFC ? 0 : 500)
{
}


ReaderConfigurationInfo ReaderInfo::getReaderConfigurationInfo() const
{
	return Env::getSingleton<ReaderDetector>()->getReaderConfigurationInfo(mName);
}
