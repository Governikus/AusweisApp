/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "MockCard.h"
#include "Reader.h"
#include "asn1/SecurityInfos.h"


namespace governikus
{

class MockReader
	: public Reader
{
	Q_OBJECT

	QScopedPointer<MockCard> mCard;

	public:
		static ReaderManagerPluginType cMOCKED_READERMANAGER_TYPE;

		static MockReader* createMockReader(const QList<TransmitConfig>& pTransmitConfig = QList<TransmitConfig>(), const QByteArray& pEfCardAccess = QByteArray());
		static MockReader* createMockReader(const QList<TransmitConfig>& pTransmitConfig, const QSharedPointer<EFCardAccess>& pEfCardAccess);

		MockReader(const QString& pReaderName = QStringLiteral("MockReader"), ReaderManagerPluginType pType = cMOCKED_READERMANAGER_TYPE);
		~MockReader() override;


		[[nodiscard]] Card* getCard() const override
		{
			return mCard.data();
		}


		void removeCard();

		MockCard* setCard(const MockCardConfig& pCardConfig, const QByteArray& pEfCardAccess, CardType pType = CardType::EID_CARD);
		MockCard* setCard(const MockCardConfig& pCardConfig, const QSharedPointer<EFCardAccess>& pEfCardAccess = QSharedPointer<EFCardAccess>(), CardType pType = CardType::EID_CARD, const FileRef& pApplication = FileRef());

		void setReaderInfo(const ReaderInfo& pReaderInfo);
		void setInfoBasicReader(bool pBasicReader);
		void setInfoCardInfo(const CardInfo& pCardInfo);
};

} // namespace governikus
