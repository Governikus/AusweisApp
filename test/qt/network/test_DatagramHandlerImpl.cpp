/*!
 * \brief Unit tests for \ref DatagramHandlerImpl
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "DatagramHandlerImpl.h"

#include "EnvHolder.h"
#include "LogHandler.h"

#include <QNetworkProxy>
#include <QSharedPointer>
#include <QtTest>

using namespace governikus;


class test_DatagramHandlerImpl
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			DatagramHandler::registerMetaTypes();
			LogHandler::getInstance().init();
		}


		void init()
		{
			DatagramHandlerImpl::cPort = 0;
		}


		void cleanup()
		{
			LogHandler::getInstance().resetBacklog();
		}


		void startUpShutDown()
		{
			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);
			QSharedPointer<DatagramHandler> socket(EnvHolder::create<DatagramHandler>());

			QVERIFY(socket->isBound());
			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Bound on port:"));

			socket.reset();
			QCOMPARE(spy.count(), 1);
			param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Shutdown socket"));
		}


		void cannotStart()
		{
			#ifdef Q_OS_WIN
			QSKIP("Windows does not block privileged ports");
			#endif

			DatagramHandlerImpl::cPort = 80;

			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);
			QSharedPointer<DatagramHandler> socket(EnvHolder::create<DatagramHandler>());

			QVERIFY(!socket->isBound());
			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Cannot bind socket: \"The address is protected\""));
		}


		void getNonJsonDatagram()
		{
			QSharedPointer<DatagramHandler> socket(EnvHolder::create<DatagramHandler>());
			QVERIFY(socket->isBound());
			QSignalSpy spySocket(socket.data(), &DatagramHandler::fireNewMessage);

			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);
			QUdpSocket clientSocket;
			#ifndef QT_NO_NETWORKPROXY
			clientSocket.setProxy(QNetworkProxy::NoProxy);
			#endif

			auto written = clientSocket.writeDatagram("dummy", QHostAddress::LocalHost, socket.staticCast<DatagramHandlerImpl>()->mSocket->localPort());
			spy.wait();
			QCOMPARE(written, 5);

			QCOMPARE(spySocket.count(), 0);
			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Datagram does not contain valid JSON: \"dummy\""));
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

			QSharedPointer<DatagramHandler> socket(EnvHolder::create<DatagramHandler>());
			QVERIFY(socket->isBound());
			QSignalSpy spySocket(socket.data(), &DatagramHandler::fireNewMessage);

			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);
			QUdpSocket clientSocket;
			#ifndef QT_NO_NETWORKPROXY
			clientSocket.setProxy(QNetworkProxy::NoProxy);
			#endif

			QByteArray data("{\"key\":\"value\"}");
			auto written = clientSocket.writeDatagram(data, broadcast ? QHostAddress::Broadcast : QHostAddress::LocalHost, socket.staticCast<DatagramHandlerImpl>()->mSocket->localPort());
			spy.wait();
			QCOMPARE(written, data.size());

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Fire new message"));

			QCOMPARE(spySocket.count(), 1);
			const auto& msg = spySocket.takeFirst();
			QCOMPARE(msg.size(), 2);
			QCOMPARE(msg.at(0).toJsonDocument().toJson(QJsonDocument::Compact), data);
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
			#ifndef QT_NO_NETWORKPROXY
			receiver.setProxy(QNetworkProxy::NoProxy);
			#endif

			QVERIFY(receiver.bind());
			QSignalSpy spyReceiver(&receiver, &QUdpSocket::readyRead);

			QSharedPointer<DatagramHandler> socket(EnvHolder::create<DatagramHandler>());
			DatagramHandlerImpl::cPort = receiver.localPort();
			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);

			QJsonObject obj;
			obj["test"] = "dummy";
			QJsonDocument doc;
			doc.setObject(obj);
			if (broadcast)
			{
				#ifdef Q_OS_FREEBSD
				QSKIP("FreeBSD does not like that");
				#endif
				QVERIFY(socket->send(doc));
			}
			else
			{
				QVERIFY(socket->send(doc, QHostAddress::LocalHost));
			}

			spyReceiver.wait();

			QCOMPARE(spyReceiver.count(), 1);
			QCOMPARE(spy.count(), 0);

			QVERIFY(receiver.hasPendingDatagrams());
			QByteArray msg;
			msg.resize(static_cast<int>(receiver.pendingDatagramSize()));
			receiver.readDatagram(msg.data(), msg.size());
			QCOMPARE(msg, QByteArray("{\"test\":\"dummy\"}"));
		}


};

QTEST_GUILESS_MAIN(test_DatagramHandlerImpl)
#include "test_DatagramHandlerImpl.moc"
