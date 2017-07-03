/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "ReaderDriverContext.h"

#include "DeviceDescriptor.h"
#include "ReaderManager.h"


using namespace governikus;

ReaderDriverContext::ReaderDriverContext()
	: mReaderDetector(QSharedPointer<ReaderDetector>(new ReaderDetector(getDriverSettings())))
{
	connect(mReaderDetector.data(), &ReaderDetector::fireReaderChangeDetected, this, &ReaderDriverContext::fireReaderChangeDetected);
}


ReaderDriverContext::~ReaderDriverContext()
{
}


QVector<QSharedPointer<DeviceDescriptor> > ReaderDriverContext::attachedDevices()
{
	const QVector<ReaderInfo> readerInfos = ReaderManager::getInstance().getReaderInfos();
	return mReaderDetector->getAttachedDevices(readerInfos);
}


QSharedPointer<DriverSettings> ReaderDriverContext::getDriverSettings() const
{
	QSharedPointer<DriverSettings> settings(new DriverSettings());
	settings->load();

	return settings;
}
