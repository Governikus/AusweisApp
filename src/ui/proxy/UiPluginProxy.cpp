/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginProxy.h"

#include "Env.h"
#include "RedirectBroadcast.h"
#include "RedirectRequest.h"

#include <QLoggingCategory>
#include <QNetworkDatagram>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(rproxy)


UiPluginProxy::UiPluginProxy()
	: UiPlugin()
	, HttpHandler()
	, mServer()
{
}


bool UiPluginProxy::initialize()
{
	return (mServer && mServer->isListening()) || listen();
}


bool UiPluginProxy::listen()
{
	mServer = Env::getShared<HttpServer>();

	if (mServer->isListening())
	{
		mSocket.reset(new QUdpSocket());
		connect(mSocket.data(), &QUdpSocket::readyRead, this, &UiPluginProxy::handleBroadcast);
		if (mSocket->bind(mServer->getServerPort()))
		{
			qCDebug(rproxy) << "UDP proxy port bound:" << mSocket->localPort();
		}
		else
		{
			qCWarning(rproxy) << "UDP proxy port failed:" << mServer->getServerPort();
		}

		Q_EMIT fireUiDominationRequest(this, tr("Reverse proxy plugin is enabled"));
		return true;
	}

	mServer.reset();
	return false;
}


void UiPluginProxy::onNewRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	handle(pRequest);
}


void UiPluginProxy::onNewWebSocketRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	new RedirectRequest(pRequest, this);
}


void UiPluginProxy::handleWorkflowRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	new RedirectRequest(pRequest, this);
}


void UiPluginProxy::handleBroadcast()
{
	while (mSocket->hasPendingDatagrams())
	{
		new RedirectBroadcast(mSocket->receiveDatagram(), mSocket->localPort());
	}
}


void UiPluginProxy::handleShowUiRequest(const QString& pUiModule, const QSharedPointer<HttpRequest>& pRequest)
{
	Q_UNUSED(pUiModule)
	new RedirectRequest(pRequest, this);
}


void UiPluginProxy::onUiDomination(const UiPlugin* pUi, const QString& pInformation, bool pAccepted)
{
	Q_UNUSED(pInformation)

	if (pUi == this && pAccepted)
	{
		connect(mServer.data(), &HttpServer::fireNewHttpRequest, this, &UiPluginProxy::onNewRequest);
		connect(mServer.data(), &HttpServer::fireNewWebSocketRequest, this, &UiPluginProxy::onNewWebSocketRequest);
		qCDebug(rproxy) << "Reverse proxy enabled";
		return;
	}
}


void UiPluginProxy::onUiDominationReleased()
{
	disconnect(mServer.data(), &HttpServer::fireNewHttpRequest, this, &UiPluginProxy::onNewRequest);
	disconnect(mServer.data(), &HttpServer::fireNewWebSocketRequest, this, &UiPluginProxy::onNewWebSocketRequest);
	qCDebug(rproxy) << "Reverse proxy disabled";
}


void UiPluginProxy::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)
}


void UiPluginProxy::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)
}


void UiPluginProxy::doShutdown()
{
	if (mServer)
	{
		mServer->disconnect(this);
		mServer.reset();
	}
}
