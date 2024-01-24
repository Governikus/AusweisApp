/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief WebSocketServer on the server side of a smartphone as card reader (SaC) scenario.
 */

#pragma once

#include "ServerMessageHandler.h"

#include <QByteArray>
#include <QSharedPointer>
#include <QString>
#include <QWebSocket>


namespace governikus
{

class WebSocketServer
	: public QObject
{
	Q_OBJECT

	protected:
		WebSocketServer() = default;

	public:
		~WebSocketServer() override;

		[[nodiscard]] virtual bool isListening() const = 0;
		[[nodiscard]] virtual bool isConnected() const = 0;
		virtual bool listen(const QString& pServerName, quint16 pPort = 0) = 0;
		virtual void close() = 0;
		[[nodiscard]] virtual QString getServerName() const = 0;
		[[nodiscard]] virtual QHostAddress getServerAddress() const = 0;
		[[nodiscard]] virtual quint16 getServerPort() const = 0;
		[[nodiscard]] virtual const QSharedPointer<ServerMessageHandler>& getMessageHandler() const = 0;

	Q_SIGNALS:
		void fireNewConnection(QSharedPointer<QWebSocket> pSocket);
		void fireConnectedChanged(bool pConnected);
		void fireMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler);
		void firePskChanged(const QByteArray& pPsk);
		void fireSocketError(QAbstractSocket::SocketError pSocketError);
};

} // namespace governikus
