/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */
#include "HttpServerRequestor.h"

#include "HttpResponse.h"
#include "MockHttpServer.h"

#include <QHostAddress>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(HttpResponse)

class test_HttpServerRequestor
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void createUrl_data()
		{
			QTest::addColumn<QLatin1String>("query");
			QTest::addColumn<quint16>("port");
			QTest::addColumn<QHostAddress>("host");
			QTest::addColumn<QLatin1String>("path");
			QTest::addColumn<QUrl>("result");

			QTest::addRow("Localhost IPv4") << "foo=bar"_L1 << quint16(1337) << QHostAddress(QHostAddress::LocalHost) << "/some/path"_L1 << QUrl("http://127.0.0.1:1337/some/path?foo=bar"_L1);
			QTest::addRow("Localhost IPv6") << "foo=bar"_L1 << quint16(1337) << QHostAddress(QHostAddress::LocalHostIPv6) << "/some/path"_L1 << QUrl("http://[::1]:1337/some/path?foo=bar"_L1);
		}


		void createUrl()
		{
			QFETCH(QLatin1String, query);
			QFETCH(quint16, port);
			QFETCH(QHostAddress, host);
			QFETCH(QLatin1String, path);
			QFETCH(QUrl, result);

			const auto& url = HttpServerRequestor::createUrl(query, port, host, path);
			QCOMPARE(url.scheme(), "http"_L1);
			QCOMPARE(url.query(), query);
			QCOMPARE(url.port(), port);
			QCOMPARE(QHostAddress(url.host()), host);
			QCOMPARE(url.path(), path);
			QCOMPARE(url, result);
		}


		void getRequest_data()
		{
			QTest::addColumn<bool>("customManager");
			QTest::addColumn<QByteArray>("path");
			QTest::addColumn<HttpResponse>("response");

			QTest::addRow("HTTP_STATUS_OK") << false << "/foo"_ba << HttpResponse(HTTP_STATUS_OK, "bar");
			QTest::addRow("HTTP_STATUS_NOT_FOUND") << false << "/foo"_ba << HttpResponse(HTTP_STATUS_NOT_FOUND);
			QTest::addRow("HTTP_STATUS_OK_customManager") << true << "/foo"_ba << HttpResponse(HTTP_STATUS_OK, "bar");
			QTest::addRow("HTTP_STATUS_NOT_FOUND_customManager") << true << "/foo"_ba << HttpResponse(HTTP_STATUS_NOT_FOUND);
		}


		void getRequest()
		{
			QFETCH(bool, customManager);
			QFETCH(QByteArray, path);
			QFETCH(HttpResponse, response);

			MockHttpServer server;
			server.addMock(path, response, HTTP_GET);

			HttpServerRequestor requestor(customManager);
			const auto& reply = requestor.getRequest(server.getAddress(QString::fromLatin1(path)));
			QVERIFY(reply->isFinished());
			QCOMPARE(reply->readAll(), response.getBody());
			const auto& statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			QCOMPARE(statusCode, response.getStatus());
		}


		void postRequest_data()
		{
			QTest::addColumn<QByteArray>("path");
			QTest::addColumn<HttpResponse>("response");

			QTest::addRow("HTTP_STATUS_OK") << "/foo"_ba << HttpResponse(HTTP_STATUS_OK, "bar");
			QTest::addRow("HTTP_STATUS_NOT_FOUND") << "/foo"_ba << HttpResponse(HTTP_STATUS_NOT_FOUND);
		}


		void postRequest()
		{
			QFETCH(QByteArray, path);
			QFETCH(HttpResponse, response);

			MockHttpServer server;
			server.addMock(path, response, HTTP_POST);

			HttpServerRequestor requestor;
			const auto& reply = requestor.postRequest(server.getAddress(QString::fromLatin1(path)), "bar", "application/json"_L1);
			QVERIFY(reply->isFinished());
			QCOMPARE(reply->readAll(), response.getBody());
			const auto& statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			QCOMPARE(statusCode, response.getStatus());
		}


		void deleteRequest_data()
		{
			QTest::addColumn<QByteArray>("path");
			QTest::addColumn<HttpResponse>("response");

			QTest::addRow("HTTP_STATUS_OK") << "/foo"_ba << HttpResponse(HTTP_STATUS_OK, "bar");
			QTest::addRow("HTTP_STATUS_NOT_FOUND") << "/foo"_ba << HttpResponse(HTTP_STATUS_NOT_FOUND);
		}


		void deleteRequest()
		{
			QFETCH(QByteArray, path);
			QFETCH(HttpResponse, response);

			MockHttpServer server;
			server.addMock(path, response, HTTP_DELETE);

			HttpServerRequestor requestor;
			const auto& reply = requestor.deleteRequest(server.getAddress(QString::fromLatin1(path)));
			QVERIFY(reply->isFinished());
			QCOMPARE(reply->readAll(), response.getBody());
			const auto& statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			QCOMPARE(statusCode, response.getStatus());
		}


};

QTEST_GUILESS_MAIN(test_HttpServerRequestor)
#include "test_HttpServerRequestor.moc"
