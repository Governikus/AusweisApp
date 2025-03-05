/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "MockReader.h"

#include <QThread>

using namespace governikus;


ReaderManagerPluginType MockReader::cMOCKED_READERMANAGER_TYPE = ReaderManagerPluginType::MOCK;

MockReader* MockReader::createMockReader(const QList<TransmitConfig>& pTransmitConfig, const QByteArray& pEfCardAccess)
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


MockReader* MockReader::createMockReader(const QList<TransmitConfig>& pTransmitConfig, const QSharedPointer<EFCardAccess>& pEfCardAccess)
{
	MockCardConfig cardConfig(pTransmitConfig);
	MockReader* reader = new MockReader();
	reader->setCard(cardConfig, pEfCardAccess);
	return reader;
}


MockReader::MockReader(const QString& pReaderName, ReaderManagerPluginType pType)
	: Reader(pType, pReaderName)
	, mCard(nullptr)
{
	setInfoBasicReader(true);
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
				removeCardInfo();
				Q_EMIT fireReaderPropertiesUpdated(getReaderInfo());
			}, type);
	QCoreApplication::processEvents();
}


MockCard* MockReader::setCard(const MockCardConfig& pCardConfig, const QByteArray& pEfCardAccess, CardType pType)
{
	setCard(pCardConfig, EFCardAccess::decode(pEfCardAccess), pType);
	return mCard.data();
}


MockCard* MockReader::setCard(const MockCardConfig& pCardConfig, const QSharedPointer<EFCardAccess>& pEfCardAccess, CardType pType, const FileRef& pApplication)
{
	// some unit tests uses MockReader without ReaderManager. So avoid a deadlock here.
	Qt::ConnectionType type = QThread::currentThread() == QObject::thread() ? Qt::DirectConnection : Qt::BlockingQueuedConnection;

	QMetaObject::invokeMethod(this, [this, pCardConfig, pEfCardAccess, pType, pApplication] {
				mCard.reset(new MockCard(pCardConfig));
				setInfoCardInfo(CardInfo(pType, pApplication, pEfCardAccess));
				Q_EMIT fireReaderPropertiesUpdated(getReaderInfo());
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
				Q_EMIT fireReaderPropertiesUpdated(pReaderInfo);
			}, type);
	QCoreApplication::processEvents();
}


void MockReader::setInfoBasicReader(bool pBasicReader)
{
	Reader::setInfoBasicReader(pBasicReader);
}


void MockReader::setInfoCardInfo(const CardInfo& pCardInfo)
{
	Reader::setInfoCardInfo(pCardInfo);
}
