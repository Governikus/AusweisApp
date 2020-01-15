/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "IosReader.h"
#include "IosReaderManagerPlugIn.h"

#include <QLoggingCategory>

#import <CoreNFC/NFCReaderSession.h>
#import <CoreNFC/NFCTagReaderSession.h>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


void IosReaderManagerPlugIn::onReaderDisconncted()
{
	ReaderManagerPlugIn::stopScan();
}


IosReaderManagerPlugIn::IosReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::NFC, NFCTagReaderSession.readingAvailable, NFCTagReaderSession.readingAvailable)
	, mIosReader(nullptr)
{
}


IosReaderManagerPlugIn::~IosReaderManagerPlugIn()
{
}


QList<Reader*> IosReaderManagerPlugIn::getReaders() const
{
	if (getInfo().isEnabled())
	{
		return QList<Reader*>({mIosReader.data()});
	}

	return QList<Reader*>();
}


void IosReaderManagerPlugIn::init()
{
	ReaderManagerPlugIn::init();

	if (mIosReader)
	{
		return;
	}

	mIosReader.reset(new IosReader());
	connect(mIosReader.data(), &IosReader::fireCardInserted, this, &IosReaderManagerPlugIn::fireCardInserted);
	connect(mIosReader.data(), &IosReader::fireCardRemoved, this, &IosReaderManagerPlugIn::fireCardRemoved);
	connect(mIosReader.data(), &IosReader::fireCardRetryCounterChanged, this, &IosReaderManagerPlugIn::fireCardRetryCounterChanged);
	connect(mIosReader.data(), &IosReader::fireReaderPropertiesUpdated, this, &IosReaderManagerPlugIn::fireReaderPropertiesUpdated);
	connect(mIosReader.data(), &IosReader::fireReaderDisconnected, this, &IosReaderManagerPlugIn::onReaderDisconncted);
	qCDebug(card_nfc) << "Add reader" << mIosReader->getName();

	if (getInfo().isEnabled())
	{
		Q_EMIT fireReaderAdded(mIosReader->getName());
	}
}


void IosReaderManagerPlugIn::shutdown()
{
	mIosReader.reset();
}


void IosReaderManagerPlugIn::startScan(bool pAutoConnect)
{
	if (isScanRunning())
	{
		return;
	}

	if (mIosReader)
	{
		mIosReader->connectReader();
	}
	ReaderManagerPlugIn::startScan(pAutoConnect);
}


void IosReaderManagerPlugIn::stopScan(const QString& pError)
{
	if (!isScanRunning())
	{
		return;
	}

	if (mIosReader)
	{
		mIosReader->disconnectReader(pError);
	}
	ReaderManagerPlugIn::stopScan();
}
