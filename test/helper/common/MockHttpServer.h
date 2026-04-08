/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpServer.h"

#include "HttpResponse.h"

#include <llhttp.h>


namespace governikus
{

struct MockKey
{
	llhttp_method method;
	QByteArray url;

	operator QString() const
	{
		const QByteArray methodStr = llhttp_method_name(method);
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
		void addMock(const QByteArray& pUrl, const HttpResponse& pResponse, const llhttp_method& pHttpMethod = HTTP_POST);
		[[nodiscard]] QUrl getAddress(const QString& pPath = QString()) const;

	private Q_SLOTS:
		void onNewHttpRequest(const QSharedPointer<HttpRequest>& pRequest);

};


} // namespace governikus
