/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManagerPlugin.h"


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


void ReaderManagerPlugin::setPluginEnabled(bool pEnabled)
{
	if (mInfo.isEnabled() != pEnabled)
	{
		mInfo.setEnabled(pEnabled);
		Q_EMIT fireStatusChanged(mInfo);
	}
}


void ReaderManagerPlugin::setPluginAvailable(bool pAvailable)
{
	mInfo.setAvailable(pAvailable);
	Q_EMIT fireStatusChanged(mInfo);
}


void ReaderManagerPlugin::setPluginValue(ReaderManagerPluginInfo::Key pKey, const QVariant& pValue)
{
	mInfo.setValue(pKey, pValue);
}


void ReaderManagerPlugin::shelve(const QPointer<Reader>& pReader)
{
	if (pReader && pReader->getReaderInfo().wasShelved())
	{
		pReader->shelveCard();
	}
}


ReaderManagerPlugin::ReaderManagerPlugin(ReaderManagerPluginType pPluginType,
		bool pAvailable,
		bool pPluginEnabled)
	: mInfo(pPluginType, pPluginEnabled, pAvailable)
{
}


[[nodiscard]] const ReaderManagerPluginInfo& ReaderManagerPlugin::getInfo() const
{
	return mInfo;
}


void ReaderManagerPlugin::init()
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());
}


void ReaderManagerPlugin::shutdown()
{
	qCDebug(card).nospace() << "Shutdown ReaderManagerPluginType::" << getInfo().getPluginType();
}


void ReaderManagerPlugin::reset()
{
	shutdown();
	init();
}


void ReaderManagerPlugin::startScan(bool /*pAutoConnect*/)
{
	if (!mInfo.isScanRunning())
	{
		mInfo.setScanRunning(true);
		Q_EMIT fireStatusChanged(mInfo);
	}
}


void ReaderManagerPlugin::stopScan(const QString& pError)
{
	Q_UNUSED(pError)

	if (mInfo.isScanRunning())
	{
		mInfo.setScanRunning(false);
		Q_EMIT fireStatusChanged(mInfo);
	}
}


void ReaderManagerPlugin::setInitialScanState(ReaderManagerPluginInfo::InitialScan pState)
{
	if (mInfo.getInitialScanState() != pState)
	{
		mInfo.setInitialScanState(pState);
		Q_EMIT fireStatusChanged(mInfo);
	}
}


void ReaderManagerPlugin::insert(const QString& pReaderName, const QVariant& pData)
{
	Q_UNUSED(pReaderName)
	Q_UNUSED(pData)

	qCDebug(card).nospace() << "insert is not supported by ReaderManagerPluginType::" << getInfo().getPluginType();
}


#ifndef QT_NO_DEBUG
void ReaderManagerPlugin::setPluginInfo(const ReaderManagerPluginInfo& pInfo)
{
	mInfo = pInfo;
	Q_EMIT fireStatusChanged(mInfo);
}


#endif
