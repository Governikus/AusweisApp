/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteClient.h"


using namespace governikus;


RemoteClient::~RemoteClient()
{
}


QVector<QSharedPointer<RemoteDeviceListEntry> > RemoteClient::getRemoteDevices() const
{
	return QVector<QSharedPointer<RemoteDeviceListEntry> >();
}


void RemoteClient::requestRemoteDevices()
{
	Q_EMIT fireRemoteDevicesInfo(QVector<QSharedPointer<RemoteDeviceListEntry> >());
}
