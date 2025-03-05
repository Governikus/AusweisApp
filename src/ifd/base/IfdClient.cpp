/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdClient.h"


using namespace governikus;


QList<QSharedPointer<IfdListEntry>> IfdClient::getAnnouncingRemoteDevices() const
{
	return QList<QSharedPointer<IfdListEntry>>();
}


void IfdClient::requestRemoteDevices()
{
	Q_EMIT fireRemoteDevicesInfo(QList<QSharedPointer<IfdListEntry>>());
}


QByteArrayList IfdClient::getConnectedDeviceIDs() const
{
	return QByteArrayList();
}


bool IfdClient::hasAnnouncingRemoteDevices() const
{
	return !getAnnouncingRemoteDevices().isEmpty();
}
