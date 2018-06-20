/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardInfo.h"
#include "EnumHelper.h"
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
	ReaderConfigurationInfo mReaderConfigurationInfo;
	bool mBasicReader;
	CardInfo mCardInfo;
	bool mConnected;
	int mMaxApduLength;

	public:
		ReaderInfo(const QString& pName = QString(),
				ReaderManagerPlugInType pPlugInType = ReaderManagerPlugInType::UNKNOWN,
				const CardInfo& pCardInfo = CardInfo(CardType::NONE));


		ReaderManagerPlugInType getPlugInType() const
		{
			return mPlugInType;
		}


		const ReaderConfigurationInfo& getReaderConfigurationInfo() const
		{
			return mReaderConfigurationInfo;
		}


		const CardInfo& getCardInfo() const
		{
			return mCardInfo;
		}


		QString getCardTypeString() const
		{
			return mCardInfo.getCardTypeString();
		}


		bool hasCard() const
		{
			return mCardInfo.isAvailable();
		}


		bool hasEidCard() const
		{
			return mCardInfo.isEid();
		}


		int getRetryCounter() const
		{
			return mCardInfo.getRetryCounter();
		}


		bool isRetryCounterDetermined() const
		{
			return mCardInfo.isRetryCounterDetermined();
		}


		bool isPinDeactivated() const
		{
			return mCardInfo.isPinDeactivated();
		}


		bool isPukInoperative() const
		{
			return mCardInfo.isPukInoperative();
		}


		void setCardInfo(const CardInfo& pCardInfo)
		{
			mCardInfo = pCardInfo;
		}


		const QString& getName() const
		{
			return mName;
		}


		void setBasicReader(bool pIsBasicReader)
		{
			mBasicReader = pIsBasicReader;
		}


		bool isBasicReader() const
		{
			return mBasicReader;
		}


		bool isConnected() const
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


		int getMaxApduLength() const
		{
			return mMaxApduLength;
		}


		bool sufficientApduLength() const
		{
			if (!hasCard())
			{
				return true;
			}
			return mMaxApduLength == 0 || mMaxApduLength >= 500;
		}


};

} /* namespace governikus */
