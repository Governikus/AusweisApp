/*!
 * \brief Remote server service to offer remote readers.
 * This class controls the advertising over UDP as well as the Websocket connection management.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>
#include <QObject>
#include <QScopedPointer>
#include <QSslCertificate>
#include <QString>

namespace governikus
{

class ServerMessageHandler;

class RemoteServer
	: public QObject
{
	Q_OBJECT

	protected:
		RemoteServer() = default;

	public:
		virtual ~RemoteServer();

		virtual bool isRunning() const = 0;
		virtual bool start(const QString& pServerName) = 0;
		virtual void stop() = 0;
		virtual void setPairing(bool pEnable = true) = 0;
		virtual bool isConnected() const = 0;
		virtual QSslCertificate getCurrentCertificate() const = 0;
		virtual const QSharedPointer<ServerMessageHandler>& getMessageHandler() const = 0;

	Q_SIGNALS:
		void fireMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler);
		void firePskChanged(const QByteArray& pPsk);
		void fireConnectedChanged(bool pConnected);
};


class RemoteReaderAdvertiser;
class RemoteWebSocketServer;


class RemoteServerImpl
	: public RemoteServer
{
	Q_OBJECT

	private:
		QScopedPointer<RemoteReaderAdvertiser> mRemoteReaderAdvertiser;
		QScopedPointer<RemoteWebSocketServer> mWebSocketServer;

	private Q_SLOTS:
		void onConnectedChanged(bool pConnected);

	public:
		RemoteServerImpl();

		virtual bool isRunning() const override;
		virtual bool start(const QString& pServerName) override;
		virtual void stop() override;
		virtual void setPairing(bool pEnable = true) override;
		virtual bool isConnected() const override;
		virtual QSslCertificate getCurrentCertificate() const override;
		virtual const QSharedPointer<ServerMessageHandler>& getMessageHandler() const override;
};

} /* namespace governikus */
