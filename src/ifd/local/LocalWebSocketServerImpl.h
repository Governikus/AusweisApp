/*!
 * \brief Implementation of LocalWebSocketServer interface
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "LocalTlsServer.h"
#include "LocalWebSocketServer.h"
#include "WebSocketServerImpl.h"

#include <QSharedPointer>

namespace governikus
{

class LocalWebSocketServerImpl
	: public LocalWebSocketServer
{
	Q_OBJECT

	private:
		QSharedPointer<LocalTlsServer> mLocalTlsServer;
		WebSocketServerImpl mWebSocketServer;

	public:
		LocalWebSocketServerImpl();
		void setPsk(const QByteArray& pPsk) override;

		bool isListening() const override;
		bool isConnected() const override;
		bool listen(const QString& pServerName, quint16 pPort) override;
		void close() override;
		QString getServerName() const override;
		QHostAddress getServerAddress() const override;
		quint16 getServerPort() const override;
		const QSharedPointer<ServerMessageHandler>& getMessageHandler() const override;
};

} // namespace governikus
