/*!
 * \brief Default configuration for TLS connections
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "SecureStorage.h"

#include <QList>
#include <QNetworkReply>
#include <QSslCipher>
#include <QSslConfiguration>
#include <QSslEllipticCurve>
#include <QSslError>

namespace governikus
{


class TlsConfiguration
{
	private:
		TlsConfiguration();
		~TlsConfiguration();
		Q_DISABLE_COPY(TlsConfiguration)

	public:
		static QStringList getFatalErrors(const QList<QSslError>& pErrors);
		static bool containsFatalError(QNetworkReply* pReply, const QList<QSslError>& pErrors);
};

} /* namespace governikus */
