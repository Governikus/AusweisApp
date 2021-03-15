/*!
 * \brief Remote server service to offer smartphones as card readers (SaCs).
 * This class controls the advertising over UDP as well as the Websocket connection management.
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
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
		void onPskChanged(const QByteArray& pPsk);
		void onConnectedChanged(bool pConnected);

	public:
		RemoteServerImpl();

		[[nodiscard]] bool isRunning() const override;
		bool start(const QString& pServerName) override;
		void stop() override;
		void setPairing(bool pEnable = true) override;
		[[nodiscard]] bool isConnected() const override;
		[[nodiscard]] bool isPairingConnection() const override;
		[[nodiscard]] QSslCertificate getCurrentCertificate() const override;
		[[nodiscard]] const QSharedPointer<ServerMessageHandler>& getMessageHandler() const override;
};

} // namespace governikus
