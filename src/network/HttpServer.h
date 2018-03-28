/*!
 * \brief Provide a HTTP server.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpRequest.h"

#include <QScopedPointer>
#include <QSharedPointer>
#include <QTcpServer>
#include <QTcpSocket>

namespace governikus
{

class HttpServer
	: public QObject
{
	Q_OBJECT

	private:
		QScopedPointer<QTcpServer, QScopedPointerDeleteLater> mServer;

	public:
		static quint16 cPort;

		HttpServer(quint16 pPort = HttpServer::cPort);
		virtual ~HttpServer();

		bool isListening() const;
		quint16 getServerPort() const;

	private Q_SLOTS:
		void onNewConnection();
		void onMessageComplete(HttpRequest* pRequest, QSharedPointer<QAbstractSocket> pSocket);

	Q_SIGNALS:
		void fireNewHttpRequest(const QSharedPointer<HttpRequest>& pRequest);
		void fireNewWebSocketRequest(const QSharedPointer<QAbstractSocket>& pSocket);
};

} /* namespace governikus */
