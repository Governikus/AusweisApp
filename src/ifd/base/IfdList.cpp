/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "IfdList.h"

#include "Env.h"
#include "Initializer.h"

#include <QtGlobal>

using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<IfdListEntry>>("QSharedPointer<IfdListEntry>");
			qRegisterMetaType<QVector<QSharedPointer<IfdListEntry>>>("QVector<QSharedPointer<IfdListEntry>>");
		})


namespace governikus
{

template<> IfdList* createNewObject<IfdList*>()
{
	return new IfdListImpl();
}


template<> IfdList* createNewObject<IfdList*, int, int>(int&& pCheckInterval, int&& pTimeout)
{
	return new IfdListImpl(pCheckInterval, pTimeout);
}


} // namespace governikus


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
	const int visibilityOld = getPercentSeen();

	const QTime threshold(QTime::currentTime().addMSecs(-pReaderResponsiveTimeout));
	QMutableVectorIterator<QTime> i(mLastSeenHistory);
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
	const int count = mLastSeenHistory.size();
	const int expectedMax = pTimeFrame / pCheckInterval;
	const int percent = 100 * count / expectedMax;

	// Maximum is calculated based on the assumption, that only IPv4 is in use.
	// If IPv6 is used in parallel - even better.
	return qMin(percent, 100);
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


QVector<QSharedPointer<IfdListEntry>> IfdList::getIfdList() const
{
	return QVector<QSharedPointer<IfdListEntry>>();
}


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


QVector<QSharedPointer<IfdListEntry>> IfdListImpl::getIfdList() const
{
	return mResponsiveList;
}


void IfdListImpl::onProcessUnresponsiveRemoteReaders()
{
	const QTime threshold(QTime::currentTime().addMSecs(-mReaderResponsiveTimeout));
	QMutableVectorIterator<QSharedPointer<IfdListEntry>> i(mResponsiveList);
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
