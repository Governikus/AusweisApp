/*
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "HttpServerRequestor.h"
#include "NetworkManager.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QNetworkRequest>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(network)


HttpServerRequestor::HttpServerRequestor()
	: QObject()
	, mEventLoop()
	, mTimer()
{
	connect(&mTimer, &QTimer::timeout, &mEventLoop, &QEventLoop::quit);
	mTimer.setSingleShot(true);
}


HttpServerRequestor::~HttpServerRequestor()
{
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


QSharedPointer<QNetworkReply> HttpServerRequestor::request(const QUrl& pUrl, int pTimeOut)
{
	if (mEventLoop.isRunning())
	{
		qCWarning(network) << "Requestor already active...";
		return nullptr;
	}

	qCDebug(network) << "Request URL:" << pUrl;

	QNetworkRequest getRequest(pUrl);
	mTimer.start(pTimeOut);
	auto* reply = Env::getSingleton<NetworkManager>()->get(getRequest);
	const auto connection = connect(reply, &QNetworkReply::finished, this, &HttpServerRequestor::finished);
	mEventLoop.exec();

	if (reply->isFinished())
	{
		return QSharedPointer<QNetworkReply>(reply, &QObject::deleteLater);
	}

	disconnect(connection);
	reply->abort();
	reply->deleteLater();
	return nullptr;
}


void HttpServerRequestor::finished()
{
	mTimer.stop();
	mEventLoop.quit();
}
