/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "ReaderDriverContext.h"

#include "ReaderManager.h"

using namespace governikus;

ReaderDriverContext::ReaderDriverContext()
	: QObject()
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
	, mReaderDetector(QSharedPointer<ReaderDetector>(new ReaderDetector(getDriverSettings())))
#endif
{
	#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
	connect(mReaderDetector.data(), &ReaderDetector::fireReaderChangeDetected, this, &ReaderDriverContext::fireReaderChangeDetected);
	#endif
}


ReaderDriverContext::~ReaderDriverContext()
{
}


#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
QVector<QSharedPointer<DeviceDescriptor> > ReaderDriverContext::attachedDevices()
{
	const QVector<ReaderInfo> readerInfos = ReaderManager::getInstance().getReaderInfos();
	return mReaderDetector->getAttachedDevices(readerInfos);
}


#endif


QSharedPointer<DriverSettings> ReaderDriverContext::getDriverSettings() const
{
	QSharedPointer<DriverSettings> settings(new DriverSettings());
	settings->load();

	return settings;
}
