/*!
 * \brief Reader mock for tests
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "asn1/SecurityInfos.h"
#include "MockCard.h"
#include "Reader.h"


namespace governikus
{

class MockReader
	: public Reader
{
	Q_OBJECT

	QScopedPointer<MockCard> mCard;

	public:
		static MockReader* createMockReader(const QVector<TransmitConfig>& pTransmitConfig = QVector<TransmitConfig>(), const QByteArray& pEfCardAccess = QByteArray());
		static MockReader* createMockReader(const QVector<TransmitConfig>& pTransmitConfig, const QSharedPointer<EFCardAccess>& pEfCardAccess);

		MockReader(const QString& pReaderName = QStringLiteral("MockReader"));
		virtual ~MockReader() override;


		Card* getCard() const override
		{
			return mCard.data();
		}


		void removeCard()
		{
			mCard.reset(nullptr);
			mReaderInfo.setCardInfo(CardInfo(CardType::NONE));
		}


		MockCard* setCard(const MockCardConfig& pCardConfig, const QByteArray& pEfCardAccess);
		MockCard* setCard(const MockCardConfig& pCardConfig, const QSharedPointer<EFCardAccess>& pEfCardAccess = QSharedPointer<EFCardAccess>());

		ReaderInfo& getReaderInfo()
		{
			return mReaderInfo;
		}


	private:
		virtual Reader::CardEvent updateCard() override
		{
			return Reader::CardEvent::NONE;
		}


};

} /* namespace governikus */
