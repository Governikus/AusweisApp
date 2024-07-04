/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateCheckCertificates.h"

#include "AppSettings.h"
#include "TlsChecker.h"

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(developermode)


StateCheckCertificates::StateCheckCertificates(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateCheckCertificates::run()
{
	const auto& commCertificates = getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getCommCertificates();
	const auto& hashAlgorithm = getContext()->getDvCvc()->getBody().getHashAlgorithm();

	// check the certificates we've encountered so far
	const auto& certList = getContext()->getCertificateList();
	for (const auto& certificate : certList)
	{
		if (!TlsChecker::checkCertificate(certificate, hashAlgorithm, commCertificates))
		{
			auto certificateDescError = QStringLiteral("Hash of certificate not in certificate description");
			if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
			{
				qCCritical(developermode) << certificateDescError;
			}
			else
			{
				qCritical() << certificateDescError;
				updateStatus({GlobalStatus::Code::Workflow_TrustedChannel_Hash_Not_In_Description,
							  {
								  GlobalStatus::ExternalInformation::CERTIFICATE_ISSUER_NAME, certificate.issuerDisplayName()
							  }
						});
				Q_EMIT fireAbort(FailureCode::Reason::Certificate_Check_Failed_Hash_Missing_In_Description);
				return;
			}
		}
	}
	Q_EMIT fireContinue();
}
