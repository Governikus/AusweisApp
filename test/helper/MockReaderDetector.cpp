/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "MockReaderDetector.h"


using namespace governikus;


MockReaderDetector::MockReaderDetector(const QVector<UsbId>& pDevIds)
	: ReaderDetector()
	, mDevIds(pDevIds)
{
}


MockReaderDetector::~MockReaderDetector()
{
}


QVector<UsbId> MockReaderDetector::attachedDevIds() const
{
	return mDevIds;
}
