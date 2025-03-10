/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "WebSocketServer.h"

namespace governikus
{

class RemoteWebSocketServer
	: public WebSocketServer
{
	Q_OBJECT

	public:
		~RemoteWebSocketServer() override;

		[[nodiscard]] virtual bool isPairingConnection() const = 0;
		[[nodiscard]] virtual bool isPairingAnnounced() const = 0;
		virtual void setPairing(bool pEnable = true) = 0;
		[[nodiscard]] virtual QSslCertificate getCurrentCertificate() const = 0;

	Q_SIGNALS:
		void firePairingCompleted(const QSslCertificate& pCertificate);

};

} // namespace governikus
