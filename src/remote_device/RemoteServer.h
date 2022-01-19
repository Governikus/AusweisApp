/*!
 * \brief Remote server service to offer smartphones as card readers (SaCs).
 * This class controls the advertising over UDP as well as the Websocket connection management.
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteWebSocketServer.h"
#include "ServerMessageHandler.h"

#include <QByteArray>
#include <QObject>
#include <QScopedPointer>
#include <QSslCertificate>

namespace governikus
{

class RemoteServer
	: public QObject
{
	Q_OBJECT

	protected:
		RemoteServer() = default;

	public:
		~RemoteServer() override;

		[[nodiscard]] virtual bool isRunning() const = 0;
		virtual bool start(const QString& pServerName) = 0;
		virtual void stop() = 0;
		virtual void setPairing(bool pEnable = true) = 0;
		[[nodiscard]] virtual bool isConnected() const = 0;
		[[nodiscard]] virtual bool isPairingConnection() const = 0;
		[[nodiscard]] virtual QSslCertificate getCurrentCertificate() const = 0;
		[[nodiscard]] virtual const QSharedPointer<ServerMessageHandler>& getMessageHandler() const = 0;

	Q_SIGNALS:
		void fireMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler);
		void firePskChanged(const QByteArray& pPsk);
		void fireConnectedChanged(bool pConnected);
		void fireIsRunningChanged();
		void firePairingCompleted();
};

} // namespace governikus
