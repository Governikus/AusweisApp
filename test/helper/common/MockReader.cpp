/*!
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

#include "MockReader.h"

#include <QThread>

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
	, mEvent(CardEvent::NONE)
{
	mReaderInfo.setConnected(true);
	mReaderInfo.setBasicReader(true);
}


MockReader::~MockReader()
{
}


void MockReader::removeCard()
{
	// some unit tests uses MockReader without ReaderManager. So avoid a deadlock here.
	Qt::ConnectionType type = QThread::currentThread() == QObject::thread() ? Qt::DirectConnection : Qt::BlockingQueuedConnection;

	QMetaObject::invokeMethod(this, [this] {
				mCard.reset(nullptr);
				mReaderInfo.setCardInfo(CardInfo(CardType::NONE));
				Q_EMIT fireReaderPropertiesUpdated(mReaderInfo);
			}, type);
	QCoreApplication::processEvents();
}


MockCard* MockReader::setCard(const MockCardConfig& pCardConfig, const QByteArray& pEfCardAccess)
{
	setCard(pCardConfig, EFCardAccess::decode(pEfCardAccess));
	return mCard.data();
}


MockCard* MockReader::setCard(const MockCardConfig& pCardConfig, const QSharedPointer<EFCardAccess>& pEfCardAccess)
{
	// some unit tests uses MockReader without ReaderManager. So avoid a deadlock here.
	Qt::ConnectionType type = QThread::currentThread() == QObject::thread() ? Qt::DirectConnection : Qt::BlockingQueuedConnection;

	QMetaObject::invokeMethod(this, [this, pCardConfig, pEfCardAccess] {
				mCard.reset(new MockCard(pCardConfig));
				mReaderInfo.setCardInfo(CardInfo(CardType::EID_CARD, pEfCardAccess));
				Q_EMIT fireReaderPropertiesUpdated(mReaderInfo);
			}, type);

	QCoreApplication::processEvents();
	return mCard.data();
}


void MockReader::setReaderInfo(const ReaderInfo& pReaderInfo)
{
	// some unit tests uses MockReader without ReaderManager. So avoid a deadlock here.
	Qt::ConnectionType type = QThread::currentThread() == QObject::thread() ? Qt::DirectConnection : Qt::BlockingQueuedConnection;

	QMetaObject::invokeMethod(this, [this, pReaderInfo] {
				mReaderInfo = pReaderInfo;
				Q_EMIT fireReaderPropertiesUpdated(mReaderInfo);
			}, type);
	QCoreApplication::processEvents();
}
