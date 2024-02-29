/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Remote Ifd server service to offer smartphones as card readers (SaCs).
 * This class controls the advertising over UDP as well as the Websocket connection management.
 */

#pragma once

#include "IfdServer.h"
#include "RemoteReaderAdvertiser.h"
#include "RemoteWebSocketServer.h"

namespace governikus
{

class RemoteIfdServer
	: public IfdServer
{
	Q_OBJECT

	private:
		QScopedPointer<RemoteReaderAdvertiser> mRemoteReaderAdvertiser;
		QScopedPointer<RemoteWebSocketServer> mWebSocketServer;

	private Q_SLOTS:
		void onPskChanged(const QByteArray& pPsk);
		void onConnectedChanged(bool pConnected);

	public:
		RemoteIfdServer();

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
