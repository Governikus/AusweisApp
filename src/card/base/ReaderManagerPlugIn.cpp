/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManagerPlugIn.h"

using namespace governikus;


ReaderManagerPlugIn::ReaderManagerPlugIn(ReaderManagerPlugInType pPlugInType,
		bool pAvailable,
		bool pPlugInEnabled)
	: mInfo(pPlugInType, pPlugInEnabled, pAvailable)
{
}


void ReaderManagerPlugIn::startScan(bool /*pAutoConnect*/)
{
}


void ReaderManagerPlugIn::stopScan()
{
}
