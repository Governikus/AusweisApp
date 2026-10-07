/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
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
