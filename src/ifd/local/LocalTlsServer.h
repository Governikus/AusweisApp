/*!
 * \brief QTcpServer with necessary TLS handling of remote device configuration.
 *
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "TlsServer.h"


namespace governikus
{

class LocalTlsServer
	: public TlsServer
{
	Q_OBJECT

	private:
		QSslConfiguration sslConfiguration() const override;

	public:
		LocalTlsServer() = default;
		bool startListening(quint16 pPort) override;

	private Q_SLOTS:
		void onSslErrors(const QList<QSslError>& pErrors) override;
		void onEncrypted() override;
};

} // namespace governikus
