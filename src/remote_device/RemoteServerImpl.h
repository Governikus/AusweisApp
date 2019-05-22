/*!
 * \brief Remote server service to offer remote readers.
 * This class controls the advertising over UDP as well as the Websocket connection management.
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteReaderAdvertiser.h"
#include "RemoteServer.h"

namespace governikus
{

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

} // namespace governikus
