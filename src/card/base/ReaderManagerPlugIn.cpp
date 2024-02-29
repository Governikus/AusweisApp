/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManagerPlugIn.h"


using namespace governikus;


ReaderManagerPlugIn::ReaderManagerPlugIn(ReaderManagerPlugInType pPlugInType,
		bool pAvailable,
		bool pPlugInEnabled)
	: mInfo(pPlugInType, pPlugInEnabled, pAvailable)
{
}


void ReaderManagerPlugIn::shelve() const
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


void ReaderManagerPlugIn::startScan(bool /*pAutoConnect*/)
{
	if (!mInfo.isScanRunning())
	{
		mInfo.setScanRunning(true);
		Q_EMIT fireStatusChanged(mInfo);
	}
}


void ReaderManagerPlugIn::stopScan(const QString& pError)
{
	Q_UNUSED(pError)

	if (mInfo.isScanRunning())
	{
		mInfo.setScanRunning(false);
		Q_EMIT fireStatusChanged(mInfo);
	}
}
