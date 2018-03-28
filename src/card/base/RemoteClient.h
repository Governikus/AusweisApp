/*!
 * \brief An interface for RemoteClientImpl, meant to omit the
 * dependency between card_base and remote_device.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"
#include "RemoteServiceSettings.h"

#include <QObject>
#include <QThread>

namespace governikus
{

class RemoteDispatcher;
class RemoteDeviceListEntry;

class RemoteClient
	: public QObject
{
	Q_OBJECT

	Q_SIGNALS:
		void fireDeviceAppeared(const QSharedPointer<RemoteDeviceListEntry>& pEntry);
		void fireDeviceVanished(const QSharedPointer<RemoteDeviceListEntry>& pEntry);
		void fireEstablishConnectionDone(const QSharedPointer<RemoteDeviceListEntry>& pEntry, GlobalStatus pStatus);

		void fireNewRemoteDispatcher(const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher);
		void fireRemoteDevicesInfo(const QVector<QSharedPointer<RemoteDeviceListEntry> >& pRemoteDevices);
		void fireDispatcherDestroyed(GlobalStatus::Code pCloseCode, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher);
		void fireDetectionChanged();
		void fireCertificateRemoved(QString pDeviceName);

	public:
		RemoteClient() = default;
		virtual ~RemoteClient();

		Q_INVOKABLE virtual void startDetection() = 0;
		Q_INVOKABLE virtual void stopDetection() = 0;
		Q_INVOKABLE virtual bool isDetecting() = 0;

		Q_INVOKABLE virtual void establishConnection(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const QString& pPsk) = 0;

		virtual QVector<QSharedPointer<RemoteDeviceListEntry> > getRemoteDevices() const;
		Q_INVOKABLE virtual void requestRemoteDevices();
		virtual QVector<RemoteServiceSettings::RemoteInfo> getConnectedDeviceInfos() = 0;
};


} /* namespace governikus */
