/*!
 * \brief Unit tests for \ref HttpResponse
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "HttpServer.h"

#include "Env.h"
#include "LogHandler.h"

#include "TestFileHelper.h"

#include <QMetaType>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QtTest>

using namespace governikus;


class test_HttpServer
	: public QObject
{
	Q_OBJECT

	private:
		QNetworkAccessManager mAccessManager;

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void init()
		{
			HttpServer::cPort = 0;
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void startUpShutDown()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			auto server = Env::getShared<HttpServer>();

			QVERIFY(server->isListening());
			QCOMPARE(logSpy.count(), 2);

			auto param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Spawn shared instance: governikus::HttpServer"));
			param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Listening on port:"));

			server.reset();
			QCOMPARE(logSpy.count(), 1);
			param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Shutdown server"));
		}


		void cannotStart()
		{
			HttpServer::cPort = 80;

			#ifdef Q_OS_WIN
			QSKIP("Windows does not block privileged ports");
			#elif defined(Q_OS_LINUX)
			const auto portStart = TestFileHelper::getUnprivilegedPortStart();
			QVERIFY(portStart != -1);
			if (portStart <= HttpServer::cPort)
			{
				QSKIP("Cannot check privileged port");
			}
			#endif

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			HttpServer server;

			QVERIFY(!server.isListening());
			QCOMPARE(logSpy.count(), 1);
			auto param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Cannot start server: \"The address is protected\""));
		}


		void sendRequest()
		{
			HttpServer server;
			QVERIFY(server.isListening());
			QSignalSpy spyServer(&server, &HttpServer::fireNewHttpRequest);

			auto url = QUrl("http://127.0.0.1:" + QString::number(server.getServerPort()) + "/eID-Client?tcTokenURL=https%3A%2F%2Fdummy.de");
			auto reply = mAccessManager.get(QNetworkRequest(url));
			QSignalSpy spyClient(reply, &QNetworkReply::finished);

			QTRY_COMPARE(spyServer.count(), 1); // clazy:exclude=qstring-allocations
			auto param = spyServer.takeFirst();
			auto httpRequest = qvariant_cast<QSharedPointer<HttpRequest> >(param.at(0));
			QCOMPARE(httpRequest->getMethod(), QByteArray("GET"));
			QCOMPARE(httpRequest->getUrl(), QUrl("/eID-Client?tcTokenURL=https%3A%2F%2Fdummy.de"));

			QVERIFY(httpRequest->send(HTTP_STATUS_NOT_FOUND));

			QTRY_COMPARE(spyClient.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(reply->error(), QNetworkReply::ContentNotFoundError);
			QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 404);
		}


		void websocketUpgrade()
		{
			HttpServer server;
			QVERIFY(server.isListening());
			QSignalSpy spyServer(&server, &HttpServer::fireNewWebSocketRequest);

			QNetworkRequest request(QUrl("http://127.0.0.1:" + QString::number(server.getServerPort())));
			request.setRawHeader("upgrade", "websocket");
			request.setRawHeader("connection", "upgrade");
			mAccessManager.get(request);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QTRY_COMPARE(spyServer.count(), 1); // clazy:exclude=qstring-allocations
			auto param = spyServer.takeFirst();
			auto socket = qvariant_cast<QSharedPointer<HttpRequest> >(param.at(0))->take();
			QVERIFY(socket->bytesAvailable() > 0); // check rollbackTransaction
			const auto& requestData = socket->readAll();
			QVERIFY(requestData.contains("GET / HTTP/1.1"));
			QVERIFY(requestData.contains("connection: upgrade"));
			QVERIFY(requestData.contains("upgrade: websocket"));
			QVERIFY(requestData.contains("\r\n\r\n"));

			param = logSpy.takeLast();
			QVERIFY(param.at(0).toString().contains("Upgrade to websocket requested"));
		}


		void unknownUpgrade()
		{
			HttpServer server;
			QVERIFY(server.isListening());

			QNetworkRequest request(QUrl("http://127.0.0.1:" + QString::number(server.getServerPort())));
			request.setRawHeader("upgrade", "unknown");
			request.setRawHeader("connection", "upgrade");
			auto reply = mAccessManager.get(request);
			QSignalSpy spyClient(reply, &QNetworkReply::finished);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QTRY_COMPARE(spyClient.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(reply->error(), QNetworkReply::ContentNotFoundError);
			QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 404);

			auto param = logSpy.takeLast();
			QVERIFY(param.at(0).toString().contains("Unknown upgrade requested"));
		}


		void methodWithoutReceiver_data()
		{
			QTest::addColumn<QNetworkRequest>("request");
			QTest::addColumn<QString>("signal");

			QNetworkRequest request;

			QTest::newRow("http") << request << QStringLiteral("fireNewHttpRequest");

			request.setRawHeader("upgrade", "websocket");
			request.setRawHeader("connection", "upgrade");
			QTest::newRow("ws") << request << QStringLiteral("fireNewWebSocketRequest");
		}


		void methodWithoutReceiver()
		{
			QFETCH(QNetworkRequest, request);
			QFETCH(QString, signal);

			HttpServer server;
			QVERIFY(server.isListening());
			request.setUrl(QUrl("http://127.0.0.1:" + QString::number(server.getServerPort())));

			auto reply = mAccessManager.get(request);

			QSignalSpy spyClient(reply, &QNetworkReply::finished);
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QTRY_COMPARE(spyClient.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 503);

			auto noSignalFound = QStringLiteral("No registration found: \"%1\"").arg(signal);
			QVERIFY(logSpy.takeLast().at(0).toString().contains(noSignalFound));
		}


};

QTEST_GUILESS_MAIN(test_HttpServer)
#include "test_HttpServer.moc"
