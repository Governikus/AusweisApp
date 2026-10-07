/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "TlsServer.h"

namespace governikus
{

class RemoteTlsServer
	: public TlsServer
{
	Q_OBJECT

	private:
		QSslConfiguration sslConfiguration() const override;
		bool acceptSslErrors(const QPointer<QSslSocket>& pSocket, const QList<QSslError>& pErrors) const override;
		bool checkSslConfiguration(const QSslConfiguration& pSslConfiguration) const override;
		void updateClientInfo(const QSslConfiguration& pSslConfiguration) override;

	public:
		RemoteTlsServer();
		void setPairing(bool pEnable = true);
		bool startListening(quint16 pPort) override;
		void setRandomPsk();

	Q_SIGNALS:
		void firePairingCompleted(const QSslCertificate& pCertificate);
};

} // namespace governikus
