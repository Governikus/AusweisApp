/*!
 * \brief Unit tests for \ref HttpResponse
 *
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "HttpResponse.h"

#include <QtTest>

using namespace governikus;


class test_HttpResponse
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void init()
		{
			QCoreApplication::setApplicationVersion("1.2");
		}


		void someHeader()
		{
			HttpResponse response;
			response.setStatus(HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION);
			const auto& msg = response.getMessage();

			QVERIFY(msg.contains("HTTP/1.0 203 Non-Authoritative Information"));
			QVERIFY(msg.contains("Content-Length: 0"));
			QVERIFY(msg.contains("Date: "));
			QVERIFY(msg.contains("Server: Test_network_HttpResponse/1.2 (TR-03124-1/1.3)"));
			QCOMPARE(msg.size(), 158);
		}


		void body()
		{
			HttpResponse response(HTTP_STATUS_OK);
			response.setBody(QByteArray("this is dummy content"), QByteArray("text/plain"));
			const auto& msg = response.getMessage();

			QVERIFY(msg.contains("HTTP/1.0 200 OK"));
			QVERIFY(msg.contains("Content-Length: 21"));
			QVERIFY(msg.contains("Content-Type: text/plain"));
			QVERIFY(msg.contains("Server: Test_network_HttpResponse/1.2 (TR-03124-1/1.3)"));
			QVERIFY(msg.contains("\r\n\r\nthis is dummy content"));
		}


		void ctorNoBody()
		{
			HttpResponse response(HTTP_STATUS_OK);
			QCOMPARE(response.getStatus(), HTTP_STATUS_OK);
			QVERIFY(response.getBody().isEmpty());
			QCOMPARE(response.getHeader("Content-Length"), QByteArray("0"));
			QVERIFY(response.getHeader("Content-Type").isEmpty());
		}


		void ctorNoContentType()
		{
			HttpResponse response(HTTP_STATUS_OK, "hello");
			QCOMPARE(response.getStatus(), HTTP_STATUS_OK);
			QCOMPARE(response.getBody(), QByteArray("hello"));
			QCOMPARE(response.getHeader("Content-Length"), QByteArray("5"));
			QVERIFY(response.getHeader("Content-Type").isEmpty());
		}


		void ctor()
		{
			HttpResponse response(HTTP_STATUS_OK, "hello", "text/plain");
			QCOMPARE(response.getStatus(), HTTP_STATUS_OK);
			QCOMPARE(response.getBody(), QByteArray("hello"));
			QCOMPARE(response.getHeader("Content-Length"), QByteArray("5"));
			QCOMPARE(response.getHeader("Content-Type"), QByteArray("text/plain"));
		}


};

QTEST_GUILESS_MAIN(test_HttpResponse)
#include "test_HttpResponse.moc"
