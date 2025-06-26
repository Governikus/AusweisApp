/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "DatagramHandlerImpl.h"

#include "Env.h"
#include "HttpServer.h"
#include "LogHandler.h"

#include "TestFileHelper.h"

#include <QNetworkInterface>
#include <QNetworkProxy>
#include <QSharedPointer>
#include <QtTest>
#ifdef Q_OS_MACOS
	#include <QOperatingSystemVersion>
#endif


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_DatagramHandlerImpl
	: public QObject
{
	Q_OBJECT

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
			QSharedPointer<DatagramHandler> socket(Env::create<DatagramHandler*>());

			QVERIFY(socket->isBound());
			QVERIFY(!logSpy.isEmpty());
			auto param = logSpy.takeLast();
			QVERIFY(param.at(0).toString().contains("Bound on port:"_L1));

			socket.reset();
			QVERIFY(!logSpy.isEmpty());
			param = logSpy.takeLast();
			QVERIFY(param.at(0).toString().contains("Shutdown socket"_L1));
		}


		void cannotStart()
		{
			HttpServer::cPort = 80;

#ifdef Q_OS_WIN
			QSKIP("Windows does not block privileged ports");
#elif defined(Q_OS_MACOS)
			QSKIP("macOS >= 10.14 does not block privileged ports - https://news.ycombinator.com/item?id=18302380");
#elif defined(Q_OS_LINUX)
			if (TestFileHelper::systemAllowsPort(HttpServer::cPort))
			{
				QSKIP("Cannot check, privileged port allowed.");
			}
#endif

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QSharedPointer<DatagramHandler> socket(Env::create<DatagramHandler*>());

			QVERIFY(!socket->isBound());
			QCOMPARE(logSpy.count(), 1);
			auto param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Cannot bind socket: \"The address is protected\""_L1));
		}


		void getNonJsonDatagram()
		{
			QSharedPointer<DatagramHandler> socket(Env::create<DatagramHandler*>());
			QVERIFY(socket->isBound());
			QSignalSpy spySocket(socket.data(), &DatagramHandler::fireNewMessage);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QUdpSocket clientSocket;
			clientSocket.setProxy(QNetworkProxy::NoProxy);

			auto written = clientSocket.writeDatagram("dummy", QHostAddress::LocalHost, socket.staticCast<DatagramHandlerImpl>()->mSocket->localPort());
			QTRY_COMPARE(spySocket.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(written, 5);
			QCOMPARE(logSpy.count(), 0);
			auto param = spySocket.takeFirst();
			QCOMPARE(param.at(0).toByteArray(), QByteArray("dummy"));
		}


		void getJsonDatagram_data()
		{
			QTest::addColumn<bool>("broadcast");

			QTest::newRow("WithBroadcast") << true;
			QTest::newRow("WithoutBroadcast") << false;
		}


		void getJsonDatagram()
		{
			QFETCH(bool, broadcast);

#ifdef Q_OS_FREEBSD
			if (broadcast)
			{
				QSKIP("FreeBSD does not like that");
			}
#endif

			DatagramHandlerImpl socket;
			QVERIFY(socket.isBound());
			QSignalSpy spySocket(&socket, &DatagramHandler::fireNewMessage);

			QUdpSocket clientSocket;
			clientSocket.setProxy(QNetworkProxy::NoProxy);

			QList<QHostAddress> addresses;
			if (broadcast)
			{
				const auto entries = socket.getAllBroadcastEntries();
				for (const auto& entry : entries)
				{
					const auto& addr = socket.getBroadcastAddress(entry);
					if (!addr.isNull())
					{
						addresses << addr;
					}
				}
			}
			else
			{
				addresses << QHostAddress::LocalHost;
			}

			for (const auto& address : std::as_const(addresses))
			{
				QByteArray data(R"({"key":"value"})");
				const auto written = clientSocket.writeDatagram(data, address, socket.mSocket->localPort());
				if (written == -1)
				{
					qCritical() << address << clientSocket.error() << clientSocket.errorString();
				}
				QTRY_COMPARE(spySocket.count(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(written, data.size());
				const auto& msg = spySocket.takeFirst();
				QCOMPARE(msg.size(), 2);
				QCOMPARE(msg.at(0).toByteArray(), data);
			}
		}


		void sendDatagram_data()
		{
			QTest::addColumn<bool>("broadcast");
			QTest::addColumn<bool>("duplicate");

			QTest::newRow("WithBroadcast - Unique") << true << false;
			QTest::newRow("WithBroadcast - Duplicate") << true << true;
			QTest::newRow("WithoutBroadcast") << false << false;
		}


		void sendDatagram()
		{
			QFETCH(bool, broadcast);
			QFETCH(bool, duplicate);

			QUdpSocket receiver;
			receiver.setProxy(QNetworkProxy::NoProxy);

			QVERIFY(receiver.bind());
			QSignalSpy spyReceiver(&receiver, &QUdpSocket::readyRead);

			QSharedPointer<DatagramHandlerImpl> datagramHandlerImpl = QSharedPointer<DatagramHandlerImpl>::create(false);
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QJsonObject obj;
			obj["test"_L1] = "dummy"_L1;
			QJsonDocument doc;
			doc.setObject(obj);
			if (broadcast)
			{
#ifdef Q_OS_FREEBSD
				QSKIP("FreeBSD does not like that");
#endif
				auto entries = datagramHandlerImpl->getAllBroadcastEntries();
				if (duplicate)
				{
					entries << entries.last();
				}
				datagramHandlerImpl->sendToAddressEntries(doc.toJson(QJsonDocument::Compact), entries, receiver.localPort());
			}
			else
			{
				QVERIFY(datagramHandlerImpl->sendToAddress(doc.toJson(QJsonDocument::Compact), QHostAddress::LocalHost, receiver.localPort()));
			}

			QTRY_COMPARE(spyReceiver.count(), 1); // clazy:exclude=qstring-allocations
			if (broadcast)
			{
				QVERIFY(logSpy.count() >= (duplicate ? 4 : 2));
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Broadcast Addresses changed...")));
				if (duplicate)
				{
					QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Skipping duplicate broadcasting to")));
				}
			}
			else
			{
				QVERIFY(logSpy.isEmpty());
			}

			QVERIFY(receiver.hasPendingDatagrams());
			QByteArray msg;
			msg.resize(static_cast<int>(receiver.pendingDatagramSize()));
			receiver.readDatagram(msg.data(), msg.size());
			QCOMPARE(msg, QByteArray("{\"test\":\"dummy\"}"));
		}


};

QTEST_GUILESS_MAIN(test_DatagramHandlerImpl)
#include "test_DatagramHandlerImpl.moc"
