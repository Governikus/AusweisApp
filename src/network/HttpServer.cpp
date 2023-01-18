/*!
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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
	bindAddresses(pPort, pAddresses);
}


HttpServer::~HttpServer()
{
	shutdown();
}


void HttpServer::shutdown()
{
	if (isListening())
	{
		qCDebug(network) << "Shutdown server";
		for (const auto& server : std::as_const(mServer))
		{
			server->close();
		}
		mServer.clear();
	}
}


void HttpServer::bindAddresses(quint16 pPort, const QVector<QHostAddress>& pAddresses)
{
	auto port = pPort;

	if (pAddresses.isEmpty())
	{
		qCCritical(network) << "Cannot start server without addresses";
		return;
	}

	for (const auto& address : pAddresses)
	{
		auto server = std::make_unique<QTcpServer>(this);
		connect(server.get(), &QTcpServer::newConnection, this, &HttpServer::onNewConnection);

		if (!server->listen(address, port))
		{
			qCDebug(network) << "Cannot start server:" << server->errorString() << '|' << address << '|' << port;

			if (server->serverError() == QAbstractSocket::AddressInUseError)
			{
				qCDebug(network) << "Abort init of http server because of fatal error...";
				shutdown();
				break;
			}

			continue;
		}

		qCDebug(network) << "Listening on port:" << server->serverPort() << '|' << server->serverAddress();
		mServer += server.release();

		if (mServer.size() == 1)
		{
			port = mServer.constFirst()->serverPort();
			mPortFile.handlePort(port);
		}
	}

	if (isListening())
	{
		HttpServer::cPort = getServerPort();
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


void HttpServer::rebind(quint16 pPort, const QVector<QHostAddress>& pAddresses)
{
	qCDebug(network) << "Rebind HttpServer:" << pPort << '|' << pAddresses;
	mPortFile.remove();
	mServer.clear();
	bindAddresses(pPort, pAddresses);
	Q_EMIT fireRebound();
}


void HttpServer::onNewConnection()
{
	for (const auto& server : std::as_const(mServer))
	{
		while (server->hasPendingConnections())
		{
			auto* request = new HttpRequest(server->nextPendingConnection(), this);
			connect(request, &HttpRequest::fireMessageComplete, this, &HttpServer::onMessageComplete);
			request->triggerSocketBuffer();
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


QString HttpServer::getDefault()
{
	QStringList list;
	for (const auto& entry : std::as_const(HttpServer::cAddresses))
	{
		list << entry.toString();
	}
	return list.join(QLatin1Char(','));
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

		Q_EMIT fireNewHttpRequest(QSharedPointer<HttpRequest>(pRequest, &QObject::deleteLater));
	}
}
