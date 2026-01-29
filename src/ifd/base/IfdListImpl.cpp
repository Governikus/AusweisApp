/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
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


void IfdListImpl::update(const Discovery& pDiscovery)
{
	for (const QSharedPointer<IfdListEntry>& entry : std::as_const(mResponsiveList))
	{
		if (entry->containsIfdId(pDiscovery.getIfdId()))
		{
			entry->setLastSeenToNow();
			entry->setDiscovery(pDiscovery);
			Q_EMIT fireDeviceUpdated(entry);

			return;
		}
	}

	const auto& newDevice = QSharedPointer<IfdListEntry>::create(pDiscovery);
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

	erase_if(mResponsiveList, [this, &threshold](const auto& pEntry) {
				if (pEntry->getLastSeen() < threshold)
				{
					Q_EMIT fireDeviceVanished(pEntry);
					return true;
				}
				if (pEntry->cleanUpSeenTimestamps(mReaderResponsiveTimeout))
				{
					Q_EMIT fireDeviceUpdated(pEntry);
				}
				return false;
			});

	if (mResponsiveList.isEmpty())
	{
		mTimer.stop();
	}
}
