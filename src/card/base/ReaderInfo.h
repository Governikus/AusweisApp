/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardInfo.h"
#include "ReaderConfigurationInfo.h"
#include "ReaderManagerPlugInInfo.h"
#include "SmartCardDefinitions.h"

#include <QString>
#include <QVariant>

namespace governikus
{
class ReaderInfo
{
	friend class Reader;

	private:
		ReaderManagerPlugInType mPlugInType;
		QString mName;
		bool mBasicReader;
		CardInfo mCardInfo;
		int mMaxApduLength;
		CardType mShelvedCard;

	public:
		explicit ReaderInfo(const QString& pName = QString(),
				ReaderManagerPlugInType pPlugInType = ReaderManagerPlugInType::UNKNOWN,
				const CardInfo& pCardInfo = CardInfo(CardType::NONE));

		[[nodiscard]] ReaderConfigurationInfo getReaderConfigurationInfo() const;


		[[nodiscard]] ReaderManagerPlugInType getPlugInType() const
		{
			return mPlugInType;
		}


		[[nodiscard]] bool isValid() const
		{
			return mPlugInType != ReaderManagerPlugInType::UNKNOWN;
		}


		void invalidate()
		{
			mPlugInType = ReaderManagerPlugInType::UNKNOWN;
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
			return QVector<CardType>({CardType::EID_CARD, CardType::SMART_EID}).contains(mCardInfo.getCardType());
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


		[[nodiscard]] bool isPhysicalCard() const
		{
			const auto& cardType = mCardInfo.getCardType();
			return cardType == CardType::EID_CARD;
		}


		[[nodiscard]] bool isSoftwareSmartEid() const
		{
			return mCardInfo.getMobileEidType() == MobileEidType::HW_KEYSTORE;
		}


		bool wasShelved() const
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
