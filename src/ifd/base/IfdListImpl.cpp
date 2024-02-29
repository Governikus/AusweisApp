/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "IfdListImpl.h"


using namespace governikus;


IfdListImpl::IfdListImpl(int pCheckInterval, int pReaderResponsiveTimeout)
	: IfdList()
	, mTimer()
	, mReaderResponsiveTimeout(pReaderResponsiveTimeout)
	, mResponsiveList()
{
	connect(&mTimer, &QTimer::timeout, this, &IfdListImpl::onProcessUnresponsiveRemoteReaders);
	pCheckInterval = pCheckInterval / 2 - 1;  // Nyquist-Shannon sampling theorem. Enable smooth UI updates.
	mTimer.setInterval(pCheckInterval);
}


IfdListImpl::~IfdListImpl()
{
	mTimer.stop();
}


void IfdListImpl::update(const IfdDescriptor& pDescriptor)
{
	for (const QSharedPointer<IfdListEntry>& entry : std::as_const(mResponsiveList))
	{
		if (entry->containsEquivalent(pDescriptor))
		{
			entry->setLastSeenToNow();
			entry->setIfdDescriptor(pDescriptor);
			Q_EMIT fireDeviceUpdated(entry);

			return;
		}
	}

	const auto& newDevice = QSharedPointer<IfdListEntry>::create(pDescriptor);
	mResponsiveList += newDevice;

	if (!mTimer.isActive())
	{
		mTimer.start();
	}

	Q_EMIT fireDeviceAppeared(newDevice);
}


void IfdListImpl::clear()
{
	decltype(mResponsiveList) removedDevices;
	mResponsiveList.swap(removedDevices);
	for (const auto& entry : std::as_const(removedDevices))
	{
		Q_EMIT fireDeviceVanished(entry);
	}
}


QList<QSharedPointer<IfdListEntry>> IfdListImpl::getIfdList() const
{
	return mResponsiveList;
}


void IfdListImpl::onProcessUnresponsiveRemoteReaders()
{
	const QTime threshold(QTime::currentTime().addMSecs(-mReaderResponsiveTimeout));
	QMutableListIterator<QSharedPointer<IfdListEntry>> i(mResponsiveList);
	while (i.hasNext())
	{
		const QSharedPointer<IfdListEntry> entry = i.next();
		if (entry->getLastSeen() < threshold)
		{
			i.remove();
			Q_EMIT fireDeviceVanished(entry);
			continue;
		}

		if (entry->cleanUpSeenTimestamps(mReaderResponsiveTimeout))
		{
			Q_EMIT fireDeviceUpdated(entry);
		}
	}

	if (mResponsiveList.isEmpty())
	{
		mTimer.stop();
	}
}
