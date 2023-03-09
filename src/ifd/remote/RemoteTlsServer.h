/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief QTcpServer with necessary TLS handling of remote device configuration.
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

	public:
		RemoteTlsServer();
		void setPairing(bool pEnable = true);
		bool startListening(quint16 pPort) override;
		[[nodiscard]] QSslCertificate getCurrentCertificate() const;

	public Q_SLOTS:
		void onEncrypted() override;
		void onSslErrors(const QList<QSslError>& pErrors) override;

	Q_SIGNALS:
		void firePairingCompleted();
};

} // namespace governikus
