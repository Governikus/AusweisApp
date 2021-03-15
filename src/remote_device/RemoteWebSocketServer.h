/*!
 * \brief WebSocketServer on the server side of a smartphone as card reader (SaC) scenario.
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ServerMessageHandler.h"

#include "RemoteTlsServer.h"

#include <QByteArray>
#include <QMetaObject>
#include <QSharedPointer>
#include <QString>
#include <QWebSocket>
#include <QWebSocketServer>


namespace governikus
{

class RemoteWebSocketServer
	: public QObject
{
	Q_OBJECT

	protected:
		RemoteWebSocketServer() = default;

	public:
		virtual ~RemoteWebSocketServer();

		[[nodiscard]] virtual bool isListening() const = 0;
		[[nodiscard]] virtual bool isConnected() const = 0;
		[[nodiscard]] virtual bool isPairingConnection() const = 0;
		virtual bool listen(const QString& pServerName) = 0;
		virtual void close() = 0;
		[[nodiscard]] virtual QString getServerName() const = 0;
		[[nodiscard]] virtual quint16 getServerPort() const = 0;
		virtual void setPairing(bool pEnable = true) = 0;
		[[nodiscard]] virtual QSslCertificate getCurrentCertificate() const = 0;
		[[nodiscard]] virtual const QSharedPointer<ServerMessageHandler>& getMessageHandler() const = 0;

	Q_SIGNALS:
		void fireConnectedChanged(bool pConnected);
		void fireMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler);
		void firePskChanged(const QByteArray& pPsk);
		void firePairingCompleted();
};


class RemoteWebSocketServerImpl
	: public RemoteWebSocketServer
{
	Q_OBJECT

	QScopedPointer<RemoteTlsServer, QScopedPointerDeleteLater> mTlsServer;
	QWebSocketServer mServer;
	QSharedPointer<ServerMessageHandler> mServerMessageHandler;
	bool mPairingConnection;

	private Q_SLOTS:
		void onWebsocketConnection();
		void onConnectionClosed();
		void onServerError(QWebSocketProtocol::CloseCode pCloseCode) const;

	public:
		RemoteWebSocketServerImpl();
		virtual ~RemoteWebSocketServerImpl() override;

		[[nodiscard]] virtual bool isListening() const override;
		[[nodiscard]] virtual bool isConnected() const override;
		[[nodiscard]] virtual bool isPairingConnection() const override;
		virtual bool listen(const QString& pServerName) override;
		virtual void close() override;
		[[nodiscard]] virtual QString getServerName() const override;
		[[nodiscard]] virtual quint16 getServerPort() const override;
		virtual void setPairing(bool pEnable = true) override;
		[[nodiscard]] virtual QSslCertificate getCurrentCertificate() const override;
		[[nodiscard]] virtual const QSharedPointer<ServerMessageHandler>& getMessageHandler() const override;
};

} // namespace governikus
