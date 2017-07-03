/*!
 * \brief Default configuration for TLS connections
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "SecureStorage.h"

#include <QList>
#include <QSslCipher>
#include <QSslConfiguration>
#include <QSslEllipticCurve>
#include <QSslError>

namespace governikus
{

class SslCipherList
	: public QList<QSslCipher>
{
	public:
		SslCipherList& operator +=(const QString& pCipherName);
};

class SslEllipticCurveVector
	: public QVector<QSslEllipticCurve>
{
	public:
		SslEllipticCurveVector& operator +=(const QString& pEllipticCurveName);
};


class TlsConfiguration
{
	private:
		static QSslConfiguration mConfig;
		static QSslConfiguration mPskConfig;

		static SslEllipticCurveVector mEllipticCurves;
		static SslCipherList mPskCiphers;
		static SslCipherList mCiphersWithForwardSecrecy;
		static SslCipherList mCiphersWithSha1ForBackwardCompatibility;

		TlsConfiguration();
		~TlsConfiguration();
		Q_DISABLE_COPY(TlsConfiguration)

	public:
		static const QList<QSslCipher>& getCiphersWithPsk();
		static const QList<QSslCipher>& getCiphersWithForwardSecrecy();
		static const QList<QSslCipher>& getCiphersWithSha1ForBackwardCompatibility();
		static const QVector<QSslEllipticCurve>& getAllowedSslEllipticCurves();

		/*!
		 * Create SSL/TLS configuration for TLS-1-2 and every other connection
		 * like ProviderService and Updates.
		 */
		static QSslConfiguration createSslConfiguration();

		/*!
		 * Create SSL/TLS configuration for TLS-2 with RSA-PSK cipher suites.
		 */
		static QSslConfiguration createPskSslConfiguration();

		static QStringList getFatalErrors(const QList<QSslError>& pErrors);
		static bool containsFatalError(const QList<QSslError>& pErrors);
};

} /* namespace governikus */
