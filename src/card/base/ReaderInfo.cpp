/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderInfo.h"

#include "Initializer.h"
#include "SmartCardDefinitions.h"

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


[[nodiscard]] ReaderManagerPluginType ReaderInfo::getPluginType() const
{
	return mPluginType;
}


[[nodiscard]] bool ReaderInfo::isValid() const
{
	return mPluginType != ReaderManagerPluginType::UNKNOWN;
}


void ReaderInfo::invalidate()
{
	mPluginType = ReaderManagerPluginType::UNKNOWN;
	mCardInfo = CardInfo(CardType::NONE);
}


[[nodiscard]] CardInfo& ReaderInfo::getCardInfo()
{
	return mCardInfo;
}


[[nodiscard]] const CardInfo& ReaderInfo::getCardInfo() const
{
	return mCardInfo;
}


[[nodiscard]] CardType ReaderInfo::getCardType() const
{
	return mCardInfo.getCardType();
}


[[nodiscard]] QString ReaderInfo::getCardTypeString() const
{
	return mCardInfo.getCardTypeString();
}


[[nodiscard]] bool ReaderInfo::hasCard() const
{
	return mCardInfo.getCardType() != CardType::NONE;
}


[[nodiscard]] bool ReaderInfo::ReaderInfo::hasEid() const
{
	return mCardInfo.getCardType() == CardType::EID_CARD;
}


[[nodiscard]] int ReaderInfo::getRetryCounter() const
{
	return mCardInfo.getRetryCounter();
}


[[nodiscard]] bool ReaderInfo::isRetryCounterDetermined() const
{
	return mCardInfo.isRetryCounterDetermined();
}


[[nodiscard]] bool ReaderInfo::isPinDeactivated() const
{
	return mCardInfo.isPinDeactivated();
}


[[nodiscard]] bool ReaderInfo::isPukInoperative() const
{
	return mCardInfo.isPukInoperative();
}


[[nodiscard]] bool ReaderInfo::wasShelved() const
{
	return mShelvedCard != CardType::NONE;
}


void ReaderInfo::shelveCard()
{
	mShelvedCard = mCardInfo.getCardType();
	mCardInfo.setCardType(CardType::NONE);
}


[[nodiscard]] bool ReaderInfo::isInsertable() const
{
	return mShelvedCard != CardType::NONE;
}


void ReaderInfo::insertCard()
{
	mCardInfo.setCardType(mShelvedCard);
}


void ReaderInfo::setCardInfo(const CardInfo& pCardInfo)
{
	mCardInfo = pCardInfo;
}


[[nodiscard]] const QString& ReaderInfo::getName() const
{
	return mName;
}


void ReaderInfo::setBasicReader(bool pIsBasicReader)
{
	mBasicReader = pIsBasicReader;
}


[[nodiscard]] bool ReaderInfo::isBasicReader() const
{
	return mBasicReader;
}


void ReaderInfo::setMaxApduLength(int pMaxApduLength)
{
	mMaxApduLength = pMaxApduLength;
}


[[nodiscard]] int ReaderInfo::getMaxApduLength() const
{
	return mMaxApduLength;
}


[[nodiscard]] bool ReaderInfo::insufficientApduLength() const
{
	return mMaxApduLength >= 0 && mMaxApduLength < 500;
}
