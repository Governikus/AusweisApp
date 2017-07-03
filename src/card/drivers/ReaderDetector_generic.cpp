/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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


QVector<QPair<uint, uint> > ReaderDetector::attachedDevIds() const
{
	return QVector<QPair<uint, uint> >();
}
