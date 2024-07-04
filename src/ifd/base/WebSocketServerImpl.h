/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief WebSocketServer on the server side of a smartphone as card reader (SaC) scenario.
 */

#pragma once

#include "ReaderManagerPluginInfo.h"
#include "TlsServer.h"
#include "WebSocketServer.h"

#include <QList>
#include <QWebSocketServer>

namespace governikus
{

class WebSocketServerImpl
	: public WebSocketServer
{
	Q_OBJECT

	private:
		QWebSocketServer mServer;
		QSharedPointer<TlsServer> mTlsServer;
		QSharedPointer<ServerMessageHandler> mServerMessageHandler;
		QList<ReaderManagerPluginType> mAllowedPluginTypes;

	private Q_SLOTS:
		void onWebsocketConnection();
		void onConnectionClosed();
		void onServerError(QWebSocketProtocol::CloseCode pCloseCode) const;

	public:
		explicit WebSocketServerImpl(QSharedPointer<TlsServer> pTlsServer, const QList<ReaderManagerPluginType>& pAllowedPluginTypes);
		~WebSocketServerImpl() override;

		[[nodiscard]] bool isListening() const override;
		[[nodiscard]] bool isConnected() const override;
		bool listen(const QString& pServerName, quint16 pPort) override;
		void close() override;
		[[nodiscard]] QString getServerName() const override;
		[[nodiscard]] QHostAddress getServerAddress() const override;
		[[nodiscard]] quint16 getServerPort() const override;
		[[nodiscard]] const QSharedPointer<ServerMessageHandler>& getMessageHandler() const override;
};

} // namespace governikus
