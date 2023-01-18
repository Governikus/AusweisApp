/*!
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInProxy.h"

#include "Env.h"
#include "RedirectRequest.h"

#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(rproxy)


UIPlugInProxy::UIPlugInProxy()
	: UIPlugIn()
	, mServer()
{
}


bool UIPlugInProxy::initialize()
{
	return (mServer && mServer->isListening()) || listen();
}


bool UIPlugInProxy::listen()
{
	mServer = Env::getShared<HttpServer>();

	if (mServer->isListening())
	{
		Q_EMIT fireUiDominationRequest(this, tr("Reverse proxy plugin is enabled"));
		return true;
	}

	mServer.reset();
	return false;
}


void UIPlugInProxy::onNewRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	handle(pRequest);
}


void UIPlugInProxy::onNewWebSocketRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	new RedirectRequest(pRequest, this);
}


void UIPlugInProxy::handleWorkflowRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	new RedirectRequest(pRequest, this);
}


void UIPlugInProxy::handleShowUiRequest(const QString& pUiModule, const QSharedPointer<HttpRequest>& pRequest)
{
	Q_UNUSED(pUiModule)
	new RedirectRequest(pRequest, this);
}


void UIPlugInProxy::onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted)
{
	Q_UNUSED(pInformation)

	if (pUi == this && pAccepted)
	{
		connect(mServer.data(), &HttpServer::fireNewHttpRequest, this, &UIPlugInProxy::onNewRequest);
		connect(mServer.data(), &HttpServer::fireNewWebSocketRequest, this, &UIPlugInProxy::onNewWebSocketRequest);
		qCDebug(rproxy) << "Reverse proxy enabled";
		return;
	}
}


void UIPlugInProxy::onUiDominationReleased()
{
	disconnect(mServer.data(), &HttpServer::fireNewHttpRequest, this, &UIPlugInProxy::onNewRequest);
	disconnect(mServer.data(), &HttpServer::fireNewWebSocketRequest, this, &UIPlugInProxy::onNewWebSocketRequest);
	qCDebug(rproxy) << "Reverse proxy disabled";
}


void UIPlugInProxy::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext)
}


void UIPlugInProxy::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext)
}


void UIPlugInProxy::doShutdown()
{
}
