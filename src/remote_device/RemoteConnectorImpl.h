/*!
 * \brief Module that creates and manages connections from a remote client to a remote server.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteConnector.h"

#include <QTimer>
#include <QWebSocket>

namespace governikus
{
class ConnectRequest;

class RemoteConnectorImpl
	: public RemoteConnector
{
	Q_OBJECT

	private:
		const int mConnectTimeoutMs;
		QVector<QSharedPointer<ConnectRequest> > mPendingRequests;

		void removeRequest(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor);

	private Q_SLOTS:
		void onConnectionCreated(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const QSharedPointer<QWebSocket>& pWebSocket);
		void onConnectionError(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const RemoteErrorCode& pError);
		void onConnectionTimeout(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor);

	public:
		RemoteConnectorImpl(int pConnectTimeoutMs = 5000);
		virtual ~RemoteConnectorImpl() override = default;

		virtual void onConnectRequest(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const QString& pPsk) override;

};

} /* namespace governikus */
