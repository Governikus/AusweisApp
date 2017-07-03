/*!
 * StateCheckCertificates.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StateCheckCertificates.h"

#include "AppSettings.h"
#include "CertificateChecker.h"

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(developermode)


StateCheckCertificates::StateCheckCertificates(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateCheckCertificates::run()
{
	auto commCertificates = getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getCommCertificates();
	auto hashAlgorithm = getContext()->getDvCvc()->getBody().getHashAlgorithm();

	// check the certificates we've encountered so far
	const auto certList = getContext()->getCertificateList();
	for (const auto& certificate : certList)
	{
		if (!CertificateChecker::checkCertificate(certificate, hashAlgorithm, commCertificates))
		{
			auto certificateDescError = QStringLiteral("Hash of certificate not in certificate description");
			if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
			{
				qCCritical(developermode) << certificateDescError;
			}
			else
			{
				qCritical() << certificateDescError;
				setResult(Result::createTrustedChannelEstablishmentError(tr("Hash of certificate not in certificate description")));
				Q_EMIT fireError();
				return;
			}
		}
	}
	Q_EMIT fireSuccess();
}
