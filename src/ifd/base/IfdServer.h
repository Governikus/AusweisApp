/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ServerMessageHandler.h"

#include <QAbstractSocket>
#include <QByteArray>
#include <QObject>
#include <QScopedPointer>
#include <QSslCertificate>

namespace governikus
{

class IfdServer
	: public QObject
{
	Q_OBJECT

	protected:
		IfdServer() = default;

	public:
		~IfdServer() override;

		[[nodiscard]] virtual bool isRunning() const = 0;
		virtual bool start(const QString& pServerName) = 0;
		virtual void stop() = 0;
		virtual void setPairing(bool pEnable = true) = 0;
		[[nodiscard]] virtual bool isConnected() const = 0;
		[[nodiscard]] virtual bool isPairingConnection() const = 0;
		[[nodiscard]] virtual QSslCertificate getCurrentCertificate() const = 0;
		[[nodiscard]] virtual const QSharedPointer<ServerMessageHandler>& getMessageHandler() const = 0;
		[[nodiscard]] virtual bool isLocal() const = 0;

	Q_SIGNALS:
		void fireMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler);
		void firePskChanged(const QByteArray& pPsk);
		void fireConnectedChanged(bool pConnected);
		void fireIsRunningChanged();
		void firePairingCompleted(const QSslCertificate& pCertificate);
		void fireSocketError(QAbstractSocket::SocketError pSocketError);
		void fireNameChanged();
};

} // namespace governikus
