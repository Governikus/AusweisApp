/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "StateRedirectBrowser.h"

#include "ECardApiResult.h"

#include <QDebug>


using namespace governikus;


StateRedirectBrowser::StateRedirectBrowser(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateRedirectBrowser::onEntry(QEvent* pEvent)
{
	AbstractState::onEntry(pEvent);

	const auto& context = getContext();
	if (const auto& url = context->getRefreshUrl(); url.isValid())
	{
		const auto& status = context->getStatus();
		const auto& startPaosResult = context->getStartPaosResult();
		if (status.isCancellationByUser() || startPaosResult.isOk())
		{
			context->setRefreshUrl(addMajorMinor(url, ECardApiResult(status)));
		}
		else
		{
			context->setRefreshUrl(addMajorMinor(url, startPaosResult));
		}
	}
	else
	{
		qDebug() << "Refresh URL is not valid:" << url;
		if (const auto& tcToken = context->getTcToken(); tcToken)
		{
			if (const auto& address = context->getTcToken()->getCommunicationErrorAddress(); address.isValid())
			{
				context->setRefreshUrl(addMajorMinor(address, ECardApiResult(GlobalStatus::Code::Workflow_Communication_Missing_Redirect_Url)));
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
}


void StateRedirectBrowser::run()
{
	const auto& context = getContext();

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


QUrl StateRedirectBrowser::addMajorMinor(const QUrl& pOriginUrl, const ECardApiResult& pResult)
{
	QUrlQuery q;
	q.setQuery(pOriginUrl.query());
	q.addQueryItem(QStringLiteral("ResultMajor"), pResult.getRedirectMajor());
	if (!pResult.isOk())
	{
		q.addQueryItem(QStringLiteral("ResultMinor"), pResult.getRedirectMinor());
	}

	QUrl adaptedUrl(pOriginUrl);
	adaptedUrl.setQuery(q);
	return adaptedUrl;
}
