/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */


#include "MockReaderManagerPlugIn.h"

#include <QDebug>

using namespace governikus;


MockReaderManagerPlugIn* MockReaderManagerPlugIn::mInstance = nullptr;

MockReaderManagerPlugIn::MockReaderManagerPlugIn()
	: ReaderManagerPlugIn(MockReader::cMOCKED_READERMANAGER_TYPE, true)
	, mReaders()
{
	mInstance = this;
}


MockReaderManagerPlugIn& MockReaderManagerPlugIn::getInstance()
{
	if (!mInstance)
	{
		qFatal("MockReaderManagerPlugIn not yet instantiated");
	}
	return *mInstance;
}


QList<Reader*> MockReaderManagerPlugIn::getReaders() const
{
	QList<Reader*> readers;
	readers.reserve(mReaders.size());
	for (MockReader* reader : mReaders)
	{
		readers += reader;
	}
	return readers;
}


MockReader* MockReaderManagerPlugIn::addReader(const QString& pReaderName, ReaderManagerPlugInType pType)
{
	MockReader* mockReader = nullptr;

	QMetaObject::invokeMethod(this, [this, pReaderName, pType] {
			auto* reader = new MockReader(pReaderName, pType);
			reader->setParent(this);

			connect(reader, &Reader::fireCardInserted, this, &ReaderManagerPlugIn::fireCardInserted);
			connect(reader, &Reader::fireCardRemoved, this, &ReaderManagerPlugIn::fireCardRemoved);
			connect(reader, &Reader::fireCardInfoChanged, this, &ReaderManagerPlugIn::fireCardInfoChanged);
			connect(reader, &Reader::fireReaderPropertiesUpdated, this, &ReaderManagerPlugIn::fireReaderPropertiesUpdated);

			mReaders.insert(pReaderName, reader);
			Q_EMIT fireReaderAdded(reader->getReaderInfo());

			return reader;
		}, Qt::BlockingQueuedConnection, &mockReader);

	QCoreApplication::processEvents();
	return mockReader;
}


void MockReaderManagerPlugIn::removeReader(const QString& pReaderName)
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


void MockReaderManagerPlugIn::removeAllReader()
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


void MockReaderManagerPlugIn::insert(const QString& pReaderName, const QVariant& pData)
{
	for (MockReader* reader : std::as_const(mReaders))
	{
		if (reader->getName() == pReaderName)
		{
			reader->insertCard(pData);
		}
	}
}
