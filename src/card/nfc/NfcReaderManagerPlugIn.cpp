/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "NfcReader.h"
#include "NfcReaderManagerPlugIn.h"

#include <QLoggingCategory>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


namespace
{
bool isAvailable()
{
#ifdef Q_OS_ANDROID
	QAndroidJniObject context = QtAndroid::androidContext();
	if (context == nullptr)
	{
		qCCritical(card_nfc) << "Cannot get context";
		return false;
	}

	return QAndroidJniObject::callStaticObjectMethod("android/nfc/NfcAdapter", "getDefaultAdapter", "(Landroid/content/Context;)Landroid/nfc/NfcAdapter;", context.object()) != nullptr;

#else
	QNearFieldManager manager;
	return manager.isAvailable();

#endif
}


} // namespace


void NfcReaderManagerPlugIn::onNfcAdapterStateChanged(bool pEnabled)
{
	if (mEnabled == pEnabled)
	{
		return;
	}

	qCDebug(card_nfc) << "NfcAdapterStateChanged:" << pEnabled;
	mEnabled = pEnabled;
	setReaderInfoEnabled(pEnabled);
	if (pEnabled)
	{
		Q_EMIT fireReaderAdded(mNfcReader->getName());
	}
	else
	{
		Q_EMIT fireReaderRemoved(mNfcReader->getName());
	}
}


NfcReaderManagerPlugIn::NfcReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::NFC, isAvailable())
	, mEnabled(false)
	, mNfcReader(nullptr)
{
}


NfcReaderManagerPlugIn::~NfcReaderManagerPlugIn()
{
}


QList<Reader*> NfcReaderManagerPlugIn::getReaders() const
{
	if (mEnabled)
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
	qCDebug(card_nfc) << "Add reader" << mNfcReader->getName();

	if (mEnabled)
	{
		Q_EMIT fireReaderAdded(mNfcReader->getName());
	}
}


void NfcReaderManagerPlugIn::shutdown()
{
	mNfcReader.reset();
}
