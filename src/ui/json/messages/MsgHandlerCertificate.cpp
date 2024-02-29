/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerCertificate.h"

#include "context/AuthContext.h"

using namespace governikus;

MsgHandlerCertificate::MsgHandlerCertificate(const MsgContext& pContext)
	: MsgHandler(MsgType::CERTIFICATE)
{
	auto ctx = pContext.getContext<AuthContext>();
	Q_ASSERT(ctx);
	Q_ASSERT(ctx->getDidAuthenticateEac1());
	Q_ASSERT(ctx->getDidAuthenticateEac1()->getCertificateDescription());
	Q_ASSERT(ctx->getAccessRightManager());
	Q_ASSERT(ctx->getAccessRightManager()->getTerminalCvc());

	const auto& eac1 = ctx->getDidAuthenticateEac1();
	const auto& certificateDescription = eac1->getCertificateDescription();

	QJsonObject desc;
	desc[QLatin1String("subjectName")] = certificateDescription->getSubjectName();
	desc[QLatin1String("subjectUrl")] = certificateDescription->getSubjectUrl();
	desc[QLatin1String("issuerName")] = certificateDescription->getIssuerName();
	desc[QLatin1String("issuerUrl")] = certificateDescription->getIssuerUrl();
	desc[QLatin1String("termsOfUsage")] = certificateDescription->getTermsOfUsage();
	desc[QLatin1String("purpose")] = certificateDescription->getPurpose();

	CVCertificateBody body = ctx->getAccessRightManager()->getTerminalCvc()->getBody();
	QJsonObject validity;
	validity[QLatin1String("effectiveDate")] = body.getCertificateEffectiveDate().toString(Qt::ISODate);
	validity[QLatin1String("expirationDate")] = body.getCertificateExpirationDate().toString(Qt::ISODate);

	mJsonObject[QLatin1String("description")] = desc;
	mJsonObject[QLatin1String("validity")] = validity;
}
