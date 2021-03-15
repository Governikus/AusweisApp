/*!
 * \brief Send RemoteReaderDiscoverCmds and maintain a list of responsive peers.
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteClient.h"

#include "DatagramHandler.h"
#include "Env.h"
#include "RemoteConnector.h"
#include "RemoteDeviceList.h"

#include <QObject>
#include <QPointer>
#include <QThread>
#include <QTimer>

class test_RemoteClient;

namespace governikus
{

class RemoteClientImpl
	: public RemoteClient
{
	Q_OBJECT

	private:
		friend ::test_RemoteClient;
		friend RemoteClient* createNewObject<RemoteClient*>();

		QSharedPointer<DatagramHandler> mDatagramHandler;
		QScopedPointer<RemoteDeviceList> mRemoteDeviceList;
		QMap<QString, int> mErrorCounter;

		QThread mRemoteConnectorThread;
		QPointer<RemoteConnector> mRemoteConnector;
		QVector<QSharedPointer<RemoteDeviceListEntry>> mRemoteConnectorPending;
		QStringList mConnectedDeviceIds;

		RemoteClientImpl();
		~RemoteClientImpl() override;

		void bootstrapRemoteConnectorThread();
		void shutdownRemoteConnectorThread();
		QSharedPointer<RemoteDeviceListEntry> mapToAndTakeRemoteConnectorPending(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor);

	private Q_SLOTS:
		void onNewMessage(const QByteArray& pData, const QHostAddress& pAddress);
		void onRemoteDispatcherCreated(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const QSharedPointer<RemoteDispatcherClient>& pDispatcher);
		void onRemoteDispatcherError(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, RemoteErrorCode pErrorCode);
		void onDispatcherDestroyed(GlobalStatus::Code pCloseCode, const QString& pId);

	public:
		Q_INVOKABLE void startDetection() override;
		Q_INVOKABLE void stopDetection() override;
		Q_INVOKABLE bool isDetecting() override;

		Q_INVOKABLE void establishConnection(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const QString& pPsk) override;

		[[nodiscard]] QVector<QSharedPointer<RemoteDeviceListEntry>> getAnnouncingRemoteDevices() const override;
		Q_INVOKABLE void requestRemoteDevices()  override;
		[[nodiscard]] QStringList getConnectedDeviceIDs() const override;
		QVector<RemoteServiceSettings::RemoteInfo> getConnectedDeviceInfos() override;
};


} // namespace governikus
