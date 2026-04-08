/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardInfo.h"
#include "ReaderConfigurationInfo.h"
#include "ReaderManagerPluginInfo.h"

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
		[[nodiscard]] ReaderManagerPluginType getPluginType() const;
		[[nodiscard]] bool isValid() const;
		void invalidate();
		[[nodiscard]] CardInfo& getCardInfo();
		[[nodiscard]] const CardInfo& getCardInfo() const;
		[[nodiscard]] CardType getCardType() const;
		[[nodiscard]] QString getCardTypeString() const;

		[[nodiscard]] bool hasCard() const;
		[[nodiscard]] bool hasEid() const;

		[[nodiscard]] int getRetryCounter() const;
		[[nodiscard]] bool isRetryCounterDetermined() const;
		[[nodiscard]] bool isPinDeactivated() const;
		[[nodiscard]] bool isPukInoperative() const;

		[[nodiscard]] bool wasShelved() const;
		void shelveCard();
		[[nodiscard]] bool isInsertable() const;
		void insertCard();


		void setCardInfo(const CardInfo& pCardInfo);
		[[nodiscard]] const QString& getName() const;
		void setBasicReader(bool pIsBasicReader);
		[[nodiscard]] bool isBasicReader() const;

		void setMaxApduLength(int pMaxApduLength);
		[[nodiscard]] int getMaxApduLength() const;
		[[nodiscard]] bool insufficientApduLength() const;
};

} // namespace governikus
