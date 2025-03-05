/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
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


QList<Reader*> MockReaderManagerPlugin::getReaders() const
{
	QList<Reader*> readers;
	readers.reserve(mReaders.size());
	for (MockReader* reader : mReaders)
	{
		readers += reader;
	}
	return readers;
}


void MockReaderManagerPlugin::startScan(bool pAutoConnect)
{
	ReaderManagerPlugin::startScan(pAutoConnect);
	for (MockReader* reader : std::as_const(mReaders))
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


MockReader* MockReaderManagerPlugin::addReader(const QString& pReaderName, ReaderManagerPluginType pType)
{
	MockReader* mockReader = nullptr;

	QMetaObject::invokeMethod(this, [this, pReaderName, pType] {
				auto* reader = new MockReader(pReaderName, pType);
				reader->setParent(this);

				connect(reader, &Reader::fireCardInserted, this, &ReaderManagerPlugin::fireCardInserted);
				connect(reader, &Reader::fireCardRemoved, this, &ReaderManagerPlugin::fireCardRemoved);
				connect(reader, &Reader::fireCardInfoChanged, this, &ReaderManagerPlugin::fireCardInfoChanged);
				connect(reader, &Reader::fireReaderPropertiesUpdated, this, &ReaderManagerPlugin::fireReaderPropertiesUpdated);

				mReaders.insert(pReaderName, reader);
				Q_EMIT fireReaderAdded(reader->getReaderInfo());

				return reader;
			}, Qt::BlockingQueuedConnection, &mockReader);

	QCoreApplication::processEvents();
	return mockReader;
}


void MockReaderManagerPlugin::removeReader(const QString& pReaderName)
{
	QMetaObject::invokeMethod(this, [this, pReaderName] {
				if (auto reader = mReaders.take(pReaderName))
				{
					Q_EMIT fireReaderRemoved(reader->getReaderInfo());
					delete reader;
				}
			}, Qt::BlockingQueuedConnection);
	QCoreApplication::processEvents();
}


void MockReaderManagerPlugin::removeAllReader()
{
	QList<Reader*> readerList;

	QMetaObject::invokeMethod(this, [this] {
				return getReaders();
			}, Qt::BlockingQueuedConnection, &readerList);

	for (auto reader : std::as_const(readerList))
	{
		removeReader(reader->getName());
	}
	QCoreApplication::processEvents();
}


void MockReaderManagerPlugin::insert(const QString& pReaderName, const QVariant& pData)
{
	for (MockReader* reader : std::as_const(mReaders))
	{
		if (reader->getName() == pReaderName)
		{
			reader->insertCard(pData);
		}
	}
}
