/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManagerPlugIn.h"

using namespace governikus;


void ReaderManagerPlugIn::onConnectToKnownReadersChanged()
{
}


ReaderManagerPlugIn::ReaderManagerPlugIn(ReaderManagerPlugInType pPlugInType,
		bool pAvailable,
		bool pPlugInEnabled)
	: mInfo(pPlugInType, pPlugInEnabled, pAvailable)
	, mScanInProgress(false)
	, mConnectToKnownReaders(false)
{
}


void ReaderManagerPlugIn::startScan()
{
	mScanInProgress = true;
}


void ReaderManagerPlugIn::stopScan()
{
	mScanInProgress = false;
}


void ReaderManagerPlugIn::setConnectToKnownReaders(bool pConnectToKnownReaders)
{
	if (mConnectToKnownReaders != pConnectToKnownReaders)
	{
		mConnectToKnownReaders = pConnectToKnownReaders;
		onConnectToKnownReadersChanged();
	}
}
