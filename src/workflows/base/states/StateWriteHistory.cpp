/*
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "StateWriteHistory.h"

#include "asn1/AccessRoleAndRight.h"

#include "AppSettings.h"
#include "LanguageLoader.h"

using namespace governikus;


StateWriteHistory::StateWriteHistory(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
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

	const auto& context = getContext();
	if (context->getStatus().isError())
	{
		Q_EMIT fireContinue();
		return;
	}

	if (context->getDidAuthenticateEac1() == nullptr || context->getAccessRightManager()->getEffectiveAccessRights().isEmpty())
	{
		qWarning() << "No EAC1 structure or effective CHAT in model.";
		Q_EMIT fireAbort();
		return;
	}

	if (auto certDesc = context->getDidAuthenticateEac1()->getCertificateDescription())
	{
		auto subjectName = certDesc->getSubjectName();
		auto termsOfUsage = certDesc->getTermsOfUsage();
		auto subjectUrl = certDesc->getSubjectUrl();

		CVCertificateBody body = context->getAccessRightManager()->getTerminalCvc()->getBody();
		const auto locale = LanguageLoader::getInstance().getUsedLocale();
		const auto effectiveDate = locale.toString(body.getCertificateEffectiveDate(), QLocale::ShortFormat);
		const auto expirationDate = locale.toString(body.getCertificateExpirationDate(), QLocale::ShortFormat);
		//: LABEL ALL_PLATFORMS
		QString validity = tr("Validity:\n%1 - %2").arg(effectiveDate, expirationDate);

		QStringList requestedData;
		QList<AccessRight> rights = context->getAccessRightManager()->getEffectiveAccessRights().values();
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
		}
	}

	Q_EMIT fireContinue();
}


void StateWriteHistory::onEntry(QEvent* pEvent)
{
	const auto& context = getContext();
	if (context->getAction() == Action::AUTH)
	{
		//: INFO ALL_PLATFORMS Status message after the authentication was completed, the results are prepared for the user and the process will be continued in the browser
		const auto text = context->getStatus().isNoError() ? tr("Preparing results") : QString(); // The empty string is set to not confuse users when they get redirected to the provider
		context->setProgress(100, text);
	}
	AbstractState::onEntry(pEvent);
}
