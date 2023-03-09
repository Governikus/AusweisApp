/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MockHttpServer.h"

#include "Env.h"

#include <QTest>
#include <http_parser.h>

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


void MockHttpServer::addMock(const QByteArray& pUrl, const HttpResponse& pResponse, const http_method& pHttpMethod)
{
	const MockKey mockUrl = {pHttpMethod, pUrl};
	mMock.insert(mockUrl, pResponse);
}


QUrl MockHttpServer::getAddress(const QString& pPath) const
{
	const auto& port = QString::number(mServer->getServerPort());
	return QUrl(QStringLiteral("http://localhost:") + port + pPath);
}


void MockHttpServer::onNewHttpRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	QVERIFY(pRequest);

	const auto& url = pRequest->getUrl().toEncoded();
	const MockKey mockUrl = {pRequest->getHttpMethod(), url};
	QVERIFY2(mMock.contains(mockUrl), qPrintable(QStringLiteral("No mock entry for \"%1\"").arg(mockUrl)));

	const auto& response = mMock.value(mockUrl);
	pRequest->send(response);
}
