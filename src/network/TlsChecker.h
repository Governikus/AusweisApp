/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Helper to check certificates and other security stuff of TLS/SSL.
 */

#pragma once

#include "FailureCode.h"
#include "LogHandler.h"

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
		static const std::function<int(QSsl::KeyAlgorithm)> cDefaultFuncMinKeySize;

		[[nodiscard]] static bool isValidKeyLength(int pKeyLength, QSsl::KeyAlgorithm pKeyAlgorithm, int pMinKeySize);

		TlsChecker() = delete;
		~TlsChecker() = delete;

	public:
		static void logSslConfig(const QSslConfiguration& pCfg, const MessageLogger& pLogger);
		[[nodiscard]] static QString toString(QSsl::SslProtocol pProtocol);
		[[nodiscard]] static QString toString(QSsl::KeyAlgorithm pKeyAlgorithm);

		[[nodiscard]] static QStringList getFatalErrors(const QList<QSslError>& pErrors);
		[[nodiscard]] static bool containsFatalError(const QSharedPointer<QNetworkReply>& pReply, const QList<QSslError>& pErrors);
		[[nodiscard]] static QString sslErrorsToString(const QList<QSslError>& pErrors);

		/*!
		 * Checks, whether the certificate's hash is contained in a set of accepted certificate hashes.
		 */
		[[nodiscard]] static bool checkCertificate(const QSslCertificate& pCertificate,
				QCryptographicHash::Algorithm pAlgorithm,
				const QSet<QString>& pAcceptedCertificateHashes);

		/*!
		 * Checks, whether the key length of the SSL certificate is of sufficient length.
		 */
		[[nodiscard]] static bool hasValidCertificateKeyLength(const QSslCertificate& pCertificate,
				const std::function<int(QSsl::KeyAlgorithm)>& pFuncMinKeySize = cDefaultFuncMinKeySize);

		/*!
		 * Checks, whether the length of the ephemeral key is of sufficient length.
		 */
		[[nodiscard]] static bool hasValidEphemeralKeyLength(const QSslKey& pEphemeralServerKey,
				const std::function<int(QSsl::KeyAlgorithm)>& pFuncMinKeySize = cDefaultFuncMinKeySize);
		[[nodiscard]] static FailureCode::FailureInfoMap getEphemeralKeyInfoMap(const QSslKey& pEphemeralServerKey);

		/*!
		 * This method is only needed until QSslCertificate provides its own method issuerDisplayName in Qt 5.12
		 */
		[[nodiscard]] static QString getCertificateIssuerName(const QSslCertificate& pCertificate);

		[[nodiscard]] static QSslCertificate getRootCertificate(const QList<QSslCertificate>& pCertificates);
};

} // namespace governikus
