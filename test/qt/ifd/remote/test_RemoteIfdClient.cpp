/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteIfdClient.h"

#include "DatagramHandler.h"
#include "Env.h"
#include "LogHandler.h"
#include "messages/Discovery.h"
#include "messages/IfdEstablishContext.h"

#include <QPointer>
#include <QtTest>

using namespace governikus;


namespace governikus
{

class DatagramHandlerMock
	: public DatagramHandler
{
	Q_OBJECT

	public:
		~DatagramHandlerMock() override;


		[[nodiscard]] bool isBound() const override
		{
			return true;
		}


		void send(const QByteArray&) override
		{
		}


};

DatagramHandlerMock::~DatagramHandlerMock()
{
}


class IfdListMock
	: public IfdList
{
	Q_OBJECT

	public:
		IfdListMock(int pI1, int pI2)
			: IfdList()
		{
			Q_UNUSED(pI1)
			Q_UNUSED(pI2)
		}


		~IfdListMock() override;

		void update(const IfdDescriptor&) override
		{
			Q_EMIT fireDeviceAppeared(QSharedPointer<IfdListEntry>());
		}


		void clear() override
		{
		}


};

IfdListMock::~IfdListMock()
{
}


class RemoteConnectorMock
	: public IfdConnector
{
	Q_OBJECT

	public Q_SLOTS:
		void onConnectRequest(const IfdDescriptor& pRemoteDeviceDescriptor, const QString& pPsk) override;

	Q_SIGNALS:
		void fireConnectionRequestReceived();
};


void RemoteConnectorMock::onConnectRequest(const IfdDescriptor&, const QString&)
{
	Q_EMIT fireConnectionRequestReceived();
}


} // namespace governikus

