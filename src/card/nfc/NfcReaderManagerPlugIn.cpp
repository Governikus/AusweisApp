/*!
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "NfcReaderManagerPlugIn.h"

#include <QLoggingCategory>
#include <QNearFieldManager>

#ifdef Q_OS_ANDROID
	#include <QJniObject>
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
#if defined(Q_OS_ANDROID)
		if (QNativeInterface::QAndroidApplication::isActivityContext())
		{
			if (QJniObject activity = QNativeInterface::QAndroidApplication::context(); activity.isValid())
			{
				activity.callMethod<void>("resetNfcReaderMode");
			}
		}
#endif
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
			QNearFieldManager().isSupported(QNearFieldTarget::TagTypeSpecificAccess)
			)
	, mNfcReader(nullptr)
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
	connect(mNfcReader.data(), &NfcReader::fireCardInfoChanged, this, &NfcReaderManagerPlugIn::fireCardInfoChanged);
	connect(mNfcReader.data(), &NfcReader::fireReaderPropertiesUpdated, this, &NfcReaderManagerPlugIn::fireReaderPropertiesUpdated);
	connect(mNfcReader.data(), &NfcReader::fireNfcAdapterStateChanged, this, &NfcReaderManagerPlugIn::onNfcAdapterStateChanged);
	connect(mNfcReader.data(), &NfcReader::fireReaderDisconnected, this, &NfcReaderManagerPlugIn::onReaderDisconnected);
	qCDebug(card_nfc) << "Add reader" << mNfcReader->getName();

	onNfcAdapterStateChanged(mNfcReader->isEnabled() && getInfo().isAvailable());
}


void NfcReaderManagerPlugIn::shutdown()
{
	onNfcAdapterStateChanged(false);
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
