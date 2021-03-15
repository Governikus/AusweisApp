/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardInfo.h"
#include "ReaderConfigurationInfo.h"
#include "ReaderManagerPlugInInfo.h"
#include "SmartCardDefinitions.h"

#include <QString>

namespace governikus
{
class ReaderInfo
{
	friend class Reader;

	ReaderManagerPlugInType mPlugInType;
	QString mName;
	bool mBasicReader;
	CardInfo mCardInfo;
	bool mConnected;
	int mMaxApduLength;

	public:
		explicit ReaderInfo(const QString& pName = QString(),
				ReaderManagerPlugInType pPlugInType = ReaderManagerPlugInType::UNKNOWN,
				const CardInfo& pCardInfo = CardInfo(CardType::NONE));

		[[nodiscard]] ReaderConfigurationInfo getReaderConfigurationInfo() const;


		[[nodiscard]] ReaderManagerPlugInType getPlugInType() const
		{
			return mPlugInType;
		}


		CardInfo& getCardInfo()
		{
			return mCardInfo;
		}


		[[nodiscard]] const CardInfo& getCardInfo() const
		{
			return mCardInfo;
		}


		[[nodiscard]] QString getCardTypeString() const
		{
			return mCardInfo.getCardTypeString();
		}


		[[nodiscard]] bool hasCard() const
		{
			return mCardInfo.isAvailable();
		}


		[[nodiscard]] bool hasEidCard() const
		{
			return mCardInfo.isEid();
		}


		[[nodiscard]] bool hasPassport() const
		{
			return mCardInfo.isPassport();
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


		[[nodiscard]] bool isConnected() const
		{
			return mConnected;
		}


		void setConnected(bool pConnected)
		{
			mConnected = pConnected;
		}


		void setMaxApduLength(int pMaxApduLength)
		{
			mMaxApduLength = pMaxApduLength;
		}


		[[nodiscard]] int getMaxApduLength() const
		{
			return mMaxApduLength;
		}


		[[nodiscard]] bool sufficientApduLength() const
		{
			return mMaxApduLength == -1 || mMaxApduLength >= 500;
		}


};

} // namespace governikus
