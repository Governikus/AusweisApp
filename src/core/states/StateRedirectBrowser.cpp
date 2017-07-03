/*!
 * StateRedirectBrowser.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StateRedirectBrowser.h"

#include "UrlUtil.h"

#include <QCoreApplication>
#include <QDebug>
#include <QUrlQuery>


using namespace governikus;


StateRedirectBrowser::StateRedirectBrowser(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateRedirectBrowser::run()
{
	if (getContext()->isTcTokenNotFound())
	{
		sendErrorPage(HttpStatusCode::NOT_FOUND);
	}
	else if (getContext()->getRefreshUrl().isEmpty())
	{
		reportCommunicationError();
	}
	else if (sendRedirect(getContext()->getRefreshUrl(), getContext()->getStatus()))
	{
		Q_EMIT fireSuccess();
	}
}


void StateRedirectBrowser::sendErrorPage(HttpStatusCode pStatus)
{
	auto activationContext = getContext()->getActivationContext();
	if (activationContext->sendErrorPage(pStatus, getContext()->getStatus()))
	{
		Q_EMIT fireSuccess();
	}
	else
	{
		qCritical() << "Cannot send error page to caller: " << activationContext->getSendError();
		setStatus(GlobalStatus(GlobalStatus::Code::Workflow_Error_Page_Transmission_Error, activationContext->getSendError()));
		Q_EMIT fireError();
	}
}


void StateRedirectBrowser::reportCommunicationError()
{
	qDebug() << "Report communication error";
	if (getContext()->getTcToken() != nullptr && getContext()->getTcToken()->getCommunicationErrorAddress().isValid())
	{
		if (sendRedirect(getContext()->getTcToken()->getCommunicationErrorAddress(), GlobalStatus::Code::Workflow_Communication_Missing_Redirect_Url))
		{
			Q_EMIT fireSuccess();
		}
	}
	else
	{
		sendErrorPage(HttpStatusCode::BAD_REQUEST);
	}
}


bool StateRedirectBrowser::sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus)
{
	auto activationContext = getContext()->getActivationContext();
	if (activationContext->sendRedirect(pRedirectAddress, pStatus))
	{
		return true;
	}
	else
	{
		qCritical() << "Cannot send redirect to caller: " << activationContext->getSendError();
		setStatus(GlobalStatus(GlobalStatus::Code::Workflow_Redirect_Transmission_Error, activationContext->getSendError()));
		Q_EMIT fireError();
		return false;
	}
}
