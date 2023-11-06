/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "NfcReaderManagerPlugIn.h"

#include <QLoggingCategory>
#include <QNearFieldManager>

#ifdef Q_OS_ANDROID
	#include <QJniObject>
#endif

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


QAtomicPointer<NfcReaderManagerPlugIn> NfcReaderManagerPlugIn::instance = nullptr;


void NfcReaderManagerPlugIn::onNfcAdapterStateChanged(bool pEnabled)
{
	if (getInfo().isEnabled() == pEnabled || !mNfcReader)
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


void NfcReaderManagerPlugIn::setReaderMode(bool pEnabled)
{
#ifdef Q_OS_ANDROID
	if (QNativeInterface::QAndroidApplication::isActivityContext())
	{
		if (QJniObject activity = QNativeInterface::QAndroidApplication::context(); activity.isValid())
		{
			activity.callMethod<void>("setReaderMode", "(Z)V", pEnabled);
		}
	}
#else
	Q_UNUSED(pEnabled)
#endif
}


void NfcReaderManagerPlugIn::enqueueReaderMode(bool pEnabled)
{
	if (auto* plugin = NfcReaderManagerPlugIn::instance.loadRelaxed())
	{
		QMetaObject::invokeMethod(plugin, [plugin, pEnabled] {
				if (plugin->mNfcReader)
				{
					setReaderMode(pEnabled);
				}
			}, Qt::QueuedConnection);
	}
}


#ifdef Q_OS_ANDROID
extern "C"
{

// These functions need to be explicitly exported so that the JVM can bind to them.
JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_MainActivity_setReaderModeNative(JNIEnv* pEnv, jobject pObj, jboolean pEnabled)
{
	Q_UNUSED(pEnv)
	Q_UNUSED(pObj)

	NfcReaderManagerPlugIn::enqueueReaderMode(pEnabled);
}


}
#endif


NfcReaderManagerPlugIn::NfcReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::NFC,
			QNearFieldManager().isSupported(QNearFieldTarget::TagTypeSpecificAccess)
			)
	, mNfcReader(nullptr)
{
	instance = this;
}


NfcReaderManagerPlugIn::~NfcReaderManagerPlugIn()
{
	instance = nullptr;
}


QList<Reader*> NfcReaderManagerPlugIn::getReaders() const
{
	if (getInfo().isEnabled() && mNfcReader)
	{
		return QList<Reader*>({mNfcReader.data()});
	}

	return QList<Reader*>();
}


void NfcReaderManagerPlugIn::init()
{
	ReaderManagerPlugIn::init();

	if (!getInfo().isAvailable() || mNfcReader)
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

	setReaderMode(true);
	onNfcAdapterStateChanged(mNfcReader->isEnabled());
}


void NfcReaderManagerPlugIn::shutdown()
{
	if (mNfcReader)
	{
		onNfcAdapterStateChanged(false);
		setReaderMode(false);
		mNfcReader.reset();
	}
}


void NfcReaderManagerPlugIn::startScan(bool pAutoConnect)
{
	if (mNfcReader)
	{
		mNfcReader->connectReader();
		ReaderManagerPlugIn::startScan(pAutoConnect);
	}
}


void NfcReaderManagerPlugIn::stopScan(const QString& pError)
{
	if (mNfcReader)
	{
		mNfcReader->disconnectReader(pError);
		ReaderManagerPlugIn::stopScan(pError);
	}
}
