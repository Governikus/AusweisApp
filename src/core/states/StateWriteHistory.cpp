/*
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateWriteHistory.h"

#include "asn1/AccessRoleAndRight.h"

#include "AppSettings.h"

using namespace governikus;


StateWriteHistory::StateWriteHistory(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
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
			auto termOfUsage = certDesc->getTermsOfUsage();
			auto subjectUrl = certDesc->getSubjectUrl();

			CVCertificateBody body = getContext()->getDidAuthenticateEac1()->getCvCertificates().at(0)->getBody();
			QString effectiveDate = body.getCertificateEffectiveDate().toString(Qt::DefaultLocaleShortDate);
			QString expirationDate = body.getCertificateExpirationDate().toString(Qt::DefaultLocaleShortDate);
			QString validity = tr("Validity:\n%1 - %2").arg(effectiveDate, expirationDate);

			QStringList requestedData;
			QList<AccessRight> rights = getContext()->getEffectiveAccessRights().toList();
			std::sort(rights.begin(), rights.end());
			for (const auto& entry : qAsConst(rights))
			{
				const auto data = AccessRoleAndRightsUtil::toTechnicalName(entry);
				if (!data.isEmpty())
				{
					requestedData += data;
				}
			}

			if (!subjectName.isNull() && !termOfUsage.isNull())
			{
				HistoryInfo info(subjectName, subjectUrl, certDesc->getPurpose(), QDateTime::currentDateTime(), termOfUsage + QStringLiteral("\n\n") + validity, requestedData);
				Env::getSingleton<AppSettings>()->getHistorySettings().addHistoryInfo(info);
				Env::getSingleton<AppSettings>()->getHistorySettings().save();
			}
		}
	}
	Q_EMIT fireContinue();
}
