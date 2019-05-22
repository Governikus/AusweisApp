/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
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


} // namespace governikus


RemoteDeviceListEntry::RemoteDeviceListEntry(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor)
	: mRemoteDeviceDescriptor(pRemoteDeviceDescriptor)
	, mLastSeen(QTime::currentTime())
{
}


void RemoteDeviceListEntry::setLastSeenToNow()
{
	mLastSeen = QTime::currentTime();
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
			entry->setLastSeenToNow();
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
		}
	}

	if (mResponsiveList.isEmpty())
	{
		mTimer.stop();
	}
}
