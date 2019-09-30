/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDeviceList.h"

#include "Env.h"
#include "Initializer.h"

#include <QtGlobal>

using namespace governikus;


static Initializer::Entry E([] {
			qRegisterMetaType<QSharedPointer<RemoteDeviceListEntry> >("QSharedPointer<RemoteDeviceListEntry>");
			qRegisterMetaType<QVector<QSharedPointer<RemoteDeviceListEntry> > >("QVector<QSharedPointer<RemoteDeviceListEntry>>");
		});


namespace governikus
{

template<> RemoteDeviceList* createNewObject<RemoteDeviceList*>()
{
	return new RemoteDeviceListImpl;
}


template<> RemoteDeviceList* createNewObject<RemoteDeviceList*, int, int>(int&& pCheckInterval, int&& pTimeout)
{
	return new RemoteDeviceListImpl(pCheckInterval, pTimeout);
}


} // namespace governikus


RemoteDeviceListEntry::RemoteDeviceListEntry(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor)
	: mRemoteDeviceDescriptor(pRemoteDeviceDescriptor)
	, mLastSeen(QTime::currentTime())
	, mLastSeenHistory()
{
}


void RemoteDeviceListEntry::setLastSeenToNow()
{
	if (mLastSeen.isValid())
	{
		mLastSeenHistory += mLastSeen;
	}

	mLastSeen = QTime::currentTime();
}


bool RemoteDeviceListEntry::cleanUpSeenTimestamps(int pReaderResponsiveTimeout)
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


int RemoteDeviceListEntry::getPercentSeen(int pCheckInterval, int pTimeFrame) const
{
	const int count = mLastSeenHistory.size();
	const int expectedMax = pTimeFrame / pCheckInterval;
	const int percent = 100 * count / expectedMax;

	// Maximum is calculated based on the assumption, that only IPv4 is in use.
	// If IPv6 is used in parallel - even better.
	return qMin(percent, 100);
}


bool RemoteDeviceListEntry::containsEquivalent(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor) const
{
	return mRemoteDeviceDescriptor.isEquivalent(pRemoteDeviceDescriptor);
}


bool RemoteDeviceListEntry::isEqual(const RemoteDeviceListEntry* const pOther) const
{
	return pOther != nullptr &&
		   mRemoteDeviceDescriptor == pOther->mRemoteDeviceDescriptor &&
		   mLastSeen == pOther->mLastSeen;
}


const QTime& RemoteDeviceListEntry::getLastSeen() const
{
	return mLastSeen;
}


const RemoteDeviceDescriptor& RemoteDeviceListEntry::getRemoteDeviceDescriptor() const
{
	return mRemoteDeviceDescriptor;
}


RemoteDeviceList::RemoteDeviceList(int, int)
{
}


RemoteDeviceList::~RemoteDeviceList()
{
}


QVector<QSharedPointer<RemoteDeviceListEntry> > RemoteDeviceList::getRemoteDevices() const
{
	return QVector<QSharedPointer<RemoteDeviceListEntry> >();
}


RemoteDeviceListImpl::RemoteDeviceListImpl(int pCheckInterval, int pReaderResponsiveTimeout)
	: RemoteDeviceList(pCheckInterval, pReaderResponsiveTimeout)
	, mTimer()
	, mReaderResponsiveTimeout(pReaderResponsiveTimeout)
	, mResponsiveList()
{
	connect(&mTimer, &QTimer::timeout, this, &RemoteDeviceListImpl::onProcessUnresponsiveRemoteReaders);
	pCheckInterval = pCheckInterval / 2 - 1;  // Nyquist-Shannon sampling theorem. Enable smooth UI updates.
	mTimer.setInterval(pCheckInterval);
}


RemoteDeviceListImpl::~RemoteDeviceListImpl()
{
	mTimer.stop();
}


void RemoteDeviceListImpl::update(const RemoteDeviceDescriptor& pDescriptor)
{
	for (const QSharedPointer<RemoteDeviceListEntry>& entry : qAsConst(mResponsiveList))
	{
		if (entry->containsEquivalent(pDescriptor))
		{
			const int visibilityOld = entry->getPercentSeen();
			entry->setLastSeenToNow();
			if (entry->getPercentSeen() != visibilityOld)
			{
				Q_EMIT fireDeviceUpdated(entry);
			}
			return;
		}
	}

	const auto& newDevice = QSharedPointer<RemoteDeviceListEntry>::create(pDescriptor);
	mResponsiveList += newDevice;

	if (!mTimer.isActive())
	{
		mTimer.start();
	}

	Q_EMIT fireDeviceAppeared(newDevice);
}


void RemoteDeviceListImpl::clear()
{
	mResponsiveList.clear();
}


QVector<QSharedPointer<RemoteDeviceListEntry> > RemoteDeviceListImpl::getRemoteDevices() const
{
	return mResponsiveList;
}


void RemoteDeviceListImpl::onProcessUnresponsiveRemoteReaders()
{
	const QTime threshold(QTime::currentTime().addMSecs(-mReaderResponsiveTimeout));
	QMutableVectorIterator<QSharedPointer<RemoteDeviceListEntry> > i(mResponsiveList);
	while (i.hasNext())
	{
		const QSharedPointer<RemoteDeviceListEntry>& entry = i.next();
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
