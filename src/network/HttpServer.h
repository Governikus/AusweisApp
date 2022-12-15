/*!
 * \brief Provide a HTTP server.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpRequest.h"
#include "PortFile.h"

#include <QMetaMethod>
#include <QTcpServer>
#include <QVector>

namespace governikus
{

class HttpServer
	: public QObject
{
	Q_OBJECT

	private:
		QVector<QTcpServer*> mServer;
		PortFile mPortFile;

		void shutdown();
		void bindAddresses(quint16 pPort, const QVector<QHostAddress>& pAddresses);
		bool checkReceiver(const QMetaMethod& pSignal, HttpRequest* pRequest);

	public:
		static quint16 cPort;
		static QVector<QHostAddress> cAddresses;

		explicit HttpServer(quint16 pPort = HttpServer::cPort,
				const QVector<QHostAddress>& pAddresses = HttpServer::cAddresses);
		~HttpServer() override;

		[[nodiscard]] int boundAddresses() const;
		[[nodiscard]] bool isListening() const;
		[[nodiscard]] quint16 getServerPort() const;
		void rebind(quint16 pPort = 0, const QVector<QHostAddress>& pAddresses = HttpServer::cAddresses);

	private Q_SLOTS:
		void onNewConnection();
		void onMessageComplete(HttpRequest* pRequest);

	Q_SIGNALS:
		void fireNewHttpRequest(const QSharedPointer<HttpRequest>& pRequest);
		void fireNewWebSocketRequest(const QSharedPointer<HttpRequest>& pRequest);
		void fireRebound();
};

} // namespace governikus
