/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "HttpServer.h"

#include <QLoggingCategory>
#include <QTcpSocket>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(network)


quint16 HttpServer::cPort = PortFile::cDefaultPort;


HttpServer::HttpServer(quint16 pPort)
	: QObject()
	, mServer(new QTcpServer())
	, mPortFile()
{
	connect(mServer.data(), &QTcpServer::newConnection, this, &HttpServer::onNewConnection);

	if (mServer->listen(QHostAddress::LocalHost, pPort))
	{
		mPortFile.handlePort(mServer->serverPort());
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


bool HttpServer::checkReceiver(const QMetaMethod& pSignal, HttpRequest* pRequest)
{
	if (isSignalConnected(pSignal))
	{
		return true;
	}

	qCDebug(network) << "No registration found:" << pSignal.name();
	pRequest->send(HTTP_STATUS_SERVICE_UNAVAILABLE);
	pRequest->deleteLater();
	return false;
}


void HttpServer::onMessageComplete(HttpRequest* pRequest)
{
	pRequest->setParent(nullptr);

	if (pRequest->isUpgrade())
	{
		if (pRequest->getHeader(QByteArrayLiteral("upgrade")).toLower() == QByteArrayLiteral("websocket"))
		{
			qCDebug(network) << "Upgrade to websocket requested";

			static const QMetaMethod signal = QMetaMethod::fromSignal(&HttpServer::fireNewWebSocketRequest);
			if (!checkReceiver(signal, pRequest))
			{
				return;
			}

			pRequest->mSocket->rollbackTransaction();
			Q_EMIT fireNewWebSocketRequest(QSharedPointer<HttpRequest>(pRequest, &QObject::deleteLater));
		}
		else
		{
			qCWarning(network) << "Unknown upgrade requested";
			pRequest->send(HTTP_STATUS_NOT_FOUND);
			pRequest->deleteLater();
		}
	}
	else
	{
		static const QMetaMethod signal = QMetaMethod::fromSignal(&HttpServer::fireNewHttpRequest);
		if (!checkReceiver(signal, pRequest))
		{
			return;
		}

		pRequest->mSocket->commitTransaction();
		Q_EMIT fireNewHttpRequest(QSharedPointer<HttpRequest>(pRequest, &QObject::deleteLater));
	}
}
