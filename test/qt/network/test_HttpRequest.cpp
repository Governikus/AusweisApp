/*!
 * \brief Unit tests for \ref HttpResponse
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "HttpRequest.h"

#include "MockSocket.h"

#include <QtTest>

using namespace governikus;


class test_HttpRequest
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseEmptyBody()
		{
			auto* socket = new MockSocket();
			socket->mReadBuffer = QByteArray("GET /favicon.ico HTTP/1.1\r\n"
											 "Host: Dummy.de\r\n"
											 "\r\n\r\n");

			HttpRequest request(socket);
			request.triggerSocketBuffer();
			QVERIFY(request.mFinished);
			QVERIFY(!request.isUpgrade());
			QCOMPARE(request.getMethod(), QByteArray("GET"));
			QCOMPARE(request.getHttpMethod(), HTTP_GET);
			QCOMPARE(request.getHeader().size(), 1);
			QCOMPARE(request.getHeader("host"), QByteArray("Dummy.de"));
			QCOMPARE(request.getUrl(), QUrl("/favicon.ico"));
			QCOMPARE(request.getBody().size(), 0);
		}


		void isUpgrade()
		{
			auto* socket = new MockSocket();
			socket->mReadBuffer = QByteArray("GET / HTTP/1.1\r\n"
											 "Host: server.example.com\r\n"
											 "Upgrade: websocket\r\n"
											 "Connection: Upgrade\r\n"
											 "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
											 "Origin: http://example.com\r\n"
											 "Sec-WebSocket-Protocol: chat, superchat\r\n"
											 "Sec-WebSocket-Version: 13\r\n"
											 "\r\n\r\n");

			HttpRequest request(socket);
			request.triggerSocketBuffer();
			QVERIFY(request.mFinished);
			QVERIFY(request.isUpgrade());
			QCOMPARE(request.getMethod(), QByteArray("GET"));
			QCOMPARE(request.getHttpMethod(), HTTP_GET);
			QCOMPARE(request.getHeader().size(), 7);
			QCOMPARE(request.getHeader("host"), QByteArray("server.example.com"));
			QCOMPARE(request.getHeader("upgrade"), QByteArray("websocket"));
			QCOMPARE(request.getHeader("connection"), QByteArray("Upgrade"));
			QCOMPARE(request.getUrl(), QUrl("/"));
			QCOMPARE(request.getBody().size(), 0);
		}


		void tcTokenURL()
		{
			const QByteArray data("GET /eID-Client?tcTokenURL=https%3A%2F%2Ftest.governikus-eid.de%3A443%2FAutent-DemoApplication%2FRequestServlet%3Fprovider%3Ddemo_epa_20%26redirect%3Dtrue HTTP/1.1\r\n"
								  "Host: 127.0.0.1:24727\r\n"
								  "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:49.0) Gecko/20100101 Firefox/49.0\r\n"
								  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
								  "Accept-Language: de-DE,de;q=0.8,en-US;q=0.5,en;q=0.3\r\n"
								  "Accept-Encoding: gzip, deflate\r\n"
								  "Cookie: csrftoken=4LcbL9banTtJjpsDRc9du7M4u1rPJZhY; rbsessionid=nevw98w871u0p5ed8cvwfk5yyqh19dh6\r\n"
								  "DNT: 1\r\n"
								  "Connection: keep-alive\r\n"
								  "Upgrade-Insecure-Requests: 1\r\n"
								  "\r\n\r\n");
			auto* socket = new MockSocket();
			socket->mReadBuffer = data;

			HttpRequest request(socket);

			QByteArray receivedData;
			connect(&request, &HttpRequest::fireSocketBuffer, this, [&receivedData] (const QByteArray& pBuffer){
					receivedData = pBuffer;
				});

			request.triggerSocketBuffer();
			QVERIFY(request.mFinished);
			QCOMPARE(receivedData, QByteArray()); // initial parsing does not fill this!
			QVERIFY(!request.isUpgrade());
			QCOMPARE(request.getMethod(), QByteArray("GET"));
			QCOMPARE(request.getHttpMethod(), HTTP_GET);
			QCOMPARE(request.getHeader().size(), 9);
			QCOMPARE(request.getHeader("host"), QByteArray("127.0.0.1:24727"));
			QCOMPARE(request.getUrl(), QUrl("/eID-Client?tcTokenURL=https%3A%2F%2Ftest.governikus-eid.de%3A443%2FAutent-DemoApplication%2FRequestServlet%3Fprovider%3Ddemo_epa_20%26redirect%3Dtrue"));
			QCOMPARE(request.getBody().size(), 0);

			request.triggerSocketBuffer();
			QCOMPARE(receivedData, data);
		}


};

QTEST_GUILESS_MAIN(test_HttpRequest)
#include "test_HttpRequest.moc"
