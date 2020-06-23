/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "messages/Discovery.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_Discovery
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void invalidJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			Discovery msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 6);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be REMOTE_IFD"));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"IFDName\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"IFDID\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"port\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Missing value \"SupportedAPI\""));
		}


		void values()
		{
			const Discovery discovery(
				QStringLiteral("Sony Xperia Z5 compact"),
				QStringLiteral("0123456789ABCDEF"),
				24728,
				{IfdVersion::Version::v0, IfdVersion::Version::v_test}
				);

			QVERIFY(!discovery.isIncomplete());
			QCOMPARE(discovery.getType(), RemoteCardMessageType::UNDEFINED);
			QCOMPARE(discovery.getContextHandle(), QString());
			QCOMPARE(discovery.getIfdName(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(discovery.getIfdId(), QStringLiteral("0123456789ABCDEF"));
			QVERIFY(discovery.getPort() == static_cast<quint16>(24728));
			QCOMPARE(discovery.getSupportedApis(), QVector<IfdVersion::Version>({IfdVersion::Version::v0, IfdVersion::Version::v_test}));
		}


		void toJson()
		{
			const Discovery discovery(
				QStringLiteral("Sony Xperia Z5 compact"),
				QStringLiteral("0123456789ABCDEF"),
				24728,
				{IfdVersion::Version::v0, IfdVersion::Version::v_test}
				);

			const QByteArray& byteArray = discovery.toByteArray();
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"IFDID\": \"0123456789ABCDEF\",\n"
							   "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
							   "    \"SupportedAPI\": [\n"
							   "        \"IFDInterface_WebSocket_v0\",\n"
							   "        \"IFDInterface_WebSocket_v_test\"\n"
							   "    ],\n"
							   "    \"msg\": \"REMOTE_IFD\",\n"
							   "    \"port\": 24728\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 5);
			QCOMPARE(obj.value(QLatin1String("IFDName")).toString(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(obj.value(QLatin1String("IFDID")).toString(), QStringLiteral("0123456789ABCDEF"));
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("REMOTE_IFD"));
			QCOMPARE(obj.value(QLatin1String("port")).toInt(), 24728);
			const QJsonValue apiLevels = obj.value(QLatin1String("SupportedAPI"));
			QVERIFY(apiLevels.isArray());
			QCOMPARE(apiLevels.toArray().size(), 2);
			QCOMPARE(apiLevels.toArray().at(0).toString(), QStringLiteral("IFDInterface_WebSocket_v0"));
			QCOMPARE(apiLevels.toArray().at(1).toString(), QStringLiteral("IFDInterface_WebSocket_v_test"));
		}


		void fromJson()
		{
			const QByteArray message("{\n"
									 "    \"IFDID\": \"0123456789ABCDEF\",\n"
									 "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
									 "    \"SupportedAPI\": [\n"
									 "        \"IFDInterface_WebSocket_v0\",\n"
									 "        \"IFDInterface_WebSocket_v_test\"\n"
									 "    ],\n"
									 "    \"msg\": \"REMOTE_IFD\",\n"
									 "    \"port\": 24728\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(!discovery.isIncomplete());
			QCOMPARE(discovery.getType(), RemoteCardMessageType::UNDEFINED);
			QCOMPARE(discovery.getContextHandle(), QString());
			QCOMPARE(discovery.getIfdName(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(discovery.getIfdId(), QStringLiteral("0123456789ABCDEF"));
			QVERIFY(discovery.getPort() == static_cast<quint16>(24728));
			QCOMPARE(discovery.getSupportedApis(), QVector<IfdVersion::Version>({IfdVersion::Version::v0, IfdVersion::Version::v_test}));
		}


		void msgField_data()
		{
			QTest::addColumn<QByteArray>("type");

			const auto& msgTypes = Enum<RemoteCardMessageType>::getList();
			for (const auto& type : msgTypes)
			{
				QTest::newRow(getEnumName(type).data()) << QString(getEnumName(type)).toUtf8();
			}
		}


		void msgField()
		{
			QFETCH(QByteArray, type);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			QByteArray message("{\n"
							   "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
							   "    \"IFDID\": \"0123456789ABCDEF\",\n"
							   "    \"port\": 24728,\n"
							   "    \"SupportedAPI\": [\"IFDInterface_WebSocket_v0\", \"IFDInterface_WebSocket_v_test\"],\n"
							   "    \"msg\": \"%1\"\n"
							   "}");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", type)).object();
			const Discovery discovery(obj);

			QVERIFY(discovery.isIncomplete());
			QCOMPARE(discovery.getType(), RemoteCardMessageType::UNDEFINED);

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be REMOTE_IFD"));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Received ifdId is not a fingerprint. Interpretion as a certificate to create our own fingerprint failed"));
		}


		void ignoreContext()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"IFDID\": \"0123456789ABCDEF\",\n"
									 "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
									 "    \"SupportedAPI\": [\n"
									 "        \"IFDInterface_WebSocket_v0\",\n"
									 "        \"IFDInterface_WebSocket_v_test\"\n"
									 "    ],\n"
									 "    \"msg\": \"REMOTE_IFD\",\n"
									 "    \"port\": 24728\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(!discovery.isIncomplete());
			QCOMPARE(discovery.getContextHandle(), QString());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Received ifdId is not a fingerprint. Interpretion as a certificate to create our own fingerprint failed"));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"IFDID\": 1,\n"
									 "    \"IFDName\": 2,\n"
									 "    \"SupportedAPI\": \"IFDInterface_WebSocket_v0\",\n"
									 "    \"msg\": \"REMOTE_IFD\",\n"
									 "    \"port\": \"3\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(discovery.isIncomplete());
			QCOMPARE(discovery.getIfdName(), QString());
			QCOMPARE(discovery.getIfdId(), QString());
			QVERIFY(discovery.getPort() == 0);
			QCOMPARE(discovery.getSupportedApis(), QVector<IfdVersion::Version>());

			QCOMPARE(logSpy.count(), 4);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"IFDName\" should be of type \"string\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"IFDID\" should be of type \"string\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("The value of \"port\" should be of type \"number\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("The value of \"SupportedAPI\" should be of type \"array\""));
		}


		void wrongApiType()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"IFDID\": \"0123456789ABCDEF\",\n"
									 "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
									 "    \"SupportedAPI\": [\n"
									 "        0,\n"
									 "        \"IFDInterface_WebSocket_v_test\"\n"
									 "    ],\n"
									 "    \"msg\": \"REMOTE_IFD\",\n"
									 "    \"port\": 24728\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(discovery.isIncomplete());
			QCOMPARE(discovery.getSupportedApis(), QVector<IfdVersion::Version>({IfdVersion::Version::v_test}));

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Received ifdId is not a fingerprint. Interpretion as a certificate to create our own fingerprint failed"));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"SupportedAPI\" should be of type \"string array\""));
		}


		void sameIfdId()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray messageHash(R"({
											"IFDID": "0575e99867361c26442ece18bed6f955ab7dd269ae8f42d3a21af0e734c3d8d9",
											"IFDName": "Sony Xperia Z5 compact",
											"SupportedAPI": ["IFDInterface_WebSocket_v0", "IFDInterface_WebSocket_v_test"],
											"msg": "REMOTE_IFD",
											"port": 24728
										 })");
			const QJsonObject& objHash = QJsonDocument::fromJson(messageHash).object();
			const Discovery discoveryHash(objHash);
			QCOMPARE(logSpy.count(), 0);

			const QByteArray messageCert(R"({
											"IFDID": "-----BEGIN CERTIFICATE-----
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
												-----END CERTIFICATE-----",
											"IFDName": "Sony Xperia Z5 compact",
											"SupportedAPI": ["IFDInterface_WebSocket_v0", "IFDInterface_WebSocket_v_test"],
											"msg": "REMOTE_IFD",
											"port": 24728
										 })");
			const QJsonObject& objCert = QJsonDocument::fromJson(messageCert).object();
			const Discovery discoveryCert(objCert);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Received ifdId is not a fingerprint. Interpretion as a certificate to create our own fingerprint succeeded"));

			QCOMPARE(discoveryHash.getIfdId(), discoveryCert.getIfdId());
		}


};

QTEST_GUILESS_MAIN(test_Discovery)
#include "test_Discovery.moc"
