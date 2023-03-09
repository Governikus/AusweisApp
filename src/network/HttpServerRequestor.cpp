/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "HttpServerRequestor.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QNetworkRequest>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(network)


HttpServerRequestor::HttpServerRequestor(bool pCustomNetworkManager)
	: QObject()
	, mNetworkManager(getNetworkManager(pCustomNetworkManager))
	, mEventLoop()
	, mTimer()
{
	Q_ASSERT(mNetworkManager && mNetworkManager->thread() == thread());
	connect(&mTimer, &QTimer::timeout, &mEventLoop, &QEventLoop::quit);
	mTimer.setSingleShot(true);
}


QPointer<NetworkManager> HttpServerRequestor::getNetworkManager(bool pCustomNetworkManager)
{
	if (pCustomNetworkManager)
	{
		auto* manager = new ServerRequestorManager();
		connect(this, &QObject::destroyed, manager, &NetworkManager::onShutdown, Qt::QueuedConnection);
		connect(this, &QObject::destroyed, manager, &QObject::deleteLater, Qt::QueuedConnection);
		return manager;
	}

	return Env::getSingleton<NetworkManager>();
}


QUrl HttpServerRequestor::createUrl(const QString& pQuery, quint16 pPort, const QHostAddress& pHost, const QString& pPath)
{
	QUrl url;
	url.setScheme(QStringLiteral("http"));
	url.setHost(pHost.toString());
	url.setPort(pPort);
	url.setPath(pPath);
	url.setQuery(pQuery);
	return url;
}


QSharedPointer<QNetworkReply> HttpServerRequestor::getRequest(QNetworkRequest pRequest, int pTimeOut)
{
	qCDebug(network) << "Request URL (GET):" << pRequest.url();
	auto reply = mNetworkManager ? mNetworkManager->get(pRequest) : nullptr;
	return waitForReply(reply, pTimeOut);
}


QSharedPointer<QNetworkReply> HttpServerRequestor::getRequest(const QUrl& pUrl, int pTimeOut)
{
	return getRequest(QNetworkRequest(pUrl), pTimeOut);
}


QSharedPointer<QNetworkReply> HttpServerRequestor::postRequest(const QUrl& pUrl, const QByteArray& pData, const QString& pContentType, int pTimeOut)
{
	QNetworkRequest request(pUrl);
	request.setHeader(QNetworkRequest::ContentTypeHeader, pContentType);
	qCDebug(network) << "Request URL (POST):" << pUrl;
	auto reply = mNetworkManager ? mNetworkManager->post(request, pData) : nullptr;
	return waitForReply(reply, pTimeOut);
}


QSharedPointer<QNetworkReply> HttpServerRequestor::deleteRequest(const QUrl& pUrl, int pTimeOut)
{
	QNetworkRequest request(pUrl);
	qCDebug(network) << "Request URL (DELETE):" << pUrl;
	auto reply = mNetworkManager ? mNetworkManager->deleteResource(request) : nullptr;
	return waitForReply(reply, pTimeOut);
}


QSharedPointer<QNetworkReply> HttpServerRequestor::waitForReply(QSharedPointer<QNetworkReply> pReply, int pTimeOut)
{
	if (!pReply)
	{
		return nullptr;
	}

	if (mEventLoop.isRunning())
	{
		qCWarning(network) << "Requestor already active...";
		return nullptr;
	}

	mTimer.start(pTimeOut);
	const auto connection = connect(pReply.data(), &QNetworkReply::finished, this, &HttpServerRequestor::finished);
	mEventLoop.exec();

	if (pReply->isFinished())
	{
		return pReply;
	}

	qCWarning(network) << "Request timed out.";

	disconnect(connection);
	pReply->abort();
	pReply->deleteLater();
	return nullptr;
}


void HttpServerRequestor::finished()
{
	mTimer.stop();
	mEventLoop.quit();
}
