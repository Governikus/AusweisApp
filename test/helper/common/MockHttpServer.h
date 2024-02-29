/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Provide a HTTP-Server for tests
 */

#pragma once

#include "HttpServer.h"

#include "HttpResponse.h"

#include <http_parser.h>


namespace governikus
{

struct MockKey
{
	http_method method;
	QByteArray url;

	operator QString() const
	{
		const QByteArray methodStr = http_method_str(method);
		return QStringLiteral("Method: %1, Url: %2").arg(QString::fromLatin1(methodStr), QString::fromUtf8((url)));
	}
};


inline bool operator<(const MockKey& pLeft, const MockKey& pRight)
{
	return QString(pLeft) < QString(pRight);
}


class MockHttpServer
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<HttpServer> mServer;
		QMap<MockKey, HttpResponse> mMock;

	public:
		MockHttpServer();

		void reset();
		void addMock(const QByteArray& pUrl, const HttpResponse& pResponse, const http_method& pHttpMethod = HTTP_POST);
		[[nodiscard]] QUrl getAddress(const QString& pPath = QString()) const;

	private Q_SLOTS:
		void onNewHttpRequest(const QSharedPointer<HttpRequest>& pRequest);

};


} // namespace governikus
