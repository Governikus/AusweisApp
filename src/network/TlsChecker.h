/*!
 * \brief Helper to check certificates and other security stuff of TLS/SSL.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QCryptographicHash>
#include <QNetworkReply>
#include <QSet>
#include <QSslCertificate>
#include <QSslCipher>
#include <QSslConfiguration>
#include <QSslKey>

namespace governikus
{

class TlsChecker
{
	private:
		TlsChecker() = delete;
		static bool isValidKeyLength(int pKeyLength, QSsl::KeyAlgorithm pKeyAlgorithm, bool pIsEphemeral);

	public:
		static void logSslConfig(const QSslConfiguration pCfg, QDebug pDebug);
		static QString toString(QSsl::SslProtocol pProtocol);
		static QString toString(QSsl::KeyAlgorithm pKeyAlgorithm);

		static QStringList getFatalErrors(const QList<QSslError>& pErrors);
		static bool containsFatalError(QNetworkReply* pReply, const QList<QSslError>& pErrors);

		/*!
		 * Checks, whether the certificate's hash is contained in a set of accepted certificate hashes.
		 */
		static bool checkCertificate(const QSslCertificate& pCertificate,
				QCryptographicHash::Algorithm pAlgorithm,
				const QSet<QString>& pAcceptedCertificateHashes);

		/*!
		 * Checks, whether the key length of the SSL certificate is of sufficient length.
		 */
		static bool hasValidCertificateKeyLength(const QSslCertificate& pCertificate);

		/*!
		 * Checks, whether the length of the ephemeral key is of sufficient length.
		 */
		static bool hasValidEphemeralKeyLength(const QSslKey& pEphemeralServerKey);
};

} /* namespace governikus */
