/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDeviceList.h"

#include "Env.h"
#include "Initializer.h"

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


} /* namespace governikus */


RemoteDeviceListEntry::RemoteDeviceListEntry(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor)
	: mRemoteDeviceDescriptor(pRemoteDeviceDescriptor)
	, mLastSeen(QTime::currentTime())
{
}


void RemoteDeviceListEntry::setLastSeenToNow()
{
	mLastSeen = QTime::currentTime();
}


bool RemoteDeviceListEntry::contains(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor) const
{
	return mRemoteDeviceDescriptor == pRemoteDeviceDescriptor;
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


RemoteDeviceListImpl::RemoteDeviceListImpl(int pCheckInterval, int pTimeout)
	: RemoteDeviceList(pCheckInterval, pTimeout)
	, mTimer()
	, mTimeout(pTimeout)
	, mList()
{
	connect(&mTimer, &QTimer::timeout, this, &RemoteDeviceListImpl::onRemoveUnresponsiveRemoteReaders);
	mTimer.setInterval(pCheckInterval);
}


RemoteDeviceListImpl::~RemoteDeviceListImpl()
{
	mTimer.stop();
}


void RemoteDeviceListImpl::update(const RemoteDeviceDescriptor& pDescriptor)
{
	for (const QSharedPointer<RemoteDeviceListEntry>& entry : qAsConst(mList))
	{
		if (entry->contains(pDescriptor))
		{
			entry->setLastSeenToNow();
			return;
		}
	}

	const QSharedPointer<RemoteDeviceListEntry> newDevice(new RemoteDeviceListEntry(pDescriptor));
	mList.append(newDevice);

	if (!mTimer.isActive())
	{
		mTimer.start();
	}

	Q_EMIT fireDeviceAppeared(newDevice);
}


void RemoteDeviceListImpl::clear()
{
	mList.clear();
}


QVector<QSharedPointer<RemoteDeviceListEntry> > RemoteDeviceListImpl::getRemoteDevices() const
{
	return mList;
}


void RemoteDeviceListImpl::onRemoveUnresponsiveRemoteReaders()
{
	const QTime threshold(QTime::currentTime().addMSecs(-mTimeout));
	QMutableVectorIterator<QSharedPointer<RemoteDeviceListEntry> > i(mList);
	while (i.hasNext())
	{
		const QSharedPointer<RemoteDeviceListEntry>& pEntry = i.next();
		if (pEntry->getLastSeen() < threshold)
		{
			i.remove();
			Q_EMIT fireDeviceVanished(pEntry);
		}
	}

	if (mList.isEmpty())
	{
		mTimer.stop();
	}
}
