/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "StateRedirectBrowser.h"

#include "UrlUtil.h"

#include <QCoreApplication>
#include <QDebug>
#include <QUrlQuery>


using namespace governikus;


StateRedirectBrowser::StateRedirectBrowser(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateRedirectBrowser::run()
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	// Only skip redirects on mobile platforms because it induces a forced focus change
	if (getContext()->isSkipRedirect())
	{
		qDebug() << "Skipping redirect, Workflow pending";
		Q_EMIT fireContinue();
	}
	else
#endif
	if (getContext()->getStatus() == GlobalStatus::Code::Workflow_InternalError_BeforeTcToken)
	{
		sendErrorPage(HTTP_STATUS_INTERNAL_SERVER_ERROR);
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
		if (getContext()->getStartPaosResult().isOk())
		{
			redirectSuccess = sendRedirect(getContext()->getRefreshUrl(), ECardApiResult(getContext()->getStatus()));
		}
		else
		{
			redirectSuccess = sendRedirect(getContext()->getRefreshUrl(), getContext()->getStartPaosResult());
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
		updateStatus({GlobalStatus::Code::Workflow_Error_Page_Transmission_Error, {GlobalStatus::ExternalInformation::ACTIVATION_ERROR, activationContext->getSendError()}
				});
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
		updateStatus({GlobalStatus::Code::Workflow_Redirect_Transmission_Error, {GlobalStatus::ExternalInformation::ACTIVATION_ERROR, activationContext->getSendError()}
				});
		Q_EMIT fireAbort();
		return false;
	}

	return true;
}
