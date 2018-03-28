/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteClientImpl.h"

#include "DatagramHandler.h"
#include "Env.h"
#include "LogHandler.h"
#include "messages/Discovery.h"
#include "messages/IfdEstablishContext.h"

#include <QPointer>
#include <QtTest/QtTest>


using namespace governikus;


namespace governikus
{

class DatagramHandlerMock
	: public DatagramHandler
{
	Q_OBJECT

	public:
		virtual ~DatagramHandlerMock() override;


		virtual bool isBound() const override
		{
			return true;
		}


		virtual bool send(const QJsonDocument&) override
		{
			return true;
		}


};

DatagramHandlerMock::~DatagramHandlerMock()
{
}


class RemoteDeviceListMock
	: public RemoteDeviceList
{
	Q_OBJECT

	public:
		RemoteDeviceListMock(int pI1, int pI2)
			: RemoteDeviceList(pI1, pI2)
		{

		}


		virtual ~RemoteDeviceListMock() override;

		virtual void update(const RemoteDeviceDescriptor&) override
		{
			Q_EMIT fireDeviceAppeared(QSharedPointer<RemoteDeviceListEntry>());
		}


		virtual void clear() override
		{

		}


};

RemoteDeviceListMock::~RemoteDeviceListMock()
{

}


class RemoteConnectorMock
	: public RemoteConnector
{
	Q_OBJECT

	public Q_SLOTS:
		virtual void onConnectRequest(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const QString& pPsk) override;

	Q_SIGNALS:
		void fireConnectionRequestReceived();
};


void RemoteConnectorMock::onConnectRequest(const RemoteDeviceDescriptor&, const QString&)
{
	Q_EMIT fireConnectionRequestReceived();
}


}

