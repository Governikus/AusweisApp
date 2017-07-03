/*
 * StateWriteHistory.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AppSettings.h"
#include "StateWriteHistory.h"

using namespace governikus;


StateWriteHistory::StateWriteHistory(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateWriteHistory::run()
{
	if (!AppSettings::getInstance().getHistorySettings().isEnabled())
	{
		qDebug() << "History disabled";
		Q_EMIT fireError();
		return;
	}

	if (getContext()->getDidAuthenticateEac1() == nullptr || getContext()->getEffectiveChat() == nullptr)
	{
		qWarning() << "No EAC1 structure or effective CHAT in model.";
		Q_EMIT fireError();
		return;
	}

	if (getContext()->getResult().isOk() &&
			getContext()->getDidAuthenticateEac1() != nullptr &&
			getContext()->getEffectiveChat() != nullptr)
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
			const auto& rights = getContext()->getEffectiveChat()->getAccessRights();
			for (const auto& entry : rights)
			{
				requestedData += AccessRoleAndRightsUtil::toDisplayText(entry);
			}

			if (!subjectName.isNull() && !termOfUsage.isNull())
			{
				HistoryEntry entry(subjectName, subjectUrl, certDesc->getPurpose(), QDateTime::currentDateTime(), termOfUsage.append("\n\n").append(validity), requestedData.join(QStringLiteral(", ")));
				AppSettings::getInstance().getHistorySettings().addHistoryEntry(entry);
				AppSettings::getInstance().getHistorySettings().save();
			}
		}
	}
	Q_EMIT fireSuccess();
}
