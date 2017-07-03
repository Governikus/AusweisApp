/*!
 * \brief Unit tests for \ref HttpResponse
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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
			MockSocket* socket = new MockSocket;
			socket->mReadBuffer = QByteArray("GET /favicon.ico HTTP/1.1\r\n"
											 "Host: Dummy.de\r\n"
											 "\r\n\r\n");

			HttpRequest request(socket);
			QVERIFY(!request.isUpgrade());
			QCOMPARE(request.getMethod(), QByteArray("GET"));
			QCOMPARE(request.getHeader().size(), 1);
			QCOMPARE(request.getHeader("host"), QByteArray("Dummy.de"));
			QCOMPARE(request.getUrl(), QUrl("/favicon.ico"));
			QCOMPARE(request.getBody().size(), 0);
		}


		void isUpgrade()
		{
			MockSocket* socket = new MockSocket;
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
			QVERIFY(request.isUpgrade());
			QCOMPARE(request.getMethod(), QByteArray("GET"));
			QCOMPARE(request.getHeader().size(), 7);
			QCOMPARE(request.getHeader("host"), QByteArray("server.example.com"));
			QCOMPARE(request.getHeader("upgrade"), QByteArray("websocket"));
			QCOMPARE(request.getHeader("connection"), QByteArray("Upgrade"));
			QCOMPARE(request.getUrl(), QUrl("/"));
			QCOMPARE(request.getBody().size(), 0);
		}


		void tcTokenURL()
		{
			MockSocket* socket = new MockSocket;
			socket->mReadBuffer = QByteArray("GET /eID-Client?tcTokenURL=https%3A%2F%2Ftest.governikus-eid.de%3A443%2FAutent-DemoApplication%2FRequestServlet%3Fprovider%3Ddemo_epa_20%26redirect%3Dtrue HTTP/1.1\r\n"
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

			HttpRequest request(socket);
			QVERIFY(!request.isUpgrade());
			QCOMPARE(request.getMethod(), QByteArray("GET"));
			QCOMPARE(request.getHeader().size(), 9);
			QCOMPARE(request.getHeader("host"), QByteArray("127.0.0.1:24727"));
			QCOMPARE(request.getUrl(), QUrl("/eID-Client?tcTokenURL=https%3A%2F%2Ftest.governikus-eid.de%3A443%2FAutent-DemoApplication%2FRequestServlet%3Fprovider%3Ddemo_epa_20%26redirect%3Dtrue"));
			QCOMPARE(request.getBody().size(), 0);
		}


};

QTEST_GUILESS_MAIN(test_HttpRequest)
#include "test_HttpRequest.moc"
