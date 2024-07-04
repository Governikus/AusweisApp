/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "NfcReaderManagerPlugin.h"

#include <QLoggingCategory>
#include <QNearFieldManager>

#ifdef Q_OS_ANDROID
	#include <QJniObject>
#endif

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


QAtomicPointer<NfcReaderManagerPlugin> NfcReaderManagerPlugin::instance = nullptr;


void NfcReaderManagerPlugin::onNfcAdapterStateChanged(bool pEnabled)
{
	if (getInfo().isEnabled() == pEnabled || !mNfcReader)
	{
		return;
	}

	qCDebug(card_nfc) << "NfcAdapterStateChanged:" << pEnabled;
	setPluginEnabled(pEnabled);
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


void NfcReaderManagerPlugin::onReaderDisconnected()
{
	ReaderManagerPlugin::stopScan();
}


void NfcReaderManagerPlugin::setReaderMode(bool pEnabled)
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


void NfcReaderManagerPlugin::enqueueReaderMode(bool pEnabled)
{
	if (auto* plugin = NfcReaderManagerPlugin::instance.loadRelaxed())
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

	NfcReaderManagerPlugin::enqueueReaderMode(pEnabled);
}


}
#endif


NfcReaderManagerPlugin::NfcReaderManagerPlugin()
	: ReaderManagerPlugin(ReaderManagerPluginType::NFC,
			QNearFieldManager().isSupported(QNearFieldTarget::TagTypeSpecificAccess)
			)
	, mNfcReader(nullptr)
{
	instance = this;
}


NfcReaderManagerPlugin::~NfcReaderManagerPlugin()
{
	instance = nullptr;
}


QList<Reader*> NfcReaderManagerPlugin::getReaders() const
{
	if (getInfo().isEnabled() && mNfcReader)
	{
		return QList<Reader*>({mNfcReader.data()});
	}

	return QList<Reader*>();
}


void NfcReaderManagerPlugin::init()
{
	ReaderManagerPlugin::init();

	if (!getInfo().isAvailable() || mNfcReader)
	{
		return;
	}

	mNfcReader.reset(new NfcReader());
	connect(mNfcReader.data(), &NfcReader::fireCardInserted, this, &NfcReaderManagerPlugin::fireCardInserted);
	connect(mNfcReader.data(), &NfcReader::fireCardRemoved, this, &NfcReaderManagerPlugin::fireCardRemoved);
	connect(mNfcReader.data(), &NfcReader::fireCardInfoChanged, this, &NfcReaderManagerPlugin::fireCardInfoChanged);
	connect(mNfcReader.data(), &NfcReader::fireReaderPropertiesUpdated, this, &NfcReaderManagerPlugin::fireReaderPropertiesUpdated);
	connect(mNfcReader.data(), &NfcReader::fireNfcAdapterStateChanged, this, &NfcReaderManagerPlugin::onNfcAdapterStateChanged);
	connect(mNfcReader.data(), &NfcReader::fireReaderDisconnected, this, &NfcReaderManagerPlugin::onReaderDisconnected);
	qCDebug(card_nfc) << "Add reader" << mNfcReader->getName();

	setReaderMode(true);
	onNfcAdapterStateChanged(mNfcReader->isEnabled());
}


void NfcReaderManagerPlugin::shutdown()
{
	if (mNfcReader)
	{
		onNfcAdapterStateChanged(false);
		setReaderMode(false);
		mNfcReader.reset();
	}
}


void NfcReaderManagerPlugin::startScan(bool pAutoConnect)
{
	if (mNfcReader)
	{
		mNfcReader->connectReader();
		ReaderManagerPlugin::startScan(pAutoConnect);
	}
}


void NfcReaderManagerPlugin::stopScan(const QString& pError)
{
	if (mNfcReader)
	{
		mNfcReader->disconnectReader(pError);
		ReaderManagerPlugin::stopScan(pError);
	}
}
