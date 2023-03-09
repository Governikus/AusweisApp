/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */
#include "HttpServerRequestor.h"

#include "HttpResponse.h"
#include "MockHttpServer.h"

#include <QHostAddress>
#include <QtTest>

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
			QTest::addColumn<QString>("query");
			QTest::addColumn<quint16>("port");
			QTest::addColumn<QHostAddress>("host");
			QTest::addColumn<QString>("path");
			QTest::addColumn<QUrl>("result");

			QTest::addRow("Localhost IPv4") << "foo=bar" << quint16(1337) << QHostAddress(QHostAddress::LocalHost) << "/some/path" << QUrl("http://127.0.0.1:1337/some/path?foo=bar");
			QTest::addRow("Localhost IPv6") << "foo=bar" << quint16(1337) << QHostAddress(QHostAddress::LocalHostIPv6) << "/some/path" << QUrl("http://[::1]:1337/some/path?foo=bar");
		}


		void createUrl()
		{
			QFETCH(QString, query);
			QFETCH(quint16, port);
			QFETCH(QHostAddress, host);
			QFETCH(QString, path);
			QFETCH(QUrl, result);

			const auto& url = HttpServerRequestor::createUrl(query, port, host, path);
			QCOMPARE(url.scheme(), "http");
			QCOMPARE(url.query(), query);
			QCOMPARE(url.port(), port);
			QCOMPARE(QHostAddress(url.host()), host);
			QCOMPARE(url.path(), path);
			QCOMPARE(url, result);
		}


		void getRequest_data()
		{
			QTest::addColumn<bool>("customManager");
			QTest::addColumn<QString>("path");
			QTest::addColumn<HttpResponse>("response");

			QTest::addRow("HTTP_STATUS_OK") << false << "/foo" << HttpResponse(HTTP_STATUS_OK, "bar");
			QTest::addRow("HTTP_STATUS_NOT_FOUND") << false << "/foo" << HttpResponse(HTTP_STATUS_NOT_FOUND);
			QTest::addRow("HTTP_STATUS_OK_customManager") << true << "/foo" << HttpResponse(HTTP_STATUS_OK, "bar");
			QTest::addRow("HTTP_STATUS_NOT_FOUND_customManager") << true << "/foo" << HttpResponse(HTTP_STATUS_NOT_FOUND);
		}


		void getRequest()
		{
			QFETCH(bool, customManager);
			QFETCH(QString, path);
			QFETCH(HttpResponse, response);

			MockHttpServer server;
			server.addMock(path.toLocal8Bit(), response, HTTP_GET);

			HttpServerRequestor requestor(customManager);
			const auto& reply = requestor.getRequest(server.getAddress(path));
			QVERIFY(reply->isFinished());
			QCOMPARE(reply->readAll(), response.getBody());
			const auto& statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			QCOMPARE(statusCode, response.getStatus());
		}


		void postRequest_data()
		{
			QTest::addColumn<QString>("path");
			QTest::addColumn<HttpResponse>("response");

			QTest::addRow("HTTP_STATUS_OK") << "/foo" << HttpResponse(HTTP_STATUS_OK, "bar");
			QTest::addRow("HTTP_STATUS_NOT_FOUND") << "/foo" << HttpResponse(HTTP_STATUS_NOT_FOUND);
		}


		void postRequest()
		{
			QFETCH(QString, path);
			QFETCH(HttpResponse, response);

			MockHttpServer server;
			server.addMock(path.toLocal8Bit(), response, HTTP_POST);

			HttpServerRequestor requestor;
			const auto& reply = requestor.postRequest(server.getAddress(path), "bar", "application/json");
			QVERIFY(reply->isFinished());
			QCOMPARE(reply->readAll(), response.getBody());
			const auto& statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			QCOMPARE(statusCode, response.getStatus());
		}


		void deleteRequest_data()
		{
			QTest::addColumn<QString>("path");
			QTest::addColumn<HttpResponse>("response");

			QTest::addRow("HTTP_STATUS_OK") << "/foo" << HttpResponse(HTTP_STATUS_OK, "bar");
			QTest::addRow("HTTP_STATUS_NOT_FOUND") << "/foo" << HttpResponse(HTTP_STATUS_NOT_FOUND);
		}


		void deleteRequest()
		{
			QFETCH(QString, path);
			QFETCH(HttpResponse, response);

			MockHttpServer server;
			server.addMock(path.toLocal8Bit(), response, HTTP_DELETE);

			HttpServerRequestor requestor;
			const auto& reply = requestor.deleteRequest(server.getAddress(path));
			QVERIFY(reply->isFinished());
			QCOMPARE(reply->readAll(), response.getBody());
			const auto& statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			QCOMPARE(statusCode, response.getStatus());
		}


};

QTEST_GUILESS_MAIN(test_HttpServerRequestor)
#include "test_HttpServerRequestor.moc"
