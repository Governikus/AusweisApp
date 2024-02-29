/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Provide a IfdServer for tests
 */

#pragma once

#include "IfdServer.h"

#include <QTest>

namespace governikus
{

class MockIfdServer
	: public IfdServer
{
	Q_OBJECT

	private:
		bool mRunning;
		bool mConnected;
		bool mPairing;
		bool mIsLocal;
		QSharedPointer<ServerMessageHandler> mServerMessageHandler;

	public:
		MockIfdServer(bool pIsLocal = false);

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

		[[nodiscard]] virtual bool isLocal() const override
		{
			return mIsLocal;
		}


};

} // namespace governikus
