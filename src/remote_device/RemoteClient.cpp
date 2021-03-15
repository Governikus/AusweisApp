/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteClient.h"


using namespace governikus;


RemoteClient::~RemoteClient()
{
}


QVector<QSharedPointer<RemoteDeviceListEntry> > RemoteClient::getAnnouncingRemoteDevices() const
{
	return QVector<QSharedPointer<RemoteDeviceListEntry> >();
}


void RemoteClient::requestRemoteDevices()
{
	Q_EMIT fireRemoteDevicesInfo(QVector<QSharedPointer<RemoteDeviceListEntry> >());
}


QStringList RemoteClient::getConnectedDeviceIDs() const
{
	return QStringList();
}


bool RemoteClient::hasAnnouncingRemoteDevices() const
{
	return !getAnnouncingRemoteDevices().isEmpty();
}