class test_RemoteIfdClient
	: public QObject
{
	Q_OBJECT

	private:
		QPointer<DatagramHandlerMock> mDatagramHandlerMock;
		QPointer<IfdListMock> mIfdListMock;
		QPointer<RemoteConnectorMock> mRemoteConnectorMock;

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void init()
		{
			Env::setCreator<DatagramHandler*>(std::function<DatagramHandler* ()>([this] {
					mDatagramHandlerMock = new DatagramHandlerMock();
					return mDatagramHandlerMock;
				}));

			Env::setCreator<IfdList*>(std::function<IfdList* ()>([this] {
					mIfdListMock = new IfdListMock(0, 0);
					return mIfdListMock;
				}));

			Env::setCreator<IfdConnector*>(std::function<IfdConnector* ()>([this] {
					mRemoteConnectorMock = new RemoteConnectorMock();
					return mRemoteConnectorMock;
				}));
		}


		void cleanup()
		{
			QVERIFY(Env::getCounter<DatagramHandler*>() <= 2);
			Env::clear();
			QVERIFY(mDatagramHandlerMock.isNull());
			QVERIFY(mIfdListMock.isNull());
			QVERIFY(mRemoteConnectorMock.isNull());
		}


		void testStartStopDetection()
		{
			RemoteIfdClient client;

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


		void testReceiveUnparsable()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray unparsableJson("{\n"
											"    \"device___Name\": \"Sony Xperia Z5 compact\",\n"
											"    \"encrypted\": true,\n"
											"    \"port\": 24728,\n"
											"    \"availableApiLevels\": [1, 2, 3, 4]\n"
											"}");

			RemoteIfdClient client;
			client.startDetection();
			QVERIFY(!mDatagramHandlerMock.isNull());

			Q_EMIT mDatagramHandlerMock->fireNewMessage(unparsableJson, QHostAddress("192.168.1.88"));
			QCOMPARE(logSpy.count(), 6);
			QVERIFY(logSpy.at(5).at(0).toString().contains("Discarding unparsable message"));
		}


		void testReceiveUnexpected()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			RemoteIfdClient client;
			client.startDetection();
			QVERIFY(!mDatagramHandlerMock.isNull());

			const auto& json = IfdEstablishContext(IfdVersion::Version::latest, DeviceInfo::getName()).toByteArray(IfdVersion::Version::latest, QStringLiteral("TestContext"));
			Q_EMIT mDatagramHandlerMock->fireNewMessage(json, QHostAddress("192.168.1.88"));
			QCOMPARE(logSpy.count(), 6);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be REMOTE_IFD"));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Discarding unparsable message"));
		}


		void testReceive_data()
		{
			QTest::addColumn<QString>("hostAddress");

			QTest::newRow("IPv4") << "192.168.1.88";
			QTest::newRow("IPv6 link-global") << "2001:0db8:85a3:08d3:1319:8a2e:0370:7344";
			QTest::newRow("IPv6 link-local") << "fe80::5a38:a519:8ff4:1f1f%enp0s31f6";
		}


		void testReceive()
		{
			QFETCH(QString, hostAddress);

			QString ifdId("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF");
			IfdVersion::Version version = IfdVersion::Version::latest;

			if (IfdVersion::supported().contains(IfdVersion::Version::v2))
			{
				ifdId = R"({
							-----BEGIN CERTIFICATE-----
							MIIC4zCCAcsCBEQvMpowDQYJKoZIhvcNAQELBQAwNDEUMBIGA1UEAwwLQXVzd2Vp
							c0FwcDIxHDAaBgNVBAUTEzY0MTgwMjY3MTE5MTA5MjY2MzQwIhgPMTk3MDAxMDEw
							MDAwMDBaGA85OTk5MTIzMTIzNTk1OVowNDEUMBIGA1UEAwwLQXVzd2Vpc0FwcDIx
							HDAaBgNVBAUTEzY0MTgwMjY3MTE5MTA5MjY2MzQwggEiMA0GCSqGSIb3DQEBAQUA
							A4IBDwAwggEKAoIBAQDGJ9C76Cb8iHuaZJxcFY0NpNllcAK5JKcrigKBki7EvF9z
							5Q/MNek2pxwTMp5SilUDJOkwgcTdm7liC/Zx+lPX8MZjhWxV73DGt9DDyJh91ypl
							6B8vZbpJlL83Vo4C4BLBG6ZaElPpyTitWWKQ4BFUoH0h2utsNFV7FHz+1oZcvhv0
							gQuzd7gQaVV6mzCePRn+4qgxYSXSJ8Ix21udgT3LoHDOBrOWSIt0g/Q1mkzJcnaC
							EnS2s6Ib0xPY5PsL1YN/dZn88/gs9Za4rZSBGIIDrpCt5WCkYbkg45LwXLmaPUrg
							uuFIFIR0HH4pxgajLHpMgYaszxkg4SkdxwJ8vIytAgMBAAEwDQYJKoZIhvcNAQEL
							BQADggEBAB4grwHZ8NMrs3vRInJQc3ftYDCAjDPjjTg/G4BVz07DmlQZpFyPfInb
							UaKfpMlaEd1EoRuNIxC796+rZhy+j97DoLkT1qnPP30GLwlZaVZeOCKIIQ+tGjUU
							cWhhIC6kRCPIQAKxKDNGIUwBAkwludieGa7Ytl7qmnnJDMNe+Ox7Sf+UOa12bJKH
							d27MYoWMfecJdTmF8xXQ7EEYjMwWHd5t5tJG9AVgzhO8zC+iJTqc9I34sIa8+9WE
							oQu+/VZgDkJaSdDJ4LqVFIvUy3CFGh6ahDVsHGC5kTDm5EQWh3puWR0AkIjUWMPi
							xU/nr0Jsab99VgX4/nnCW92v/DIRc1c=
							-----END CERTIFICATE-----
						})";
				version = IfdVersion::Version::v2;
			}
			const QByteArray offerJson = Discovery("Sony Xperia Z5 compact", ifdId, 24728, {version}).toByteArray(version);

			RemoteIfdClient client;
			client.startDetection();
			QVERIFY(!mDatagramHandlerMock.isNull());

			QVERIFY(!mIfdListMock.isNull());
			QSignalSpy spyAppearedList(mIfdListMock.data(), &IfdListMock::fireDeviceAppeared);
			QSignalSpy spyAppeared(&client, &IfdClient::fireDeviceAppeared);

			Q_EMIT mDatagramHandlerMock->fireNewMessage(offerJson, QHostAddress(hostAddress));
			QCOMPARE(spyAppearedList.count(), 1);
			QCOMPARE(spyAppeared.count(), 1);

			QSignalSpy spyVanished(&client, &IfdClient::fireDeviceVanished);
			QCOMPARE(spyVanished.count(), 0);
			Q_EMIT mIfdListMock->fireDeviceVanished(QSharedPointer<IfdListEntry>());
			QCOMPARE(spyVanished.count(), 1);
		}


		void testRemoteConnectorThread()
		{
			QScopedPointer<IfdClient> client(new RemoteIfdClient());
			QCOMPARE(Env::getCounter<IfdConnector*>(), 1);
			QVERIFY(!mRemoteConnectorMock.isNull());
			QVERIFY(client->thread() != mRemoteConnectorMock->thread());

			client.reset();
			QVERIFY(mRemoteConnectorMock.isNull());
		}


		void testRemoteConnectorEstablish()
		{
			const QByteArray offerJson("{\n"
									   "    \"deviceName\": \"Sony Xperia Z5 compact\",\n"
									   "    \"encrypted\": true,\n"
									   "    \"port\": 24728,\n"
									   "    \"availableApiLevels\": [\"IFDInterface_WebSocket_v0\", \"IFDInterface_WebSocket_v2\"]\n"
									   "}");

			RemoteIfdClient client;
			client.startDetection();
			QVERIFY(!mDatagramHandlerMock.isNull());
			Q_EMIT mDatagramHandlerMock->fireNewMessage(offerJson, QHostAddress("192.168.1.88"));

			QVERIFY(!mRemoteConnectorMock.isNull());
			QSignalSpy spyConnectionRequest(mRemoteConnectorMock.data(), &RemoteConnectorMock::fireConnectionRequestReceived);
			const Discovery discovery("", QStringLiteral("0123456789ABCDEF"), 12345, {IfdVersion::Version::latest, IfdVersion::Version::v2});
			const IfdDescriptor descr(discovery, QHostAddress("192.168.1.88"));
			QSharedPointer<IfdListEntry> emptyEntry(new IfdListEntry(descr));
			client.establishConnection(emptyEntry, QString("password1"));

			QTRY_COMPARE(spyConnectionRequest.count(), 1); // clazy:exclude=qstring-allocations

			QSignalSpy spyConnectionDone(&client, &IfdClient::fireEstablishConnectionDone);
			Q_EMIT mRemoteConnectorMock->fireDispatcherError(emptyEntry->getIfdDescriptor(),
					IfdErrorCode::CONNECTION_ERROR);
			QCOMPARE(spyConnectionDone.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_RemoteIfdClient)
#include "test_RemoteIfdClient.moc"
