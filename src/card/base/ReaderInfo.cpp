/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderInfo.h"

#include "Initializer.h"

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	#include "ReaderDetector.h"
#endif

using namespace governikus;

INIT_FUNCTION([] {
			qRegisterMetaType<ReaderInfo>("ReaderInfo");
		})

ReaderInfo::ReaderInfo(const QString& pName,
		ReaderManagerPluginType pPluginType,
		const CardInfo& pCardInfo)
	: mPluginType(pPluginType)
	, mName(pName)
	, mBasicReader(true)
	, mCardInfo(pCardInfo)
	, mMaxApduLength(500)
	, mShelvedCard(CardType::NONE)
{
#ifdef Q_OS_ANDROID
	if (pPluginType == ReaderManagerPluginType::NFC)
	{
		mMaxApduLength = -1;
	}
#endif
}


ReaderConfigurationInfo ReaderInfo::getReaderConfigurationInfo() const
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	return Env::getSingleton<ReaderDetector>()->getReaderConfigurationInfo(mName);

#else
	return ReaderConfigurationInfo(mName);

#endif
}


[[nodiscard]] bool ReaderInfo::isInsertable() const
{
	switch (mShelvedCard)
	{
		case CardType::NONE:
			return false;

		case CardType::SMART_EID:
			return mCardInfo.getRetryCounter() > 0 && !mCardInfo.isPinInitial();

		default:
			return true;
	}

}
