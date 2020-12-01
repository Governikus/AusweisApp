/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "CertificateChecker.h"

#include "AppSettings.h"
#include "TlsChecker.h"

#include <QCryptographicHash>
#include <QDate>
#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(developermode)


CertificateChecker::CertificateStatus CertificateChecker::checkAndSaveCertificate(const QSslCertificate& pCertificate,
		const QUrl& pUrl,
		const QSharedPointer<DIDAuthenticateEAC1>& pEAC1,
		const QSharedPointer<const CVCertificate>& pDvCvc,
		const std::function<void(const QUrl& pUrl, const QSslCertificate& pCert)>& pSaveCertificateFunc)
{
	if (!TlsChecker::hasValidCertificateKeyLength(pCertificate))
	{
		return CertificateStatus::Unsupported_Algorithm_Or_Length;
	}

	if (pEAC1 && pDvCvc)
	{
		if (const auto& certificateDescription = pEAC1->getCertificateDescription())
		{
			const QSet<QString>& certHashes = certificateDescription->getCommCertificates();
			QCryptographicHash::Algorithm hashAlgo = pDvCvc->getBody().getHashAlgorithm();
			if (!TlsChecker::checkCertificate(pCertificate, hashAlgo, certHashes))
			{
				const auto& hashError = QStringLiteral("hash of certificate not in certificate description");

				if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
				{
					qCCritical(developermode) << hashError;
				}
				else
				{
					qCritical() << hashError;
					return CertificateStatus::Hash_Not_In_Description;
				}
			}
		}
	}

	pSaveCertificateFunc(pUrl, pCertificate);
	return CertificateStatus::Good;
}
