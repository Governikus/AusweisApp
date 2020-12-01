/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteConnector.h"

#include <QTimer>
#include <QWebSocket>

namespace governikus
{

class ConnectRequest
	: public QObject
{
	Q_OBJECT

	private:
		const RemoteDeviceDescriptor mRemoteDeviceDescriptor;
		const QByteArray mPsk;
		const QSharedPointer<QWebSocket> mSocket;
		QTimer mTimer;

	private Q_SLOTS:
		void onConnected();
		void onError(QAbstractSocket::SocketError pError);
		void onTimeout();
		void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator);
		void onSslErrors(const QList<QSslError>& pErrors);

	public:
		ConnectRequest(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor,
				const QByteArray& pPsk,
				int pTimeoutMs);
		virtual ~ConnectRequest() = default;

		const RemoteDeviceDescriptor& getRemoteDeviceDescriptor() const;

		void start();

	Q_SIGNALS:
		void fireConnectionCreated(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor,
				const QSharedPointer<QWebSocket>& pWebSocket);
		void fireConnectionError(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const RemoteErrorCode& pError);
		void fireConnectionTimeout(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor);
};

} // namespace governikus
