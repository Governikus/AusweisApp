/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardInfo.h"
#include "ReaderConfigurationInfo.h"
#include "ReaderManagerPluginInfo.h"
#include "SmartCardDefinitions.h"

#include <QString>
#include <QVariant>

namespace governikus
{
class ReaderInfo
{
	friend class Reader;

	private:
		ReaderManagerPluginType mPluginType;
		QString mName;
		bool mBasicReader;
		CardInfo mCardInfo;
		int mMaxApduLength;
		CardType mShelvedCard;

	public:
		explicit ReaderInfo(const QString& pName = QString(),
				ReaderManagerPluginType pPluginType = ReaderManagerPluginType::UNKNOWN,
				const CardInfo& pCardInfo = CardInfo(CardType::NONE));

		[[nodiscard]] ReaderConfigurationInfo getReaderConfigurationInfo() const;


		[[nodiscard]] ReaderManagerPluginType getPluginType() const
		{
			return mPluginType;
		}


		[[nodiscard]] bool isValid() const
		{
			return mPluginType != ReaderManagerPluginType::UNKNOWN;
		}


		void invalidate()
		{
			mPluginType = ReaderManagerPluginType::UNKNOWN;
			mCardInfo = CardInfo(CardType::NONE);
		}


		[[nodiscard]] CardInfo& getCardInfo()
		{
			return mCardInfo;
		}


		[[nodiscard]] const CardInfo& getCardInfo() const
		{
			return mCardInfo;
		}


		[[nodiscard]] CardType getCardType() const
		{
			return mCardInfo.getCardType();
		}


		[[nodiscard]] QString getCardTypeString() const
		{
			return mCardInfo.getCardTypeString();
		}


		[[nodiscard]] bool hasCard() const
		{
			return mCardInfo.getCardType() != CardType::NONE;
		}


		[[nodiscard]] bool hasEid() const
		{
			return QList<CardType>({CardType::EID_CARD, CardType::SMART_EID}).contains(mCardInfo.getCardType());
		}


		[[nodiscard]] int getRetryCounter() const
		{
			return mCardInfo.getRetryCounter();
		}


		[[nodiscard]] bool isRetryCounterDetermined() const
		{
			return mCardInfo.isRetryCounterDetermined();
		}


		[[nodiscard]] bool isPinDeactivated() const
		{
			return mCardInfo.isPinDeactivated();
		}


		[[nodiscard]] bool isPukInoperative() const
		{
			return mCardInfo.isPukInoperative();
		}


		[[nodiscard]] bool isSoftwareSmartEid() const
		{
			return mCardInfo.getMobileEidType() == MobileEidType::HW_KEYSTORE;
		}


		[[nodiscard]] bool wasShelved() const
		{
			return mShelvedCard != CardType::NONE;
		}


		void shelveCard()
		{
			mShelvedCard = mCardInfo.getCardType();
			mCardInfo.setCardType(CardType::NONE);
		}


		[[nodiscard]] bool isInsertable() const;


		void insertCard()
		{
			mCardInfo.setCardType(mShelvedCard);
		}


		void setCardInfo(const CardInfo& pCardInfo)
		{
			mCardInfo = pCardInfo;
		}


		[[nodiscard]] const QString& getName() const
		{
			return mName;
		}


		void setBasicReader(bool pIsBasicReader)
		{
			mBasicReader = pIsBasicReader;
		}


		[[nodiscard]] bool isBasicReader() const
		{
			return mBasicReader;
		}


		void setMaxApduLength(int pMaxApduLength)
		{
			mMaxApduLength = pMaxApduLength;
		}


		[[nodiscard]] int getMaxApduLength() const
		{
			return mMaxApduLength;
		}


		[[nodiscard]] bool insufficientApduLength() const
		{
			return mMaxApduLength >= 0 && mMaxApduLength < 500;
		}


};

} // namespace governikus
