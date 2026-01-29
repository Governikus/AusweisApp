/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

#include "MockReaderManagerPlugin.h"

#include <QDebug>


using namespace governikus;


MockReaderManagerPlugin* MockReaderManagerPlugin::mInstance = nullptr;

MockReaderManagerPlugin::MockReaderManagerPlugin()
	: ReaderManagerPlugin(MockReader::cMOCKED_READERMANAGER_TYPE, true)
	, mReaders()
{
	mInstance = this;
}


MockReaderManagerPlugin& MockReaderManagerPlugin::getInstance()
{
	if (!mInstance)
	{
		qFatal("MockReaderManagerPlugin not yet instantiated");
	}
	return *mInstance;
}


QPointer<Reader> MockReaderManagerPlugin::getReader(const QString& pReaderName) const
{
	return mReaders.value(pReaderName).data();
}


void MockReaderManagerPlugin::startScan(bool pAutoConnect)
{
	ReaderManagerPlugin::startScan(pAutoConnect);
	for (const auto& reader : std::as_const(mReaders))
	{
		const auto& readerInfo = reader->getReaderInfo();
		if (readerInfo.isInsertable())
		{
			Q_EMIT fireReaderPropertiesUpdated(reader->getReaderInfo());
			return;
		}

		Q_EMIT fireCardInfoChanged(readerInfo);
	}
}


QPointer<MockReader> MockReaderManagerPlugin::addReader(const QString& pReaderName, ReaderManagerPluginType pType)
{
	QSharedPointer<MockReader> mockReader;

	QMetaObject::invokeMethod(this, [this, pReaderName, pType] {
				const auto& reader = QSharedPointer<MockReader>::create(pReaderName, pType);

				connect(reader.data(), &Reader::fireCardInserted, this, &ReaderManagerPlugin::fireCardInserted);
				connect(reader.data(), &Reader::fireCardRemoved, this, &ReaderManagerPlugin::fireCardRemoved);
				connect(reader.data(), &Reader::fireCardInfoChanged, this, &ReaderManagerPlugin::fireCardInfoChanged);
				connect(reader.data(), &Reader::fireReaderPropertiesUpdated, this, &ReaderManagerPlugin::fireReaderPropertiesUpdated);

				mReaders.insert(pReaderName, reader);
				Q_EMIT fireReaderAdded(reader->getReaderInfo());

				return reader;
			}, Qt::BlockingQueuedConnection, &mockReader);

	QCoreApplication::processEvents();
	return mockReader.data();
}


void MockReaderManagerPlugin::removeReader(const QString& pReaderName)
{
	QMetaObject::invokeMethod(this, [this, pReaderName] {
				if (const auto& reader = mReaders.take(pReaderName))
				{
					Q_EMIT fireReaderRemoved(reader->getReaderInfo());
				}
			}, Qt::BlockingQueuedConnection);
	QCoreApplication::processEvents();
}


void MockReaderManagerPlugin::removeAllReader()
{
	QList<QSharedPointer<MockReader>> readerList;

	QMetaObject::invokeMethod(this, [this] {
				return mReaders.values();
			}, Qt::BlockingQueuedConnection, &readerList);

	for (const auto& reader : std::as_const(readerList))
	{
		removeReader(reader->getName());
	}
	QCoreApplication::processEvents();
}


void MockReaderManagerPlugin::insert(const QString& pReaderName, const QVariant& pData)
{
	for (const auto& reader : std::as_const(mReaders))
	{
		if (reader->getName() == pReaderName)
		{
			reader->insertCard(pData);
		}
	}
}


void MockReaderManagerPlugin::shelveAll() const
{
	for (const auto& reader : mReaders)
	{
		shelve(reader.data());
	}
}
