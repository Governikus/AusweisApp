/*!
 * \brief Send RemoteReaderDiscoverCmds and maintain a list of responsive peers.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteClient.h"

#include "DatagramHandler.h"
#include "RemoteConnector.h"
#include "RemoteDeviceList.h"

#include <QObject>
#include <QPointer>
#include <QThread>
#include <QTimer>

namespace governikus
{

class RemoteClientImpl
	: public RemoteClient
{
	Q_OBJECT

	private:
		QSharedPointer<DatagramHandler> mDatagramHandler;
		QScopedPointer<RemoteDeviceList> mRemoteDeviceList;
		QMap<QString, int> mErrorCounter;

		QThread mRemoteConnectorThread;
		QPointer<RemoteConnector> mRemoteConnector;
		QVector<QSharedPointer<RemoteDeviceListEntry> > mRemoteConnectorPending;
		QStringList mConnectedDeviceIds;

		void bootstrapRemoteConnectorThread();
		void shutdownRemoteConnectorThread();
		QSharedPointer<RemoteDeviceListEntry> mapToAndTakeRemoteConnectorPending(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor);

	private Q_SLOTS:
		void onNewMessage(const QJsonDocument& pData, const QHostAddress& pAddress);
		void onRemoteDispatcherCreated(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const QSharedPointer<RemoteDispatcher>& pAdapter);
		void onRemoteDispatcherError(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, RemoteErrorCode pErrorCode);
		void onDispatcherDestroyed(GlobalStatus::Code pCloseCode, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher);

	public:
		RemoteClientImpl();
		virtual ~RemoteClientImpl() override;

		Q_INVOKABLE virtual void startDetection() override;
		Q_INVOKABLE virtual void stopDetection() override;
		Q_INVOKABLE virtual bool isDetecting() override;

		Q_INVOKABLE virtual void establishConnection(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const QString& pPsk) override;

		virtual QVector<QSharedPointer<RemoteDeviceListEntry> > getRemoteDevices() const override;
		Q_INVOKABLE virtual void requestRemoteDevices()  override;
		virtual QVector<RemoteServiceSettings::RemoteInfo> getConnectedDeviceInfos() override;
};


} /* namespace governikus */
