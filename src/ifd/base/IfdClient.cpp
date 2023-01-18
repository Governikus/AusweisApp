/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "IfdClient.h"


using namespace governikus;


QVector<QSharedPointer<IfdListEntry>> IfdClient::getAnnouncingRemoteDevices() const
{
	return QVector<QSharedPointer<IfdListEntry>>();
}


void IfdClient::requestRemoteDevices()
{
	Q_EMIT fireRemoteDevicesInfo(QVector<QSharedPointer<IfdListEntry>>());
}


QStringList IfdClient::getConnectedDeviceIDs() const
{
	return QStringList();
}


bool IfdClient::hasAnnouncingRemoteDevices() const
{
	return !getAnnouncingRemoteDevices().isEmpty();
}