class test_RemoteClient
	: public QObject
{
	Q_OBJECT

	private:
		QPointer<DatagramHandlerMock> mDatagramHandlerMock;
		QPointer<RemoteDeviceListMock> mRemoteDeviceListMock;
		QPointer<RemoteConnectorMock> mRemoteConnectorMock;

	private Q_SLOTS:
		void initTestCase()
		{
			LogHandler::getInstance().init();
		}


		void init()
		{
			Env::setCreator<DatagramHandler*>(std::function<DatagramHandler*()>([&] {
						mDatagramHandlerMock = new DatagramHandlerMock;
						return mDatagramHandlerMock;
					}));

			Env::setCreator<RemoteDeviceList*>(std::function<RemoteDeviceList*()>([&] {
						mRemoteDeviceListMock = new RemoteDeviceListMock(0, 0);
						return mRemoteDeviceListMock;
					}));

			Env::setCreator<RemoteConnector*>(std::function<RemoteConnector*()>([&] {
						mRemoteConnectorMock = new RemoteConnectorMock;
						return mRemoteConnectorMock;
					}));
		}


		void cleanup()
		{
			QVERIFY(Env::getCounter<DatagramHandler*>() <= 2);
			Env::clear();
			QVERIFY(mDatagramHandlerMock.isNull());
			QVERIFY(mRemoteDeviceListMock.isNull());
			QVERIFY(mRemoteConnectorMock.isNull());
		}


		void testStartStopDetection()
		{
			RemoteClientImpl client;

			QVERIFY(mDatagramHandlerMock.isNull());

			client.startDetection();
			QCOMPARE(Env::getCounter<DatagramHandler*>(), 1);
			QVERIFY(!mDatagramHandlerMock.isNull());

			client.stopDetection();
			QVERIFY(mDatagramHandlerMock.isNull());

			client.startDetection();
			QCOMPARE(Env::getCounter<DatagramHandler*>(), 2);
			QVERIFY(!mDatagramHandlerMock.isNull());

			client.stopDetection();
			QVERIFY(mDatagramHandlerMock.isNull());
		}


		void testReceiveIPv6()
		{
			QSignalSpy logSpy(&LogHandler::getInstance(), &LogHandler::fireLog);

			const auto& offerJson = QJsonDocument::fromJson("{\n"
															"    \"deviceName\": \"Sony Xperia Z5 compact\",\n"
															"    \"encrypted\": true,\n"
															"    \"port\": 24728,\n"
															"    \"availableApiLevels\": [1, 2, 3, 4]\n"
															"}");

			RemoteClientImpl client;
			QSignalSpy spyAppeared(&client, &RemoteClient::fireDeviceAppeared);
			client.startDetection();
			Q_EMIT mDatagramHandlerMock->fireNewMessage(offerJson, QHostAddress("2001:0db8:85a3:08d3:1319:8a2e:0370:7344"));

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("IPv6 not supported"));
			QVERIFY(spyAppeared.isEmpty());
		}


		void testReceiveUnparsable()
		{
			QSignalSpy logSpy(&LogHandler::getInstance(), &LogHandler::fireLog);

			const auto& unparsableJson = QJsonDocument::fromJson("{\n"
																 "    \"device___Name\": \"Sony Xperia Z5 compact\",\n"
																 "    \"encrypted\": true,\n"
																 "    \"port\": 24728,\n"
																 "    \"availableApiLevels\": [1, 2, 3, 4]\n"
																 "}");

			RemoteClientImpl client;
			client.startDetection();
			QVERIFY(!mDatagramHandlerMock.isNull());

			Q_EMIT mDatagramHandlerMock->fireNewMessage(unparsableJson, QHostAddress("192.168.1.88"));
			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(1).at(0).toString().contains("Discarding unparsable message"));
		}


		void testReceiveUnexpected()
		{
			QSignalSpy logSpy(&LogHandler::getInstance(), &LogHandler::fireLog);

			RemoteClientImpl client;
			client.startDetection();
			QVERIFY(!mDatagramHandlerMock.isNull());

			Q_EMIT mDatagramHandlerMock->fireNewMessage(IfdEstablishContext(QStringLiteral("IFDInterface_WebSocket_v0"), DeviceInfo::getName()).toJson(QStringLiteral("TestContext")), QHostAddress("192.168.1.88"));
			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"IFDName\" should be of type string"));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Discarding unparsable message"));
		}


		void testReceive()
		{
			const auto& offerJson = QJsonDocument::fromJson("{\n"
															"    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
															"    \"IFDID\": \"0123456789ABCDEF\",\n"
															"    \"encrypted\": true,\n"
															"    \"msg\": \"REMOTE_IFD\",\n"
															"    \"port\": 24728,\n"
															"    \"SupportedAPI\": [\"IFDInterface_WebSocket_v0\", \"IFDInterface_WebSocket_v2\"]\n"
															"}");

			RemoteClientImpl client;
			client.startDetection();
			QVERIFY(!mDatagramHandlerMock.isNull());

			QVERIFY(!mRemoteDeviceListMock.isNull());
			QSignalSpy spyAppearedList(mRemoteDeviceListMock.data(), &RemoteDeviceListMock::fireDeviceAppeared);
			QSignalSpy spyAppeared(&client, &RemoteClient::fireDeviceAppeared);

			Q_EMIT mDatagramHandlerMock->fireNewMessage(offerJson, QHostAddress("192.168.1.88"));
			QCOMPARE(spyAppearedList.count(), 1);
			QCOMPARE(spyAppeared.count(), 1);

			QSignalSpy spyVanished(&client, &RemoteClient::fireDeviceVanished);
			QCOMPARE(spyVanished.count(), 0);
			Q_EMIT mRemoteDeviceListMock->fireDeviceVanished(QSharedPointer<RemoteDeviceListEntry>());
			QCOMPARE(spyVanished.count(), 1);
		}


		void testRemoteConnectorThread()
		{
			QScopedPointer<RemoteClient> client(new RemoteClientImpl);
			QCOMPARE(Env::getCounter<RemoteConnector*>(), 1);
			QVERIFY(!mRemoteConnectorMock.isNull());
			QVERIFY(client->thread() != mRemoteConnectorMock->thread());

			client.reset();
			QVERIFY(mRemoteConnectorMock.isNull());
		}


		void testRemoteConnectorEstablish()
		{
			const auto& offerJson = QJsonDocument::fromJson("{\n"
															"    \"deviceName\": \"Sony Xperia Z5 compact\",\n"
															"    \"encrypted\": true,\n"
															"    \"port\": 24728,\n"
															"    \"availableApiLevels\": [\"IFDInterface_WebSocket_v0\", \"IFDInterface_WebSocket_v2\"]\n"
															"}");

			RemoteClientImpl client;
			client.startDetection();
			QVERIFY(!mDatagramHandlerMock.isNull());
			Q_EMIT mDatagramHandlerMock->fireNewMessage(offerJson, QHostAddress("192.168.1.88"));

			QVERIFY(!mRemoteConnectorMock.isNull());
			QSignalSpy spyConnectionRequest(mRemoteConnectorMock.data(), &RemoteConnectorMock::fireConnectionRequestReceived);
			const QSharedPointer<Discovery> msg(new Discovery("", QStringLiteral("0123456789ABCDEF"), 12345, {IfdVersion::Version::v0}));
			const RemoteDeviceDescriptor descr(msg, QHostAddress("192.168.1.88"));
			QSharedPointer<RemoteDeviceListEntry> emptyEntry(new RemoteDeviceListEntry(descr));
			client.establishConnection(emptyEntry, QString("password1"));

			spyConnectionRequest.wait();
			QCOMPARE(spyConnectionRequest.count(), 1);

			QSignalSpy spyConnectionDone(&client, &RemoteClient::fireEstablishConnectionDone);
			Q_EMIT mRemoteConnectorMock->fireRemoteDispatcherError(emptyEntry->getRemoteDeviceDescriptor(), RemoteErrorCode::CONNECTION_ERROR);
			QCOMPARE(spyConnectionDone.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_RemoteClient)
#include "test_RemoteClientImpl.moc"
