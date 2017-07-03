/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgHandlerCertificate.h"

using namespace governikus;

MsgHandlerCertificate::MsgHandlerCertificate(const MsgContext& pContext)
	: MsgHandler(MsgType::CERTIFICATE)
{
	auto ctx = pContext.getAuthContext();
	Q_ASSERT(ctx);
	Q_ASSERT(ctx->getDidAuthenticateEac1());
	Q_ASSERT(ctx->getDidAuthenticateEac1()->getCertificateDescription());
	Q_ASSERT(!ctx->getDidAuthenticateEac1()->getCvCertificates().isEmpty());

	const auto& eac1 = ctx->getDidAuthenticateEac1();
	const auto& certificateDescription = eac1->getCertificateDescription();

	QJsonObject desc;
	desc["subjectName"] = certificateDescription->getSubjectName();
	desc["subjectUrl"] = certificateDescription->getSubjectUrl();
	desc["issuerName"] = certificateDescription->getIssuerName();
	desc["issuerUrl"] = certificateDescription->getIssuerUrl();
	desc["termsOfUsage"] = certificateDescription->getTermsOfUsage();

	CVCertificateBody body = eac1->getCvCertificates().at(0)->getBody();
	QJsonObject validity;
	validity["effectiveDate"] = body.getCertificateEffectiveDate().toString(Qt::ISODate);
	validity["expirationDate"] = body.getCertificateExpirationDate().toString(Qt::ISODate);

	mJsonObject["description"] = desc;
	mJsonObject["validity"] = validity;
}
