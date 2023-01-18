/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MockReaderDetector.h"


using namespace governikus;

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
QVector<UsbId> ReaderDetector::attachedDevIds() const
{
	return QVector<UsbId>();
}


#endif


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
