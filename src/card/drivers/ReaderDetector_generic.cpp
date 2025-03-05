/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderDetector.h"


using namespace governikus;


bool ReaderDetector::initNativeEvents()
{
	return false;
}


bool ReaderDetector::terminateNativeEvents()
{
	return false;
}


QList<UsbId> ReaderDetector::attachedDevIds() const
{
	return QList<UsbId>();
}
