/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateRedirectBrowser.h"

#include "ECardApiResult.h"

#include <QDebug>


using namespace governikus;


namespace
{
inline QString removePrefix(QString pStr)
{
	return pStr.replace(QRegularExpression(QStringLiteral("(.*)#")), QLatin1String(""));
}


inline QString getSuffix(ECardApiResult::Minor pMinor)
{
	return removePrefix(ECardApiResult::getMinorString(pMinor));
}


} // namespace


StateRedirectBrowser::StateRedirectBrowser(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateRedirectBrowser::run()
{
	const auto& context = getContext();
	if (const auto& url = context->getRefreshUrl(); url.isValid())
	{
		const auto& result = context->getStartPaosResult().isOk() ? ECardApiResult(context->getStatus()) : context->getStartPaosResult();
		context->setRefreshUrl(addMajorMinor(url, GlobalStatus(result)));
	}
	else
	{
		qDebug() << "Refresh URL is not valid:" << url;
		if (const auto& tcToken = context->getTcToken(); tcToken)
		{
			if (const auto& address = context->getTcToken()->getCommunicationErrorAddress(); address.isValid())
			{
				context->setRefreshUrl(addMajorMinor(address, GlobalStatus::Code::Workflow_Communication_Missing_Redirect_Url));
			}
			else
			{
				qDebug() << "CommunicationErrorAddress is not valid:" << address;
			}
		}
		else
		{
			qDebug() << "TcToken is missing";
		}
	}

	if (const auto& handler = context->getBrowserHandler(); handler)
	{
		if (const auto& error = handler(context); !error.isEmpty())
		{
			qCritical() << "Cannot send page to caller:" << error;
			context->setReceivedBrowserSendFailed(true);
			updateStatus({GlobalStatus::Code::Workflow_Browser_Transmission_Error, {GlobalStatus::ExternalInformation::ACTIVATION_ERROR, error}
					});
			Q_EMIT fireAbort(FailureCode::Reason::Browser_Send_Failed);
			return;
		}
	}

	Q_EMIT fireContinue();
}


QUrl StateRedirectBrowser::addMajorMinor(const QUrl& pOriginUrl, const GlobalStatus& pStatus)
{
	QUrlQuery q;
	q.setQuery(pOriginUrl.query());

	const ECardApiResult::Major majorEnumVal = pStatus.isError() ? ECardApiResult::Major::Error : ECardApiResult::Major::Ok;
	QString major = removePrefix(ECardApiResult::getMajorString(majorEnumVal));
	q.addQueryItem(QStringLiteral("ResultMajor"), major);

	if (pStatus.isError())
	{
		QString minor;

		switch (pStatus.getStatusCode())
		{
			case GlobalStatus::Code::Paos_Error_AL_Communication_Error:
				minor = getSuffix(ECardApiResult::Minor::AL_Communication_Error);
				break;

			case GlobalStatus::Code::Paos_Error_DP_Trusted_Channel_Establishment_Failed:
				minor = getSuffix(ECardApiResult::Minor::DP_Trusted_Channel_Establishment_Failed);
				break;

			case GlobalStatus::Code::Paos_Error_SAL_Cancellation_by_User:
				minor = getSuffix(ECardApiResult::Minor::SAL_Cancellation_by_User);
				break;

			default:
				if (pStatus.isOriginServer())
				{
					minor = QStringLiteral("serverError");
				}
				else
				{
					minor = QStringLiteral("clientError");
				}
				break;
		}
		q.addQueryItem(QStringLiteral("ResultMinor"), minor);
	}

	QUrl adaptedUrl(pOriginUrl);
	adaptedUrl.setQuery(q);
	return adaptedUrl;
}
