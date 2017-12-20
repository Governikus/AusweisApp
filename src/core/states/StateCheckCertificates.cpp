/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "StateCheckCertificates.h"

#include "AppSettings.h"
#include "TlsChecker.h"

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(developermode)


StateCheckCertificates::StateCheckCertificates(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
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
			if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
			{
				qCCritical(developermode) << certificateDescError;
			}
			else
			{
				qCritical() << certificateDescError;
				updateStatus(GlobalStatus::Code::Workflow_TrustedChannel_Hash_Not_In_Description);
				Q_EMIT fireAbort();
				return;
			}
		}
	}
	Q_EMIT fireContinue();
}
