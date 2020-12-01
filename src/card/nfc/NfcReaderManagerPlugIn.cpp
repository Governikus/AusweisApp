/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "NfcReaderManagerPlugIn.h"

#include <QLoggingCategory>
#include <QNearFieldManager>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


void NfcReaderManagerPlugIn::onNfcAdapterStateChanged(bool pEnabled)
{
	if (getInfo().isEnabled() == pEnabled)
	{
		return;
	}

	qCDebug(card_nfc) << "NfcAdapterStateChanged:" << pEnabled;
	setPlugInEnabled(pEnabled);
	if (pEnabled)
	{
		Q_EMIT fireReaderAdded(mNfcReader->getReaderInfo());
	}
	else
	{
		Q_EMIT fireReaderRemoved(mNfcReader->getReaderInfo());
	}
}


void NfcReaderManagerPlugIn::onReaderDisconnected()
{
	ReaderManagerPlugIn::stopScan();
}


NfcReaderManagerPlugIn::NfcReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::NFC,
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
			QNearFieldManager().isSupported(QNearFieldTarget::TagTypeSpecificAccess)
#else
			QNearFieldManager().isSupported()
#endif
			)
	, mNfcReader(nullptr)
{
}


NfcReaderManagerPlugIn::~NfcReaderManagerPlugIn()
{
}


QList<Reader*> NfcReaderManagerPlugIn::getReaders() const
{
	if (getInfo().isEnabled())
	{
		return QList<Reader*>({mNfcReader.data()});
	}

	return QList<Reader*>();
}


void NfcReaderManagerPlugIn::init()
{
	ReaderManagerPlugIn::init();

	if (mNfcReader)
	{
		return;
	}

	mNfcReader.reset(new NfcReader());
	connect(mNfcReader.data(), &NfcReader::fireCardInserted, this, &NfcReaderManagerPlugIn::fireCardInserted);
	connect(mNfcReader.data(), &NfcReader::fireCardRemoved, this, &NfcReaderManagerPlugIn::fireCardRemoved);
	connect(mNfcReader.data(), &NfcReader::fireCardRetryCounterChanged, this, &NfcReaderManagerPlugIn::fireCardRetryCounterChanged);
	connect(mNfcReader.data(), &NfcReader::fireReaderPropertiesUpdated, this, &NfcReaderManagerPlugIn::fireReaderPropertiesUpdated);
	connect(mNfcReader.data(), &NfcReader::fireNfcAdapterStateChanged, this, &NfcReaderManagerPlugIn::onNfcAdapterStateChanged);
	connect(mNfcReader.data(), &NfcReader::fireReaderDisconnected, this, &NfcReaderManagerPlugIn::onReaderDisconnected);
	qCDebug(card_nfc) << "Add reader" << mNfcReader->getName();

	onNfcAdapterStateChanged(mNfcReader->isEnabled());
}


void NfcReaderManagerPlugIn::shutdown()
{
	mNfcReader.reset();
}


void NfcReaderManagerPlugIn::startScan(bool pAutoConnect)
{
	mNfcReader->connectReader();
	ReaderManagerPlugIn::startScan(pAutoConnect);
}


void NfcReaderManagerPlugIn::stopScan(const QString& pError)
{
	mNfcReader->disconnectReader(pError);
	ReaderManagerPlugIn::stopScan(pError);
}
