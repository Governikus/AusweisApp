/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdListEntry.h"
#include "Initializer.h"

#include <QSharedPointer>


using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<IfdListEntry>>("QSharedPointer<IfdListEntry>");
			qRegisterMetaType<QList<QSharedPointer<IfdListEntry>>>("QList<QSharedPointer<IfdListEntry>>");
		})


IfdListEntry::IfdListEntry(const IfdDescriptor& pIfdDescriptor)
	: mIfdDescriptor(pIfdDescriptor)
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
	bool entryRemoved = false;
	const auto visibilityOld = getPercentSeen();

	const QTime threshold(QTime::currentTime().addMSecs(-pReaderResponsiveTimeout));
	QMutableListIterator i(mLastSeenHistory);
	while (i.hasNext())
	{
		if (i.next() < threshold)
		{
			i.remove();
			entryRemoved = true;
		}
	}

	return entryRemoved && getPercentSeen() != visibilityOld;
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


void IfdListEntry::setIfdDescriptor(const IfdDescriptor& pIfdDescriptor)
{
	mIfdDescriptor = pIfdDescriptor;
}


bool IfdListEntry::containsEquivalent(const IfdDescriptor& pIfdDescriptor) const
{
	return mIfdDescriptor.isSameIfd(pIfdDescriptor);
}


bool IfdListEntry::isEqual(const IfdListEntry* const pOther) const
{
	return pOther != nullptr &&
		   mIfdDescriptor == pOther->mIfdDescriptor &&
		   mLastSeen == pOther->mLastSeen;
}


const QTime& IfdListEntry::getLastSeen() const
{
	return mLastSeen;
}


const IfdDescriptor& IfdListEntry::getIfdDescriptor() const
{
	return mIfdDescriptor;
}
