/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "MockReader.h"

using namespace governikus;


MockReader* MockReader::createMockReader(const QVector<TransmitConfig>& pTransmitConfig, const QByteArray& pEfCardAccess)
{
	if (pEfCardAccess.isNull())
	{
		return createMockReader(pTransmitConfig, QSharedPointer<EFCardAccess>());
	}
	else
	{
		return createMockReader(pTransmitConfig, EFCardAccess::decode(pEfCardAccess));
	}
}


MockReader* MockReader::createMockReader(const QVector<TransmitConfig>& pTransmitConfig, const QSharedPointer<EFCardAccess>& pEfCardAccess)
{
	MockCardConfig cardConfig(pTransmitConfig);
	QScopedPointer<MockReader> reader(new MockReader());
	reader->setCard(cardConfig, pEfCardAccess);
	return reader.take();
}


MockReader::MockReader(const QString& pReaderName)
	: Reader(ReaderManagerPlugInType::UNKNOWN, pReaderName)
	, mCard(nullptr)
{
	mReaderInfo.setConnected(true);
	mReaderInfo.setBasicReader(true);
}


MockReader::~MockReader()
{
}


MockCard* MockReader::setCard(const MockCardConfig& pCardConfig, const QByteArray& pEfCardAccess)
{
	setCard(pCardConfig, EFCardAccess::decode(pEfCardAccess));
	return mCard.data();
}


MockCard* MockReader::setCard(const MockCardConfig& pCardConfig, const QSharedPointer<EFCardAccess>& pEfCardAccess)
{
	mCard.reset(new MockCard(pCardConfig));
	mReaderInfo.setCardInfo(CardInfo(CardType::EID_CARD, pEfCardAccess));
	return mCard.data();
}
