/*!
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#include "MockHttpServer.h"

#include "Env.h"

using namespace governikus;

MockHttpServer::MockHttpServer()
	: QObject()
	, mServer()
{
	HttpServer::cPort = 0;
	mServer = Env::getShared<HttpServer>();
	QVERIFY(mServer);
	QVERIFY(mServer->isListening());
	connect(mServer.data(), &HttpServer::fireNewHttpRequest, this, &MockHttpServer::onNewHttpRequest);
}


void MockHttpServer::reset()
{
	mMock.clear();
}


void MockHttpServer::addMock(const QByteArray& pUrl, const HttpResponse& pResponse)
{
	mMock.insert(pUrl, pResponse);
}


QUrl MockHttpServer::getAddress(const QString& pPath) const
{
	const auto& port = QString::number(mServer->getServerPort());
	return QUrl(QStringLiteral("http://localhost:") + port + pPath);
}


void MockHttpServer::onNewHttpRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	QVERIFY(pRequest);
	const auto& response = mMock.value(pRequest->getUrl().toEncoded());
	if (response.isValid())
	{
		pRequest->send(response);
	}
	else
	{
		pRequest->send(HttpStatusCode::INTERNAL_SERVER_ERROR);
	}
}
