/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "MockReaderDetector.h"


using namespace governikus;

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
QList<UsbId> ReaderDetector::attachedDevIds() const
{
	return QList<UsbId>();
}


#endif


MockReaderDetector::MockReaderDetector(const QList<UsbId>& pDevIds)
	: ReaderDetector()
	, mDevIds(pDevIds)
{
}


MockReaderDetector::~MockReaderDetector()
{
}


QList<UsbId> MockReaderDetector::attachedDevIds() const
{
	return mDevIds;
}
