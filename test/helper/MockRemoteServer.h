/*!
 * \brief Provide a RemoteServer for tests
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
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

		virtual bool isRunning() const override;
		virtual bool start(const QString& pServerName) override;
		virtual void stop() override;
		virtual void setPairing(bool pEnable) override;
		bool getPairing();
		virtual bool isConnected() const override;
		virtual bool isPairingConnection() const override;
		void setConnected(bool pConnected);
		virtual QSslCertificate getCurrentCertificate() const override;
		virtual const QSharedPointer<ServerMessageHandler>& getMessageHandler() const override;
		void setMessageHandler(const QSharedPointer<ServerMessageHandler>& pHandler);
};

} // namespace governikus
