/*
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
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
	, mReply(nullptr)
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


QPointer<QNetworkReply> HttpServerRequestor::request(const QUrl& pUrl, int pTimeOut)
{
	qCDebug(network) << "Request URL:" << pUrl;

	QNetworkRequest getRequest(pUrl);
	mTimer.start(pTimeOut);
	mReply.reset(Env::getSingleton<NetworkManager>()->get(getRequest));
	connect(mReply.data(), &QNetworkReply::finished, this, &HttpServerRequestor::finished);
	mEventLoop.exec();

	return mReply->isFinished() ? mReply.data() : nullptr;
}


void HttpServerRequestor::finished()
{
	mTimer.stop();
	mEventLoop.quit();
}
