/*!
 * CertificateChecker.h
 *
 * \brief Contains the definition of the CertificateChecker class.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "AppSettings.h"

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
	static bool isValidKeyLength(int pKeyLength, QSsl::KeyAlgorithm pKeyAlgorithm, bool pIsEphemeral);

	public:
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
		template<typename ModelClass> static QString checkAndSaveCertificate(const QSslCertificate& pCertificate, const QUrl& pUrl, QSharedPointer<ModelClass> pContext)
		{
			Q_ASSERT(!pContext.isNull());

			if (!hasValidCertificateKeyLength(pCertificate))
			{
				return QObject::tr("Error while connecting to the server. The SSL certificate uses an unsupported key algorithm or length.");
			}

			// the call to cvc.isSyntaxValid is made to check, whether the cvc is set
			auto eac1 = pContext->getDidAuthenticateEac1();
			if (eac1 && pContext->getDvCvc())
			{
				if (auto certificateDescription = eac1->getCertificateDescription())
				{
					const QSet<QString> certHashes = certificateDescription->getCommCertificates();
					QCryptographicHash::Algorithm hashAlgo = pContext->getDvCvc()->getBody().getHashAlgorithm();
					if (!checkCertificate(pCertificate, hashAlgo, certHashes))
					{
						auto hashError = QStringLiteral("hash of certificate not in certificate description");

						if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
						{
							qCCritical(developermode) << hashError;
						}
						else
						{
							qCritical() << hashError;
							return QObject::tr("hash of certificate not in certificate description");
						}
					}
				}
			}

			pContext->addCertificateData(pUrl, pCertificate);
			return QString();
		}


};

} // namespace governikus
