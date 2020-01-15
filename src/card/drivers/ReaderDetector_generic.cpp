/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
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


QVector<UsbId> ReaderDetector::attachedDevIds() const
{
	return QVector<UsbId>();
}
