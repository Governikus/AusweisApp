/*
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "StateWriteHistory.h"

#include "asn1/AccessRoleAndRight.h"

#include "AppSettings.h"

using namespace governikus;


StateWriteHistory::StateWriteHistory(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateWriteHistory::run()
{
	if (!Env::getSingleton<AppSettings>()->getHistorySettings().isEnabled())
	{
		qDebug() << "History disabled";
		Q_EMIT fireContinue();
		return;
	}

	if (getContext()->getDidAuthenticateEac1() == nullptr || getContext()->getEffectiveAccessRights().isEmpty())
	{
		qWarning() << "No EAC1 structure or effective CHAT in model.";
		Q_EMIT fireAbort();
		return;
	}

	if (getContext()->getStatus().isNoError())
	{
		if (auto certDesc = getContext()->getDidAuthenticateEac1()->getCertificateDescription())
		{
			auto subjectName = certDesc->getSubjectName();
			auto termsOfUsage = certDesc->getTermsOfUsage();
			auto subjectUrl = certDesc->getSubjectUrl();

			CVCertificateBody body = getContext()->getDidAuthenticateEac1()->getCvCertificates().at(0)->getBody();
			QString effectiveDate = body.getCertificateEffectiveDate().toString(Qt::DefaultLocaleShortDate);
			QString expirationDate = body.getCertificateExpirationDate().toString(Qt::DefaultLocaleShortDate);
			//: LABEL ALL_PLATFORMS
			QString validity = tr("Validity:\n%1 - %2").arg(effectiveDate, expirationDate);

			QStringList requestedData;
			QList<AccessRight> rights = getContext()->getEffectiveAccessRights().values();
			std::sort(rights.begin(), rights.end());
			for (const auto& entry : qAsConst(rights))
			{
				const auto data = AccessRoleAndRightsUtil::toTechnicalName(entry);
				if (!data.isEmpty())
				{
					requestedData += data;
				}
			}

			if (!subjectName.isNull())
			{
				termsOfUsage = termsOfUsage.isEmpty() ? validity : termsOfUsage + QStringLiteral("\n\n") + validity;
				HistoryInfo info(subjectName, subjectUrl, certDesc->getPurpose(), QDateTime::currentDateTime(), termsOfUsage, requestedData);
				Env::getSingleton<AppSettings>()->getHistorySettings().addHistoryInfo(info);
				Env::getSingleton<AppSettings>()->getHistorySettings().save();
			}
		}
	}
	Q_EMIT fireContinue();
}


void StateWriteHistory::onEntry(QEvent* pEvent)
{
	//: INFO ALL_PLATFORMS Status message after the authentication was completed, the results are prepared for the user, mainly relevant for the self authentication since it takes some more time.
	const auto text = getContext()->getStatus().isNoError() ? tr("Preparing results") : QString(); // The empty string is set to not confuse users when they get redirected to the provider
	getContext()->setProgress(100, text);
	AbstractState::onEntry(pEvent);
}
