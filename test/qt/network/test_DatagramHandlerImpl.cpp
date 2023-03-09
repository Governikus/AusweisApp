/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref DatagramHandlerImpl
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
			QCOMPARE(logSpy.count(), 1);
			auto param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Bound on port:"));

			socket.reset();
			QCOMPARE(logSpy.count(), 1);
			param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Shutdown socket"));
		}


		void cannotStart()
		{
			HttpServer::cPort = 80;

#ifdef Q_OS_WIN
			QSKIP("Windows does not block privileged ports");
#elif defined(Q_OS_LINUX)
			if (TestFileHelper::systemAllowsPort(HttpServer::cPort))
			{
				QSKIP("Cannot check, privileged port allowed.");
			}
#endif

#ifdef Q_OS_MACOS
			if (QOperatingSystemVersion::current() >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 10, 14))
			{
				QSKIP("macOS >= 10.14 does not block privileged ports - https://news.ycombinator.com/item?id=18302380");
			}
#endif

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QSharedPointer<DatagramHandler> socket(Env::create<DatagramHandler*>());

			QVERIFY(!socket->isBound());
			QCOMPARE(logSpy.count(), 1);
			auto param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Cannot bind socket: \"The address is protected\""));
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

			QSharedPointer<DatagramHandler> socket(Env::create<DatagramHandler*>());
			QVERIFY(socket->isBound());
			QSignalSpy spySocket(socket.data(), &DatagramHandler::fireNewMessage);

			QUdpSocket clientSocket;
			clientSocket.setProxy(QNetworkProxy::NoProxy);

			QByteArray data(R"({"key":"value"})");
			auto written = clientSocket.writeDatagram(data, broadcast ? QHostAddress::Broadcast : QHostAddress::LocalHost, socket.staticCast<DatagramHandlerImpl>()->mSocket->localPort());
			QTRY_COMPARE(spySocket.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(written, data.size());
			const auto& msg = spySocket.takeFirst();
			QCOMPARE(msg.size(), 2);
			QCOMPARE(msg.at(0).toByteArray(), data);
		}


		void sendDatagram_data()
		{
			QTest::addColumn<bool>("broadcast");

			QTest::newRow("WithBroadcast") << true;
			QTest::newRow("WithoutBroadcast") << false;
		}


		void sendDatagram()
		{
			QFETCH(bool, broadcast);

			QUdpSocket receiver;
			receiver.setProxy(QNetworkProxy::NoProxy);

			QVERIFY(receiver.bind());
			QSignalSpy spyReceiver(&receiver, &QUdpSocket::readyRead);

			QSharedPointer<DatagramHandlerImpl> datagramHandlerImpl = QSharedPointer<DatagramHandlerImpl>::create(false);
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QJsonObject obj;
			obj["test"] = "dummy";
			QJsonDocument doc;
			doc.setObject(obj);
			if (broadcast)
			{
#ifdef Q_OS_FREEBSD
				QSKIP("FreeBSD does not like that");
#endif
				datagramHandlerImpl->sendToAllAddressEntries(doc.toJson(QJsonDocument::Compact), receiver.localPort());
			}
			else
			{
				QVERIFY(datagramHandlerImpl->sendToAddress(doc.toJson(QJsonDocument::Compact), QHostAddress::LocalHost, receiver.localPort()));
			}

			QTRY_COMPARE(spyReceiver.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(logSpy.count(), broadcast ? QNetworkInterface::allInterfaces().size() + 1 : 0);

			QVERIFY(receiver.hasPendingDatagrams());
			QByteArray msg;
			msg.resize(static_cast<int>(receiver.pendingDatagramSize()));
			receiver.readDatagram(msg.data(), msg.size());
			QCOMPARE(msg, QByteArray("{\"test\":\"dummy\"}"));
		}


};

QTEST_GUILESS_MAIN(test_DatagramHandlerImpl)
#include "test_DatagramHandlerImpl.moc"
