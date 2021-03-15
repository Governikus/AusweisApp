/*!
 * \brief Provide a RemoteServer for tests
 *
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteServer.h"

#include <QTest>

namespace governikus
{

class MockRemoteServer
	: public RemoteServer
{
	Q_OBJECT

	private:
		bool mRunning;
		bool mConnected;
		bool mPairing;
		QSharedPointer<ServerMessageHandler> mServerMessageHandler;

	public:
		MockRemoteServer();

		[[nodiscard]] bool isRunning() const override;
		bool start(const QString& pServerName) override;
		void stop() override;
		void setPairing(bool pEnable) override;
		bool getPairing();
		[[nodiscard]] bool isConnected() const override;
		[[nodiscard]] bool isPairingConnection() const override;
		void setConnected(bool pConnected);
		[[nodiscard]] QSslCertificate getCurrentCertificate() const override;
		[[nodiscard]] const QSharedPointer<ServerMessageHandler>& getMessageHandler() const override;
		void setMessageHandler(const QSharedPointer<ServerMessageHandler>& pHandler);
};

} // namespace governikus
