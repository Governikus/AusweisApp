/*!
 * \brief Provide a HTTP-Server for tests
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpServer.h"

#include "HttpResponse.h"

#include <QTest>

namespace governikus
{

class MockHttpServer
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<HttpServer> mServer;
		QMap<QByteArray, HttpResponse> mMock;

	public:
		MockHttpServer();

		void reset();
		void addMock(const QByteArray& pUrl, const HttpResponse& pResponse);
		QUrl getAddress(const QString& pPath = QString()) const;

	private Q_SLOTS:
		void onNewHttpRequest(const QSharedPointer<HttpRequest>& pRequest);
};

} // namespace governikus
