/*!
 * CertificateChecker.h
 *
 * \brief Contains the definition of the CertificateChecker class.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "AppSettings.h"
#include "context/AuthContext.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QLoggingCategory>
#include <QSet>
#include <QSslCertificate>
#include <QString>

Q_DECLARE_LOGGING_CATEGORY(developermode)

namespace governikus
{

/*!
 * \brief Utility class for checking various constraints on certificates
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */
class CertificateChecker
{
	Q_GADGET

	static bool isValidKeyLength(int pKeyLength, QSsl::KeyAlgorithm pKeyAlgorithm, bool pIsEphemeral);

	public:
		enum class CertificateStatus
		{
			Good,
			Unsupported_Algorithm_Or_Length,
			Hash_Not_In_Description
		};
		Q_ENUM(CertificateStatus)

		static QString toString(QSsl::KeyAlgorithm pKeyAlgorithm);

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

		/*!
		 * Checks and save certificate into given WorkflowContext
		 * \return Returns a translated error string if an error happened, otherwise QString()
		 */
		static CertificateStatus checkAndSaveCertificate(const QSslCertificate& pCertificate, const QUrl& pUrl, QSharedPointer<AuthContext> pContext);
};

} // namespace governikus
