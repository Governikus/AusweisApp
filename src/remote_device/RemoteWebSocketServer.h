/*!
 * \brief WebSocketServer on the server side of a remote reader scenario.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
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

		virtual bool isListening() const = 0;
		virtual bool isConnected() const = 0;
		virtual bool listen(const QString& pServerName) = 0;
		virtual void close() = 0;
		virtual QString getServerName() const = 0;
		virtual quint16 getServerPort() const = 0;
		virtual void setPairing(bool pEnable = true) = 0;
		virtual QSslCertificate getCurrentCertificate() const = 0;
		virtual const QSharedPointer<ServerMessageHandler>& getMessageHandler() const = 0;

	Q_SIGNALS:
		void fireConnectedChanged(bool pConnected);
		void fireMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler);
		void firePskChanged(const QByteArray& pPsk);
};


class RemoteWebSocketServerImpl
	: public RemoteWebSocketServer
{
	Q_OBJECT

	QScopedPointer<RemoteTlsServer, QScopedPointerDeleteLater> mTlsServer;
	QWebSocketServer mServer;
	QSharedPointer<ServerMessageHandler> mServerMessageHandler;

	private Q_SLOTS:
		void onWebsocketConnection();
		void onConnectionClosed();
		void onServerError(QWebSocketProtocol::CloseCode pCloseCode);

	public:
		RemoteWebSocketServerImpl();
		virtual ~RemoteWebSocketServerImpl() override;

		virtual bool isListening() const override;
		virtual bool isConnected() const override;
		virtual bool listen(const QString& pServerName) override;
		virtual void close() override;
		virtual QString getServerName() const override;
		virtual quint16 getServerPort() const override;
		virtual void setPairing(bool pEnable = true) override;
		virtual QSslCertificate getCurrentCertificate() const override;
		virtual const QSharedPointer<ServerMessageHandler>& getMessageHandler() const override;
};

} // namespace governikus
