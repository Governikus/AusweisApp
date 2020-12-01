/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManagerPlugIn.h"

using namespace governikus;


ReaderManagerPlugIn::ReaderManagerPlugIn(ReaderManagerPlugInType pPlugInType,
		bool pAvailable,
		bool pPlugInEnabled)
	: mInfo(pPlugInType, pPlugInEnabled, pAvailable)
	, mScanRunning(false)
{
}


void ReaderManagerPlugIn::startScan(bool /*pAutoConnect*/)
{
	if (!mScanRunning)
	{
		mScanRunning = true;
		Q_EMIT fireStatusChanged(mInfo);
	}
}


void ReaderManagerPlugIn::stopScan(const QString& pError)
{
	Q_UNUSED(pError)

	if (mScanRunning)
	{
		mScanRunning = false;
		Q_EMIT fireStatusChanged(mInfo);
	}
}
