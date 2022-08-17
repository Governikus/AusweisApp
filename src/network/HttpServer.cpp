/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "HttpServer.h"

#include <QLoggingCategory>
#include <QTcpSocket>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(network)


quint16 HttpServer::cPort = PortFile::cDefaultPort;
QVector<QHostAddress> HttpServer::cAddresses = {QHostAddress::LocalHost, QHostAddress::LocalHostIPv6};


HttpServer::HttpServer(quint16 pPort, const QVector<QHostAddress>& pAddresses)
	: QObject()
	, mServer()
	, mPortFile()
{
	auto port = pPort;

	if (pAddresses.isEmpty())
	{
		qCCritical(network) << "Cannot start server without addresses";
		return;
	}

	for (const auto& address : pAddresses)
	{
		const QSharedPointer<QTcpServer> server(new QTcpServer(), &QObject::deleteLater);
		connect(server.data(), &QTcpServer::newConnection, this, &HttpServer::onNewConnection);

		if (!server->listen(address, port))
		{
			qCDebug(network) << "Cannot start server:" << server->errorString() << '|' << address << '|' << port;
			continue;
		}

		qCDebug(network) << "Listening on port:" << server->serverPort() << '|' << server->serverAddress();
		mServer += server;

		if (mServer.size() == 1)
		{
			port = mServer.constFirst()->serverPort();
			mPortFile.handlePort(port);
		}
	}
}


HttpServer::~HttpServer()
{
	if (isListening())
	{
		qCDebug(network) << "Shutdown server";
		for (const auto& server : qAsConst(mServer))
		{
			server->close();
		}
	}
}


int HttpServer::boundAddresses() const
{
	return mServer.size();
}


bool HttpServer::isListening() const
{
	return std::any_of(mServer.begin(), mServer.end(), [](const auto& server){
			return server->isListening();
		});
}


quint16 HttpServer::getServerPort() const
{
	if (mServer.isEmpty())
	{
		return 0;
	}

	return mServer.constFirst()->serverPort();
}


void HttpServer::onNewConnection()
{
	for (const auto& server : qAsConst(mServer))
	{
		while (server->hasPendingConnections())
		{
			auto socket = server->nextPendingConnection();
			socket->startTransaction();
			auto request = new HttpRequest(socket, this);
			connect(request, &HttpRequest::fireMessageComplete, this, &HttpServer::onMessageComplete);
		}
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
