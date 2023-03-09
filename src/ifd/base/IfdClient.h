/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief An interface for IfdClients, meant to omit the
 * dependency between card_base and ifd.
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
		void fireRemoteDevicesInfo(const QVector<QSharedPointer<IfdListEntry>>& pRemoteDevices);
		void fireDispatcherDestroyed(GlobalStatus::Code pCloseCode, const QString& pId);
		void fireDetectionChanged();
		void fireCertificateRemoved(const QString& pDeviceName);

	public:
		IfdClient() = default;
		~IfdClient() override = default;

		Q_INVOKABLE virtual void startDetection() = 0;
		Q_INVOKABLE virtual void stopDetection() = 0;
		Q_INVOKABLE virtual bool isDetecting() = 0;

		Q_INVOKABLE virtual void establishConnection(const QSharedPointer<IfdListEntry>& pEntry, const QString& pPsk) = 0;

		[[nodiscard]] virtual QVector<QSharedPointer<IfdListEntry>> getAnnouncingRemoteDevices() const;
		[[nodiscard]] bool hasAnnouncingRemoteDevices() const;
		Q_INVOKABLE virtual void requestRemoteDevices();
		[[nodiscard]] virtual QStringList getConnectedDeviceIDs() const;
		virtual QVector<RemoteServiceSettings::RemoteInfo> getConnectedDeviceInfos() = 0;
};


} // namespace governikus
