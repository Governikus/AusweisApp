/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#include "IfdListEntry.h"
#include "Initializer.h"

#include <QSharedPointer>


using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<IfdListEntry>>("QSharedPointer<IfdListEntry>");
			qRegisterMetaType<QList<QSharedPointer<IfdListEntry>>>("QList<QSharedPointer<IfdListEntry>>");
		})


IfdListEntry::IfdListEntry(const Discovery& pDiscovery)
	: mDiscovery(pDiscovery)
	, mLastSeen(QTime::currentTime())
	, mLastSeenHistory()
{
}


void IfdListEntry::setLastSeenToNow()
{
	if (mLastSeen.isValid())
	{
		mLastSeenHistory += mLastSeen;
	}

	mLastSeen = QTime::currentTime();
}


bool IfdListEntry::cleanUpSeenTimestamps(int pReaderResponsiveTimeout)
{
	const auto visibilityOld = getPercentSeen();
	const QTime threshold(QTime::currentTime().addMSecs(-pReaderResponsiveTimeout));

	const auto removed = erase_if(mLastSeenHistory, [&threshold](const auto pValue){
				return pValue < threshold;
			});

	return removed > 0 && getPercentSeen() != visibilityOld;
}


int IfdListEntry::getPercentSeen(int pCheckInterval, int pTimeFrame) const
{
	const auto count = static_cast<int>(mLastSeenHistory.size());
	const int expectedMax = pTimeFrame / pCheckInterval;
	const int percent = 100 * count / expectedMax;

	// Maximum is calculated based on the assumption, that only IPv4 is in use.
	// If IPv6 is used in parallel - even better.
	return std::min(percent, 100);
}


void IfdListEntry::setDiscovery(const Discovery& pDiscovery)
{
	mDiscovery = pDiscovery;
}


bool IfdListEntry::containsIfdId(const QByteArray& pIfdId) const
{
	return mDiscovery.getIfdId() == pIfdId;
}


const QTime& IfdListEntry::getLastSeen() const
{
	return mLastSeen;
}


const Discovery& IfdListEntry::getDiscovery() const
{
	return mDiscovery;
}
