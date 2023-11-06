/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDeviceModelEntry.h"

#include "RemoteServiceSettings.h"


using namespace governikus;


RemoteDeviceModelEntry::RemoteDeviceModelEntry(const QSharedPointer<IfdListEntry>& pListEntry)
	: mDeviceName(RemoteServiceSettings::escapeDeviceName(pListEntry->getIfdDescriptor().getIfdName()))
	, mId(pListEntry->getIfdDescriptor().getIfdId())
	, mPaired(false)
	, mIsPairing(pListEntry->getIfdDescriptor().isPairingAnnounced())
	, mNetworkVisible(false)
	, mConnected(false)
	, mSupported(pListEntry->getIfdDescriptor().isSupported())
	, mLastConnected()
	, mRemoteDeviceListEntry(pListEntry)
{
	Q_ASSERT(!mDeviceName.contains(QLatin1Char('<')));
}


RemoteDeviceModelEntry::RemoteDeviceModelEntry(const QString& pDeviceNameEscaped,
		const QString& pId,
		bool pNetworkVisible,
		bool pConnected,
		bool pSupported,
		bool pIsPairing,
		const QDateTime& pLastConnected,
		const QSharedPointer<IfdListEntry>& pRemoteDeviceListEntry)
	: mDeviceName(pDeviceNameEscaped)
	, mId(pId)
	, mPaired(true)
	, mIsPairing(pIsPairing)
	, mNetworkVisible(pNetworkVisible)
	, mConnected(pConnected)
	, mSupported(pSupported)
	, mLastConnected(pLastConnected)
	, mRemoteDeviceListEntry(pRemoteDeviceListEntry)
{
	Q_ASSERT(!mDeviceName.contains(QLatin1Char('<')));
}


RemoteDeviceModelEntry::RemoteDeviceModelEntry(const QString& pDeviceNameEscaped)
	: mDeviceName(pDeviceNameEscaped)
	, mId()
	, mPaired(false)
	, mIsPairing(false)
	, mNetworkVisible(false)
	, mConnected(false)
	, mSupported(false)
	, mLastConnected()
	, mRemoteDeviceListEntry(nullptr)
{
	Q_ASSERT(!mDeviceName.contains(QLatin1Char('<')));
}


const QSharedPointer<IfdListEntry>& RemoteDeviceModelEntry::getRemoteDeviceListEntry() const
{
	return mRemoteDeviceListEntry;
}


QString RemoteDeviceModelEntry::getDeviceNameEscaped() const
{
	return mDeviceName;
}


bool RemoteDeviceModelEntry::isPaired() const
{
	return mPaired;
}


void RemoteDeviceModelEntry::setPaired(bool pPaired)
{
	mPaired = pPaired;
}


bool RemoteDeviceModelEntry::isPairing() const
{
	return mIsPairing;
}


void RemoteDeviceModelEntry::setIsPairing(bool pIsPairing)
{
	mIsPairing = pIsPairing;
}


const QString& RemoteDeviceModelEntry::getId() const
{
	return mId;
}


void RemoteDeviceModelEntry::setId(const QString& pId)
{
	mId = pId;
}


bool RemoteDeviceModelEntry::isNetworkVisible() const
{
	return mNetworkVisible;
}


int RemoteDeviceModelEntry::getLinkQuality() const
{
	if (mConnected)
	{
		return 100;
	}

	if (mRemoteDeviceListEntry.isNull())
	{
		return 0;
	}

	return mRemoteDeviceListEntry->getPercentSeen();
}


bool RemoteDeviceModelEntry::isSupported() const
{
	return mSupported;
}


void RemoteDeviceModelEntry::setNetworkVisible(bool pNetworkVisible)
{
	mNetworkVisible = pNetworkVisible;
}


const QDateTime& RemoteDeviceModelEntry::getLastConnected() const
{
	return mLastConnected;
}


void RemoteDeviceModelEntry::setLastConnected(const QDateTime& pLastConnected)
{
	mLastConnected = pLastConnected;
}


bool RemoteDeviceModelEntry::operator==(const RemoteDeviceModelEntry& pOther) const
{
	return getId() == pOther.getId();
}
