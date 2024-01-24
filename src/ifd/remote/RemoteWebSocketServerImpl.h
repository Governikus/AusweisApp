/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief WebSocketServer on the server side of a smartphone as card reader (SaC) scenario.
 */

#pragma once

#include "RemoteTlsServer.h"
#include "RemoteWebSocketServer.h"
#include "WebSocketServerImpl.h"

#include <QSharedPointer>
#include <QWebSocket>


namespace governikus
{

class RemoteWebSocketServerImpl
	: public RemoteWebSocketServer
{
	Q_OBJECT

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

		[[nodiscard]] bool isPairingConnection() const override;
		[[nodiscard]] bool isPairingAnnounced() const override;
		void setPairing(bool pEnable = true) override;
		[[nodiscard]] QSslCertificate getCurrentCertificate() const override;
};

} // namespace governikus
