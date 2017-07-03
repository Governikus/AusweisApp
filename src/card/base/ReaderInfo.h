/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "CardInfo.h"
#include "EnumHelper.h"
#include "MetaTypeHelper.h"
#include "ReaderManagerPlugInInfo.h"
#include "SmartCardDefinitions.h"

#include <QString>

namespace governikus
{

defineEnumType(ExtendedLengthApduSupportCode,
		UNKNOWN = -1,
		NOT_SUPPORTED = 0,
		SUPPORTED = 1)

class ReaderInfo
{
	friend class Reader;

	ReaderManagerPlugInType mPlugInType;
	QString mName;
	ReaderType mReaderType;
	bool mBasicReader;
	CardInfo mCardInfo;
	bool mConnected;
	ExtendedLengthApduSupportCode mExtendedLengthApduSupportCode;

	public:
		ReaderInfo(ReaderManagerPlugInType pPlugInType = ReaderManagerPlugInType::UNKNOWN, const QString& pName = QString(), ReaderType pReaderType = ReaderType::UNKNOWN, const CardInfo& pCardInfo = CardInfo(CardType::NONE));

		bool isValid() const
		{
			return !mName.isNull();
		}


		ReaderManagerPlugInType getPlugInType() const
		{
			return mPlugInType;
		}


		const CardInfo& getCardInfo() const
		{
			return mCardInfo;
		}


		CardType getCardType() const
		{
			return mCardInfo.getCardType();
		}


		int getRetryCounter() const
		{
			return mCardInfo.getRetryCounter();
		}


		bool isPinDeactivated() const
		{
			return mCardInfo.isPinDeactivated();
		}


		void setCardInfo(const CardInfo& pCardInfo)
		{
			mCardInfo = pCardInfo;
		}


		const QString& getName() const
		{
			return mName;
		}


		ReaderType getReaderType() const
		{
			return mReaderType;
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


		void setExtendedLengthApduSupportCode(ExtendedLengthApduSupportCode pExtendedLengthApduSupportCode)
		{
			mExtendedLengthApduSupportCode = pExtendedLengthApduSupportCode;
		}


		ExtendedLengthApduSupportCode getExtendedLengthApduSupportCode() const
		{
			return mExtendedLengthApduSupportCode;
		}


};

} /* namespace governikus */

REGISTER_META_TYPE(ReaderInfo)
