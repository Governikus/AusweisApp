/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
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
	if (getContext()->isSkipRedirect())
	{
		qDebug() << "Skipping redirect, Workflow pending";
		Q_EMIT fireContinue();
	}
	else if (getContext()->isTcTokenNotFound())
	{
		sendErrorPage(HTTP_STATUS_NOT_FOUND);
	}
	else if (getContext()->getRefreshUrl().isEmpty())
	{
		reportCommunicationError();
	}
	else
	{
		bool redirectSuccess;
		if (!getContext()->getStartPaosResult().isOk())
		{
			redirectSuccess = sendRedirect(getContext()->getRefreshUrl(), getContext()->getStartPaosResult());
		}
		else
		{
			redirectSuccess = sendRedirect(getContext()->getRefreshUrl(), getContext()->getStatus());
		}

		if (redirectSuccess)
		{
			Q_EMIT fireContinue();
		}
	}
}


void StateRedirectBrowser::sendErrorPage(http_status pStatus)
{
	auto activationContext = getContext()->getActivationContext();
	if (activationContext->sendErrorPage(pStatus, getContext()->getStatus()))
	{
		Q_EMIT fireContinue();
	}
	else
	{
		qCritical() << "Cannot send error page to caller:" << activationContext->getSendError();
		updateStatus(GlobalStatus(GlobalStatus::Code::Workflow_Error_Page_Transmission_Error, activationContext->getSendError()));
		Q_EMIT fireAbort();
	}
}


void StateRedirectBrowser::reportCommunicationError()
{
	qDebug() << "Report communication error";
	if (getContext()->getTcToken() != nullptr && getContext()->getTcToken()->getCommunicationErrorAddress().isValid())
	{
		if (sendRedirect(getContext()->getTcToken()->getCommunicationErrorAddress(), ECardApiResult(GlobalStatus::Code::Workflow_Communication_Missing_Redirect_Url)))
		{
			Q_EMIT fireContinue();
		}
	}
	else
	{
		sendErrorPage(HTTP_STATUS_BAD_REQUEST);
	}
}


bool StateRedirectBrowser::sendRedirect(const QUrl& pRedirectAddress, const ECardApiResult& pResult)
{
	auto activationContext = getContext()->getActivationContext();
	if (!activationContext->sendRedirect(pRedirectAddress, GlobalStatus(pResult)))
	{
		qCritical() << "Cannot send redirect to caller:" << activationContext->getSendError();
		updateStatus(GlobalStatus(GlobalStatus::Code::Workflow_Redirect_Transmission_Error, activationContext->getSendError()));
		Q_EMIT fireAbort();
		return false;
	}

	return true;
}
