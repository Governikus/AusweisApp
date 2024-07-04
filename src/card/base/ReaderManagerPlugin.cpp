/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManagerPlugin.h"


using namespace governikus;


ReaderManagerPlugin::ReaderManagerPlugin(ReaderManagerPluginType pPluginType,
		bool pAvailable,
		bool pPluginEnabled)
	: mInfo(pPluginType, pPluginEnabled, pAvailable)
{
}


void ReaderManagerPlugin::shelve() const
{
	const auto& readers = getReaders();
	for (const auto& reader : readers)
	{
		if (reader->getReaderInfo().wasShelved())
		{
			reader->shelveCard();
		}
	}
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
