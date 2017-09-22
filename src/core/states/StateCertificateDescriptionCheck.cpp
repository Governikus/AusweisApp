/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
	auto terminalCertificate = getContext()->getTerminalCvc();

	if (getContext()->getDidAuthenticateEac1()->getCertificateDescriptionAsBinary().isNull())
	{
		qCritical() << "No certificate description available";
		setResult(Result::createParameterError(tr("No certificate description available.")));
		Q_EMIT fireError();
		return;
	}

	if (getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getSubjectUrl().isNull())
	{
		qCritical() << "No subject url available in certificate description";
		setResult(Result::createParameterError(tr("No subject url available in certificate description.")));
		Q_EMIT fireError();
		return;
	}

	QCryptographicHash hashCalculator(terminalCertificate->getBody().getHashAlgorithm());
	hashCalculator.addData(getContext()->getDidAuthenticateEac1()->getCertificateDescriptionAsBinary());

	const QByteArray hashOfDescription = terminalCertificate->getBody().getExtensions().value(KnownOIDs::CertificateExtensions::id_description);
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
			setResult(Result::createParameterError(tr("The certificate description does not match the certificate.")));
			Q_EMIT fireError();
			return;
		}
	}

	// check same origin policy for TCToken URL and subject URL
	QUrl subjectUrl = getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getSubjectUrl();
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
			setResult(Result::createParameterError(tr("The subject URL in the certificate description and the TCToken URL don't satisfy the same origin policy.")));
			Q_EMIT fireError();
			return;
		}
	}

	Q_EMIT fireSuccess();
}
