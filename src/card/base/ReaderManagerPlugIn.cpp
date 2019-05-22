/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
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
	mScanRunning = true;
}


void ReaderManagerPlugIn::stopScan()
{
	mScanRunning = false;
}
