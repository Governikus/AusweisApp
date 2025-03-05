/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "CertificateChecker.h"

#include "AppSettings.h"
#include "TlsChecker.h"

#include <QCryptographicHash>
#include <QDate>
#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(developermode)


bool CertificateChecker::abortOnError()
{
	const auto& hashError = QStringLiteral("hash of certificate not in certificate description");

	if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
	{
		qCCritical(developermode) << hashError;
		return false;
	}
	else
	{
		qCritical() << hashError;
		return true;
	}
}


GlobalStatus::Code CertificateChecker::getGlobalStatus(CertificateStatus pStatus, bool pPaos)
{
	switch (pStatus)
	{
		case CertificateStatus::Unsupported_Algorithm_Or_Length:
			return pPaos
				   ? GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Certificate_Unsupported_Algorithm_Or_Length
				   : GlobalStatus::Code::Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length;

		case CertificateStatus::Hash_Not_In_Description:
			return pPaos
				   ? GlobalStatus::Code::Workflow_TrustedChannel_Hash_Not_In_Description
				   : GlobalStatus::Code::Workflow_Network_Ssl_Hash_Not_In_Certificate_Description;

		case CertificateStatus::Good:
			return GlobalStatus::Code::No_Error;
	}

	Q_UNREACHABLE();
}


CertificateChecker::CertificateStatus CertificateChecker::checkAndSaveCertificate(const QSslCertificate& pCertificate,
		const QUrl& pUrl,
		const QSharedPointer<AuthContext> pContext)
{
	Q_ASSERT(!pContext.isNull());

	if (!TlsChecker::hasValidCertificateKeyLength(pCertificate))
	{
		return CertificateStatus::Unsupported_Algorithm_Or_Length;
	}

	const auto& eac1 = pContext->getDidAuthenticateEac1();
	const auto& dvCvc = pContext->getDvCvc();
	if (eac1 && dvCvc)
	{
		if (const auto& certificateDescription = eac1->getCertificateDescription())
		{
			const QSet<QString>& certHashes = certificateDescription->getCommCertificates();
			QCryptographicHash::Algorithm hashAlgo = dvCvc->getBody().getHashAlgorithm();
			if (!TlsChecker::checkCertificate(pCertificate, hashAlgo, certHashes) && abortOnError())
			{
				return CertificateStatus::Hash_Not_In_Description;
			}
		}
	}

	pContext->addCertificateData(pUrl, pCertificate);
	return CertificateStatus::Good;
}
