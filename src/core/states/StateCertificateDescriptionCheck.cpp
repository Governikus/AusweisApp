/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateCertificateDescriptionCheck.h"

#include "asn1/CVCertificateChainBuilder.h"
#include "asn1/KnownOIDs.h"
#include "AppSettings.h"
#include "UrlUtil.h"


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(developermode)


StateCertificateDescriptionCheck::StateCertificateDescriptionCheck(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{

}


void StateCertificateDescriptionCheck::run()
{
	Q_ASSERT(getContext()->getTerminalCvc());
	Q_ASSERT(!getContext()->getDidAuthenticateEac1().isNull());
	const auto& terminalCertificate = getContext()->getTerminalCvc();

	if (getContext()->getDidAuthenticateEac1()->getCertificateDescriptionAsBinary().isNull())
	{
		qCritical() << "No certificate description available";
		updateStatus(GlobalStatus::Code::Workflow_Certificate_No_Description);
		Q_EMIT fireAbort();
		return;
	}

	if (getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getSubjectUrl().isNull())
	{
		qCritical() << "No subject url available in certificate description";
		updateStatus(GlobalStatus::Code::Workflow_Certificate_No_Url_In_Description);
		Q_EMIT fireAbort();
		return;
	}

	QCryptographicHash hashCalculator(terminalCertificate->getBody().getHashAlgorithm());
	hashCalculator.addData(getContext()->getDidAuthenticateEac1()->getCertificateDescriptionAsBinary());

	const auto& idDescOid = toByteArray(KnownOIDs::CertificateExtensions::ID_DESCRIPTION);
	const QByteArray& hashOfDescription = terminalCertificate->getBody().getExtensions().value(idDescOid);
	if (hashCalculator.result() != hashOfDescription)
	{
		auto certificateHashError = QStringLiteral("The certificate description does not match the certificate.");
		if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
		{
			qCCritical(developermode) << certificateHashError;
		}
		else
		{
			qCritical() << certificateHashError;
			updateStatus(GlobalStatus::Code::Workflow_Certificate_Hash_Error);
			Q_EMIT fireAbort();
			return;
		}
	}

	// check same origin policy for TCToken URL and subject URL
	const QUrl& subjectUrl = getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getSubjectUrl();
	if (!UrlUtil::isMatchingSameOriginPolicy(subjectUrl, getContext()->getTcTokenUrl()))
	{
		auto sameOriginPolicyError = QStringLiteral("The subject URL in the certificate description and the TCToken URL don't satisfy the same origin policy.");
		if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
		{
			qCCritical(developermode) << sameOriginPolicyError;
		}
		else
		{
			qCritical() << sameOriginPolicyError;
			updateStatus(GlobalStatus::Code::Workflow_Certificate_Sop_Error);
			Q_EMIT fireAbort();
			return;
		}
	}

	Q_EMIT fireContinue();
}
