/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "GlobalStatus.h"
#include "IfdDispatcherClient.h"
#include "IfdList.h"
#include "RemoteServiceSettings.h"

#include <QObject>
#include <QThread>

namespace governikus
{

class IfdClient
	: public QObject
	, private Env::ThreadSafe
{
	Q_OBJECT

	Q_SIGNALS:
		void fireDeviceAppeared(const QSharedPointer<IfdListEntry>& pEntry);
		void fireDeviceUpdated(const QSharedPointer<IfdListEntry>& pEntry);
		void fireDeviceVanished(const QSharedPointer<IfdListEntry>& pEntry);
		void fireEstablishConnectionDone(const QSharedPointer<IfdListEntry>& pEntry, const GlobalStatus& pStatus);

		void fireNewDispatcher(const QSharedPointer<IfdDispatcherClient>& pDispatcher);
		void fireDispatcherChanged(const QSharedPointer<IfdDispatcherClient>& pDispatcher);
		void fireRemoteDevicesInfo(const QList<QSharedPointer<IfdListEntry>>& pRemoteDevices);
		void fireDispatcherDestroyed(GlobalStatus::Code pCloseCode, const QByteArray& pId);
		void fireDetectionChanged();
		void fireCertificateRemoved(const QString& pDeviceName);

	public:
		IfdClient() = default;
		~IfdClient() override = default;

		Q_INVOKABLE virtual void startDetection() = 0;
		Q_INVOKABLE virtual void stopDetection() = 0;
		Q_INVOKABLE virtual bool isDetecting() = 0;

		Q_INVOKABLE virtual void establishConnection(const QSharedPointer<IfdListEntry>& pEntry, const QByteArray& pPsk) = 0;

		[[nodiscard]] virtual QList<QSharedPointer<IfdListEntry>> getAnnouncingRemoteDevices() const;
		[[nodiscard]] bool hasAnnouncingRemoteDevices() const;
		Q_INVOKABLE virtual void requestRemoteDevices();
		[[nodiscard]] virtual QByteArrayList getConnectedDeviceIDs() const;
		virtual QList<RemoteServiceSettings::RemoteInfo> getConnectedDeviceInfos() = 0;
};


} // namespace governikus
