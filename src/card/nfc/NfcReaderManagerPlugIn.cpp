/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "NfcBridge.h"
#include "NfcReader.h"
#include "NfcReaderManagerPlugIn.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


NfcReaderManagerPlugIn * NfcReaderManagerPlugIn::mInstance = nullptr;


#ifdef Q_OS_ANDROID
JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_NfcAdapterStateChangeReceiver_nfcAdapterStateChanged(JNIEnv* env, jobject obj, jint newState)
{
	Q_UNUSED(env)
	Q_UNUSED(obj)
	if (NfcReaderManagerPlugIn::mInstance == nullptr)
	{
		qCWarning(card_nfc) << "NfcReaderManagerPlugIn not yet instantiated, skip processing";
		return;
	}
	if (newState == 1) // STATE_OFF (class android.nfc.NfcAdapter)
	{
		qCDebug(card_nfc) << "Nfc was powered off";
		// jump from Android thread into Qt thread by invoking per BlockingQueuedConnection
		QMetaObject::invokeMethod(NfcReaderManagerPlugIn::mInstance, "setNfcStatus", Qt::BlockingQueuedConnection, Q_ARG(bool, false));
	}
	else if (newState == 3) // STATE_ON (class android.nfc.NfcAdapter)
	{
		qCDebug(card_nfc) << "Nfc was powered on";
		// jump from Android thread into Qt thread by invoking per BlockingQueuedConnection
		QMetaObject::invokeMethod(NfcReaderManagerPlugIn::mInstance, "setNfcStatus", Qt::BlockingQueuedConnection, Q_ARG(bool, true));
	}
}


#endif


NfcReaderManagerPlugIn::NfcReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::NFC, NfcBridge::getInstance().isNfcAvailable())
	, mInitialized(false)
	, mReaderList()
{
	NfcReaderManagerPlugIn::mInstance = this;
}


NfcReaderManagerPlugIn::~NfcReaderManagerPlugIn()
{
}


QList<Reader*> NfcReaderManagerPlugIn::getReader() const
{
	return mReaderList;
}


void NfcReaderManagerPlugIn::addNfcReader()
{
	NfcReader* reader = new NfcReader();

	connect(reader, &NfcReader::fireCardInserted, this, &NfcReaderManagerPlugIn::fireCardInserted);
	connect(reader, &NfcReader::fireCardRemoved, this, &NfcReaderManagerPlugIn::fireCardRemoved);
	connect(reader, &NfcReader::fireCardRetryCounterChanged, this, &NfcReaderManagerPlugIn::fireCardRetryCounterChanged);
	connect(reader, &NfcReader::fireReaderPropertiesUpdated, this, &NfcReaderManagerPlugIn::fireReaderPropertiesUpdated);

	mReaderList << reader;
	qCDebug(card_nfc) << "Add reader" << reader->getName();
	Q_EMIT fireReaderAdded(reader->getName());
}


void NfcReaderManagerPlugIn::init()
{
	ReaderManagerPlugIn::init();
#ifdef Q_OS_ANDROID
	if (!mInitialized && NfcBridge::getInstance().start())
	{
		mInitialized = true;
		setNfcStatus(NfcBridge::getInstance().isNfcEnabled());
	}
#endif
}


void NfcReaderManagerPlugIn::shutdown()
{
#ifdef Q_OS_ANDROID
	if (mInitialized && NfcBridge::getInstance().stop())
	{
		mInitialized = false;
	}
#endif
}


void NfcReaderManagerPlugIn::setNfcStatus(bool pEnabled)
{
	if (!mInitialized)
	{
		return;
	}

	setReaderInfoEnabled(pEnabled);
	if (pEnabled)
	{
		if (mReaderList.isEmpty())
		{
			addNfcReader();
		}
	}
	else
	{
		while (!mReaderList.isEmpty())
		{
			Reader* reader = mReaderList.takeFirst();
			qCDebug(card_nfc) << "Remove reader" << reader->getName();
			Q_EMIT fireReaderRemoved(reader->getName());
			delete reader;
		}
	}
}
