/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */


#include "MockReaderManagerPlugIn.h"

#include <QDebug>

using namespace governikus;


MockReaderManagerPlugIn* MockReaderManagerPlugIn::mInstance = nullptr;

MockReaderManagerPlugIn::MockReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::UNKNOWN, true)
	, mReaders()
{
	mInstance = this;
}


MockReaderManagerPlugIn::~MockReaderManagerPlugIn()
{
	removeAllReader();
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


MockReader* MockReaderManagerPlugIn::addReader(const QString& pReaderName)
{
	auto reader = new MockReader(pReaderName);

	connect(reader, &Reader::fireCardInserted, this, &ReaderManagerPlugIn::fireCardInserted);
	connect(reader, &Reader::fireCardRemoved, this, &ReaderManagerPlugIn::fireCardRemoved);
	connect(reader, &Reader::fireCardRetryCounterChanged, this, &ReaderManagerPlugIn::fireCardRetryCounterChanged);

	mReaders.insert(pReaderName, reader);
	Q_EMIT fireReaderAdded(pReaderName);

	return reader;
}


void MockReaderManagerPlugIn::removeReader(const QString& pReaderName)
{
	if (auto reader = mReaders.take(pReaderName))
	{
		Q_EMIT fireReaderRemoved(reader->getName());
		delete reader;
	}
}


void MockReaderManagerPlugIn::removeAllReader()
{
	const auto& readerList = getReaders();
	for (auto reader : readerList)
	{
		removeReader(reader->getName());
	}
}
