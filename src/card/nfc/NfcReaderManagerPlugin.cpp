/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
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
	if (getInfo().isEnabled() == pEnabled || !mReader)
	{
		return;
	}

	qCDebug(card_nfc) << "NfcAdapterStateChanged:" << pEnabled;
	setPluginEnabled(pEnabled);
	if (pEnabled)
	{
		Q_EMIT fireReaderAdded(mReader->getReaderInfo());
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
		Q_EMIT fireReaderRemoved(mReader->getReaderInfo());
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
					if (plugin->mReader)
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
	, mReader(nullptr)
{
	instance = this;
}


NfcReaderManagerPlugin::~NfcReaderManagerPlugin()
{
	instance = nullptr;
}


QPointer<Reader> NfcReaderManagerPlugin::getReader(const QString& pReaderName) const
{
	if (getInfo().isEnabled() && mReader && mReader->getName() == pReaderName)
	{
		return mReader.data();
	}

	return nullptr;
}


void NfcReaderManagerPlugin::init()
{
	ReaderManagerPlugin::init();

	if (!getInfo().isAvailable() || mReader)
	{
		return;
	}

	mReader.reset(new NfcReader());
	connect(mReader.data(), &NfcReader::fireCardInserted, this, &NfcReaderManagerPlugin::fireCardInserted);
	connect(mReader.data(), &NfcReader::fireCardRemoved, this, &NfcReaderManagerPlugin::fireCardRemoved);
	connect(mReader.data(), &NfcReader::fireCardInfoChanged, this, &NfcReaderManagerPlugin::fireCardInfoChanged);
	connect(mReader.data(), &NfcReader::fireReaderPropertiesUpdated, this, &NfcReaderManagerPlugin::fireReaderPropertiesUpdated);
	connect(mReader.data(), &NfcReader::fireNfcAdapterStateChanged, this, &NfcReaderManagerPlugin::onNfcAdapterStateChanged);
	connect(mReader.data(), &NfcReader::fireReaderDisconnected, this, &NfcReaderManagerPlugin::onReaderDisconnected);
	qCDebug(card_nfc) << "Add reader" << mReader->getName();

	setReaderMode(true);
	onNfcAdapterStateChanged(mReader->isEnabled());
}


void NfcReaderManagerPlugin::shutdown()
{
	if (mReader)
	{
		onNfcAdapterStateChanged(false);
		setReaderMode(false);
		mReader.reset();
	}
}


void NfcReaderManagerPlugin::startScan(bool pAutoConnect)
{
	if (mReader)
	{
		mReader->connectReader();
		ReaderManagerPlugin::startScan(pAutoConnect);
	}
}


void NfcReaderManagerPlugin::stopScan(const QString& pError)
{
	if (mReader)
	{
		mReader->disconnectReader(pError);
		ReaderManagerPlugin::stopScan(pError);
	}
}


void NfcReaderManagerPlugin::shelveAll() const
{
	if (getInfo().isEnabled() && mReader)
	{
		shelve(mReader.data());
	}
}
