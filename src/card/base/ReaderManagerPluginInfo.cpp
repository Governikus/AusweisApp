/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManagerPluginInfo.h"

#include "Initializer.h"

using namespace governikus;

INIT_FUNCTION([] {
			qRegisterMetaType<ReaderManagerPluginInfo>("ReaderManagerPluginInfo");
		})


ReaderManagerPluginInfo::ReaderManagerPluginInfo(ReaderManagerPluginType pType, bool pEnabled, bool pAvailable)
	: mType(pType)
	, mValues()
	, mEnabled(pEnabled)
	, mAvailable(pAvailable)
	, mScanRunning(false)
	, mInitialScan(InitialScan::UNKNOWN)
{
}


#include "moc_ReaderManagerPluginInfo.cpp"
