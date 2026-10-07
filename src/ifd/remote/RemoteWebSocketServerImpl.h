/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "RemoteTlsServer.h"
#include "RemoteWebSocketServer.h"
#include "WebSocketServerImpl.h"

#include <QSharedPointer>
#include <QWebSocket>


class test_IfdConnector;


namespace governikus
{

class RemoteWebSocketServerImpl
	: public RemoteWebSocketServer
{
	Q_OBJECT
	friend class ::test_IfdConnector;

	private:
		QSharedPointer<RemoteTlsServer> mRemoteTlsServer;
		WebSocketServerImpl mWebSocketServer;
		bool mPairingConnection;

	private Q_SLOTS:
		void onNewConnection(QSharedPointer<QWebSocket> pSocket);

	public:
		RemoteWebSocketServerImpl();

		bool isListening() const override;
		bool isConnected() const override;
		bool listen(const QString& pServerName, quint16 pPort) override;
		void close() override;
		QString getServerName() const override;
		QHostAddress getServerAddress() const override;
		quint16 getServerPort() const override;
		const QSharedPointer<ServerMessageHandler>& getMessageHandler() const override;
		void rotatePsk() override;

		[[nodiscard]] bool isPairingConnection() const override;
		[[nodiscard]] bool isPairingAnnounced() const override;
		void setPairing(bool pEnable = true) override;
		[[nodiscard]] QSslCertificate getCurrentCertificate() const override;
};

} // namespace governikus
