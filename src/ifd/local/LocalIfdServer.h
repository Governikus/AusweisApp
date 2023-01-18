/*!
 * \brief Local Ifd server service to offer SaC to localhost websocket connections.
 *
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdServer.h"
#include "LocalWebSocketServer.h"

namespace governikus
{

class LocalIfdServer
	: public IfdServer
{
	Q_OBJECT

	private:
		QScopedPointer<LocalWebSocketServer> mWebSocketServer;

	public:
		LocalIfdServer();

		void setPsk(const QByteArray& pPsk);

		[[nodiscard]] bool isRunning() const override;
		bool start(const QString& pServerName) override;
		void stop() override;
		void setPairing(bool pEnable = true) override;
		[[nodiscard]] bool isConnected() const override;
		[[nodiscard]] bool isPairingConnection() const override;
		[[nodiscard]] QSslCertificate getCurrentCertificate() const override;
		[[nodiscard]] const QSharedPointer<ServerMessageHandler>& getMessageHandler() const override;

		[[nodiscard]] bool isLocal() const override;


};

} // namespace governikus
