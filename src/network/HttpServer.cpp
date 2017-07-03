/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "HttpServer.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(network)

quint16 HttpServer::cPort = 24727;

HttpServer::HttpServer(quint16 pPort)
	: QObject()
	, mServer(new QTcpServer)
{
	connect(mServer.data(), &QTcpServer::newConnection, this, &HttpServer::onNewConnection);

	if (mServer->listen(QHostAddress::LocalHost, pPort))
	{
		qCDebug(network) << "Listening on port:" << mServer->serverPort();
	}
	else
	{
		qCDebug(network) << "Cannot start server:" << mServer->errorString();
	}
}


HttpServer::~HttpServer()
{
	if (isListening())
	{
		qCDebug(network) << "Shutdown server";
		mServer->close();
	}
}


bool HttpServer::isListening() const
{
	return mServer->isListening();
}


quint16 HttpServer::getServerPort() const
{
	return mServer->serverPort();
}


void HttpServer::onNewConnection()
{
	while (mServer->hasPendingConnections())
	{
		auto socket = mServer->nextPendingConnection();
		socket->startTransaction();
		auto request = new HttpRequest(socket, this);
		connect(request, &HttpRequest::fireMessageComplete, this, &HttpServer::onMessageComplete);
	}
}


void HttpServer::onMessageComplete(HttpRequest* pRequest, QSharedPointer<QAbstractSocket> pSocket)
{
	pRequest->setParent(nullptr);

	if (pRequest->isUpgrade())
	{
		pRequest->deleteLater();
		pSocket->rollbackTransaction();

		if (pRequest->getHeader(QByteArrayLiteral("upgrade")).toLower() == QByteArrayLiteral("websocket"))
		{
			qCDebug(network) << "Upgrade to websocket requested";
			Q_EMIT fireNewWebSocketRequest(pSocket);
		}
		else
		{
			qCWarning(network) << "Unknown upgrade requested";
			pRequest->send(HttpStatusCode::NOT_FOUND);
		}
	}
	else
	{
		pSocket->commitTransaction();
		Q_EMIT fireNewHttpRequest(QSharedPointer<HttpRequest>(pRequest, &QObject::deleteLater));
	}
}
