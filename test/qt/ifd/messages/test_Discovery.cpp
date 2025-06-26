/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "messages/Discovery.h"

#include "LogHandler.h"
#include "TestFileHelper.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(IfdVersion::Version)


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
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			Discovery msg(obj);
			QVERIFY(msg.isIncomplete());
			QCOMPARE(msg.getType(), IfdMessageType::UNDEFINED);
			QCOMPARE(msg.getContextHandle(), QString());
			QCOMPARE(msg.getIfdName(), QString());
			QCOMPARE(msg.getIfdId(), QByteArray());
			QVERIFY(msg.getPort() == 0);
			QCOMPARE(msg.getSupportedApis(), QList<IfdVersion::Version>());
			QCOMPARE(msg.isPairing(), false);

			QCOMPARE(logSpy.count(), 6);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"msg\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be REMOTE_IFD")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"IFDName\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"IFDID\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"port\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"SupportedAPI\"")));
		}


		void values()
		{
			Discovery discovery(
				QStringLiteral("Sony Xperia Z5 compact"),
				QByteArrayLiteral("0123456789ABCDEF"),
				24728,
				{IfdVersion::Version::v0, IfdVersion::Version::v2}
				);
			discovery.setAddresses({QHostAddress("192.168.1.42"_L1)});

			QVERIFY(!discovery.isIncomplete());
			QCOMPARE(discovery.getType(), IfdMessageType::UNDEFINED);
			QCOMPARE(discovery.getContextHandle(), QString());
			QCOMPARE(discovery.getIfdName(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(discovery.getIfdId(), QByteArrayLiteral("0123456789ABCDEF"));
			QVERIFY(discovery.getPort() == static_cast<quint16>(24728));
			QCOMPARE(discovery.getSupportedApis(), QList<IfdVersion::Version>({IfdVersion::Version::v0, IfdVersion::Version::v2}));
			QVERIFY(!discovery.isPairing());
			QCOMPARE(discovery.getAddresses(), {QUrl("wss://192.168.1.42:24728"_L1)});

			discovery.setPairing(true);
			QVERIFY(discovery.isPairing());
		}


		void toJson_data()
		{
			QList<QHostAddress> ipv4({QHostAddress("192.168.1.42"_L1)});
			QList<QHostAddress> ipv6({QHostAddress("::ffff:192.168.1.42"_L1)});
			QList<QHostAddress> ipv46({QHostAddress("192.168.1.42"_L1), QHostAddress("::ffff:192.168.1.42"_L1)});

			QByteArray ipv4_plain("wss://192.168.1.42:24728");
			QByteArray ipv6_plain("wss://[::ffff:192.168.1.42]:24728");
			QByteArray ipv46_plain("wss://192.168.1.42:24728\",\n        \"wss://[::ffff:192.168.1.42]:24728");

			QTest::addColumn<IfdVersion::Version>("version");
			QTest::addColumn<bool>("pairing");
			QTest::addColumn<QByteArray>("json_pairing");
			QTest::addColumn<QList<QHostAddress>>("hostAddresses");
			QTest::addColumn<QByteArray>("json_address");
			QTest::newRow("Unknown - Pairing enabled - IPv4") << IfdVersion::Version::Unknown
															  << true << QByteArray()
															  << ipv4 << ipv4_plain;
			QTest::newRow("Unknown - Pairing disabled - IPv6") << IfdVersion::Version::Unknown
															   << false << QByteArray()
															   << ipv6 << ipv6_plain;
			QTest::newRow("Unknown - Pairing disabled - IPv4/6") << IfdVersion::Version::Unknown
																 << false << QByteArray()
																 << ipv46 << ipv46_plain;
			QTest::newRow("v0 - Pairing enabled - IPv4") << IfdVersion::Version::v0
														 << true << QByteArray()
														 << ipv4 << ipv4_plain;
			QTest::newRow("v0 - Pairing disabled - IPv6") << IfdVersion::Version::v0
														  << false << QByteArray()
														  << ipv6 << ipv6_plain;
			QTest::newRow("v0 - Pairing disabled - IPv4/6") << IfdVersion::Version::v0
															<< false << QByteArray()
															<< ipv46 << ipv46_plain;
			QTest::newRow("v2 - Pairing enabled - IPv6") << IfdVersion::Version::v2
														 << true << QByteArray("    \"pairing\": true,\n")
														 << ipv4 << ipv4_plain;
			QTest::newRow("v2 - Pairing disabled - IPv6") << IfdVersion::Version::v2
														  << false << QByteArray("    \"pairing\": false,\n")
														  << ipv6 << ipv6_plain;
			QTest::newRow("v2 - Pairing disabled - IPv4/6") << IfdVersion::Version::v2
															<< false << QByteArray("    \"pairing\": false,\n")
															<< ipv46 << ipv46_plain;
		}


		void toJson()
		{
			QFETCH(IfdVersion::Version, version);
			QFETCH(bool, pairing);
			QFETCH(QByteArray, json_pairing);
			QFETCH(QList<QHostAddress>, hostAddresses);
			QFETCH(QByteArray, json_address);

			Discovery discovery(
				QStringLiteral("Sony Xperia Z5 compact"),
				QByteArray::fromHex(QByteArrayLiteral("0123456789ABCDEF")),
				24728,
				{IfdVersion::Version::v0, IfdVersion::Version::v2}
				);
			discovery.setPairing(pairing);
			discovery.setAddresses(hostAddresses);

			const QByteArray& byteArray = discovery.toByteArray(version);
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"IFDID\": \"0123456789abcdef\",\n"
							   "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
							   "    \"SupportedAPI\": [\n"
							   "        \"IFDInterface_WebSocket_v0\",\n"
							   "        \"IFDInterface_WebSocket_v2\"\n"
							   "    ],\n"
							   "    \"addresses\": [\n"
							   "        \"[ADDRESS]\"\n"
							   "    ],\n"
							   "    \"msg\": \"REMOTE_IFD\",\n"
							   "[PAIRING]"
							   "    \"port\": 24728\n"
							   "}\n").replace("[ADDRESS]", json_address).replace("[PAIRING]", json_pairing));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), version >= IfdVersion::Version::v2 ? 7 : 6);
			QCOMPARE(obj.value(QLatin1String("IFDName")).toString(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(obj.value(QLatin1String("IFDID")).toString(), QStringLiteral("0123456789abcdef"));
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("REMOTE_IFD"));
			QCOMPARE(obj.value(QLatin1String("pairing")).toBool(!pairing), version < IfdVersion::Version::v2 ? !pairing : pairing);
			QCOMPARE(obj.value(QLatin1String("port")).toInt(), 24728);
			const QJsonValue apiLevels = obj.value(QLatin1String("SupportedAPI"));
			QVERIFY(apiLevels.isArray());
			QCOMPARE(apiLevels.toArray().size(), 2);
			QCOMPARE(apiLevels.toArray().at(0).toString(), QStringLiteral("IFDInterface_WebSocket_v0"));
			QCOMPARE(apiLevels.toArray().at(1).toString(), QStringLiteral("IFDInterface_WebSocket_v2"));
			const QJsonValue addresses = obj.value(QLatin1String("addresses"));
			QVERIFY(addresses.isArray());
			QCOMPARE(addresses.toArray().size(), hostAddresses.size());
		}


		void fromJson_data()
		{
			QTest::addColumn<QByteArray>("json_pairing");
			QTest::addColumn<bool>("pairing");
			QTest::addColumn<QByteArray>("json_address");
			QTest::addColumn<QList<QUrl>>("addresses");
			QTest::newRow("Pairing enabled - IPv4") << "\"pairing\": true,"_ba << true
													<< "\"wss://192.168.1.42:24728\""_ba << QList<QUrl>({QUrl("wss://192.168.1.42:24728"_L1)});
			QTest::newRow("Pairing disabled - IPv6") << "\"pairing\": false,"_ba << false
													 << "\"wss://[::ffff:192.168.1.42]:24728\""_ba << QList<QUrl>({QUrl("wss://[::ffff:192.168.1.42]:24728"_L1)});
			QTest::newRow("Pairing disabled - IPv4/6") << "\"pairing\": false,"_ba << false
													   << "\"wss://192.168.1.42:24728\", \"wss://[::ffff:192.168.1.42]:24728\""_ba << QList<QUrl>({QUrl("wss://192.168.1.42:24728"_L1), QUrl("wss://[::ffff:192.168.1.42]:24728"_L1)});
		}


		void fromJson()
		{
			QFETCH(QByteArray, json_pairing);
			QFETCH(bool, pairing);
			QFETCH(QByteArray, json_address);
			QFETCH(QList<QUrl>, addresses);

			QByteArray message(R"({
									"IFDID": "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF",
									"IFDName": "Sony Xperia Z5 compact",
									"SupportedAPI": [
										"IFDInterface_WebSocket_v0"
									],
									"addresses": [
										[ADDRESS]
									],
									"msg": "REMOTE_IFD",
									[PAIRING]
									"port": 24728
							   })");
			message.replace("[ADDRESS]", json_address).replace("[PAIRING]", json_pairing);

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(!discovery.isIncomplete());
			QCOMPARE(discovery.getType(), IfdMessageType::UNDEFINED);
			QCOMPARE(discovery.getContextHandle(), QString());
			QCOMPARE(discovery.getIfdName(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(discovery.getIfdId(), QByteArray::fromHex(QByteArrayLiteral("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF")));
			QVERIFY(discovery.getPort() == static_cast<quint16>(24728));
			QCOMPARE(discovery.getAddresses(), addresses);
			QCOMPARE(discovery.getSupportedApis(), QList<IfdVersion::Version>({IfdVersion::Version::v0}));
			QCOMPARE(discovery.isPairing(), pairing);
		}


		void pairing_data()
		{
			const QByteArray fingerprint("0575e99867361c26442ece18bed6f955ab7dd269ae8f42d3a21af0e734c3d8d9");
			const QByteArray certificate(R"(-----BEGIN CERTIFICATE-----
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
											-----END CERTIFICATE-----)");

			QTest::addColumn<QByteArray>("json_version");
			QTest::addColumn<QByteArray>("json_ifdid");
			QTest::addColumn<QByteArray>("json_pairing");
			QTest::addColumn<bool>("incomplete");
			QTest::addColumn<bool>("pairing");

			QTest::newRow("v0 - Pairing true") << QByteArray(R"("IFDInterface_WebSocket_v0")") << fingerprint << QByteArray(R"("pairing": true,)") << false << true;
			QTest::newRow("v0 - Pairing false") << QByteArray(R"("IFDInterface_WebSocket_v0")") << fingerprint << QByteArray(R"("pairing": false,)") << false << false;
			QTest::newRow("v0 - Pairing missing") << QByteArray(R"("IFDInterface_WebSocket_v0")") << fingerprint << QByteArray() << false << false;
			QTest::newRow("v2 - Pairing true") << QByteArray(R"("IFDInterface_WebSocket_v2")") << certificate << QByteArray(R"("pairing": true,)") << false << true;
			QTest::newRow("v2 - Pairing false") << QByteArray(R"("IFDInterface_WebSocket_v2")") << certificate << QByteArray(R"("pairing": false,)") << false << false;
			QTest::newRow("v2 - Pairing missing") << QByteArray(R"("IFDInterface_WebSocket_v2")") << certificate << QByteArray() << true << false;
			QTest::newRow("02 - Pairing true") << QByteArray(R"("IFDInterface_WebSocket_v0", "IFDInterface_WebSocket_v2")") << certificate << QByteArray(R"("pairing": true,)") << false << true;
			QTest::newRow("02 - Pairing false") << QByteArray(R"("IFDInterface_WebSocket_v0", "IFDInterface_WebSocket_v2")") << certificate << QByteArray(R"("pairing": false,)") << false << false;
			QTest::newRow("02 - Pairing missing") << QByteArray(R"("IFDInterface_WebSocket_v0", "IFDInterface_WebSocket_v2")") << certificate << QByteArray() << true << false;
		}


		void pairing()
		{
			QFETCH(QByteArray, json_version);
			QFETCH(QByteArray, json_ifdid);
			QFETCH(QByteArray, json_pairing);
			QFETCH(bool, incomplete);
			QFETCH(bool, pairing);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message("{\n"
							   "	\"IFDID\": \"[IFDID]\",\n"
							   "	\"IFDName\": \"Sony Xperia Z5 compact\",\n"
							   "	\"SupportedAPI\": [\n"
							   "		[VERSION]\n"
							   "	],\n"
							   "	\"addresses\": [\n"
							   "		\"wss://192.168.1.42:27728\"\n"
							   "	],\n"
							   "	\"msg\": \"REMOTE_IFD\",\n"
							   "	[PAIRING]\n"
							   "	\"port\": 24728\n"
							   "}");
			message.replace("[VERSION]", json_version);
			message.replace("[IFDID]", json_ifdid);
			message.replace("[PAIRING]", json_pairing);

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QCOMPARE(discovery.isIncomplete(), incomplete);
			QCOMPARE(discovery.isPairing(), pairing);

			QCOMPARE(logSpy.count(), incomplete ? 1 : 0);
			if (incomplete)
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"pairing\"")));
			}
		}


		void msgField_data()
		{
			QTest::addColumn<QByteArray>("type");

			const auto& msgTypes = Enum<IfdMessageType>::getList();
			for (const auto& type : msgTypes)
			{
				QTest::newRow(getEnumName(type).data()) << QString(getEnumName(type)).toUtf8();
			}
		}


		void msgField()
		{
			QFETCH(QByteArray, type);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message("{\n"
							   "	\"IFDName\": \"Sony Xperia Z5 compact\",\n"
							   "	\"IFDID\": \"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF\",\n"
							   "	\"port\": 24728,\n"
							   "	\"addresses\": [\"wss://192.168.1.42:27728\"],\n"
							   "	\"SupportedAPI\": [\"IFDInterface_WebSocket_v0\"],\n"
							   "	\"pairing\": true,\n"
							   "	\"msg\": \"%1\"\n"
							   "}");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", type)).object();
			const Discovery discovery(obj);

			QVERIFY(discovery.isIncomplete());
			QCOMPARE(discovery.getType(), IfdMessageType::UNDEFINED);

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be REMOTE_IFD")));
		}


		void ignoreContext()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "	\"ContextHandle\": \"TestContext\",\n"
									 "	\"IFDID\": \"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF\",\n"
									 "	\"IFDName\": \"Sony Xperia Z5 compact\",\n"
									 "	\"SupportedAPI\": [\n"
									 "		\"IFDInterface_WebSocket_v0\"\n"
									 "	],\n"
									 "	\"addresses\": [\n"
									 "		\"wss://192.168.1.42:27728\"\n"
									 "	],\n"
									 "	\"msg\": \"REMOTE_IFD\",\n"
									 "	\"pairing\": true,\n"
									 "	\"port\": 24728\n"
									 "}");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(!discovery.isIncomplete());
			QCOMPARE(discovery.getContextHandle(), QString());

			QCOMPARE(logSpy.count(), 0);
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message(R"({
										"IFDID": 1,
										"IFDName": 2,
										"SupportedAPI": "IFDInterface_WebSocket_v0",
										"msg": "REMOTE_IFD",
										"pairing": 3,
										"port": "4",
										"addresses": "192.168.1.42:24728"
									 })");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(discovery.isIncomplete());
			QCOMPARE(discovery.getIfdName(), QString());
			QCOMPARE(discovery.getIfdId(), QByteArray());
			QVERIFY(discovery.getPort() == 0);
			QCOMPARE(discovery.getSupportedApis(), QList<IfdVersion::Version>());
			QCOMPARE(discovery.isPairing(), false);

			QCOMPARE(logSpy.count(), 6);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"IFDName\" should be of type \"string\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"IFDID\" should be of type \"string\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"port\" should be of type \"number\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"SupportedAPI\" should be of type \"array\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"pairing\" should be of type \"boolean\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"addresses\" should be of type \"array\"")));
		}


		void emptyApiArray()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "	\"ContextHandle\": \"TestContext\",\n"
									 "	\"IFDID\": \"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF\",\n"
									 "	\"IFDName\": \"Sony Xperia Z5 compact\",\n"
									 "	\"SupportedAPI\": [\n"
									 "	],\n"
									 "	\"addresses\": [\n"
									 "		\"wss://192.168.1.42:27728\"\n"
									 "	],\n"
									 "	\"msg\": \"REMOTE_IFD\",\n"
									 "	\"pairing\": true,\n"
									 "	\"port\": 24728\n"
									 "}");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(discovery.isIncomplete());
			QVERIFY(discovery.getSupportedApis().isEmpty());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("At least one entry is required for \"SupportedAPI\"")));
		}


		void wrongApiType()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "	\"ContextHandle\": \"TestContext\",\n"
									 "	\"IFDID\": \"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF\",\n"
									 "	\"IFDName\": \"Sony Xperia Z5 compact\",\n"
									 "	\"SupportedAPI\": [\n"
									 "		0,\n"
									 "		\"IFDInterface_WebSocket_v0\"\n"
									 "	],\n"
									 "	\"addresses\": [\n"
									 "		\"wss://192.168.1.42:27728\"\n"
									 "	],\n"
									 "	\"msg\": \"REMOTE_IFD\",\n"
									 "	\"pairing\": true,\n"
									 "	\"port\": 24728\n"
									 "}");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(discovery.isIncomplete());
			QCOMPARE(discovery.getSupportedApis(), QList<IfdVersion::Version>({IfdVersion::Version::v0}));

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"SupportedAPI\" should be of type \"string array\"")));
		}


		void emptyAddresses()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "	\"ContextHandle\": \"TestContext\",\n"
									 "	\"IFDID\": \"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF\",\n"
									 "	\"IFDName\": \"Sony Xperia Z5 compact\",\n"
									 "	\"SupportedAPI\": [\n"
									 "		\"IFDInterface_WebSocket_v0\"\n"
									 "	],\n"
									 "	\"addresses\": [\n"
									 "	],\n"
									 "	\"msg\": \"REMOTE_IFD\",\n"
									 "	\"pairing\": true,\n"
									 "	\"port\": 24728\n"
									 "}");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(discovery.isIncomplete());
			QVERIFY(discovery.getAddresses().isEmpty());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("At least one entry is required for \"addresses\"")));
		}


		void wrongAddressesType()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "	\"ContextHandle\": \"TestContext\",\n"
									 "	\"IFDID\": \"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF\",\n"
									 "	\"IFDName\": \"Sony Xperia Z5 compact\",\n"
									 "	\"SupportedAPI\": [\n"
									 "		\"IFDInterface_WebSocket_v0\"\n"
									 "	],\n"
									 "	\"addresses\": [\n"
									 "		0,\n"
									 "		\"192.168.1.41:24728\",\n"
									 "		\"abs://192.168.1.42:24728\",\n"
									 "		\"wss://:24728\",\n"
									 "		\"wss://192.168.1.42\",\n"
									 "		\"wss://192.168.1.42:24728\"\n"
									 "	],\n"
									 "	\"msg\": \"REMOTE_IFD\",\n"
									 "	\"pairing\": true,\n"
									 "	\"port\": 24728\n"
									 "}");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(discovery.isIncomplete());
			QCOMPARE(discovery.getAddresses(), {QUrl("wss://192.168.1.42:24728"_L1)});

			QCOMPARE(logSpy.count(), 5);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"addresses\" should be of type \"string\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"addresses\" should be of type \"url\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Found \"addresses\" entry with wrong scheme: abs://192.168.1.42:24728")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Found \"addresses\" entry without host: wss://:24728")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Found \"addresses\" entry without port: wss://192.168.1.42")));
		}


		void IfdId_data()
		{
			const QByteArray fingerprint("0575e99867361c26442ece18bed6f955ab7dd269ae8f42d3a21af0e734c3d8d9");
			const QByteArray certificate(R"(-----BEGIN CERTIFICATE-----
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
											-----END CERTIFICATE-----)");

			QTest::addColumn<QByteArray>("json_version");
			QTest::addColumn<QByteArray>("json_ifdid");
			QTest::addColumn<QByteArray>("ifdid");
			QTest::addColumn<bool>("incomplete");

			QTest::newRow("v0 - fingerprint") << QByteArray(R"("IFDInterface_WebSocket_v0")") << fingerprint << QByteArray::fromHex(fingerprint) << false;
			QTest::newRow("v0 - certificate") << QByteArray(R"("IFDInterface_WebSocket_v0")") << certificate << QByteArray::fromHex(fingerprint) << false;
			QTest::newRow("v0 - not empty") << QByteArray(R"("IFDInterface_WebSocket_v0")") << QByteArray("0123456789ABCDEF") << QByteArray::fromHex("0123456789ABCDEF") << false;
			QTest::newRow("v0 - empty") << QByteArray(R"("IFDInterface_WebSocket_v0")") << QByteArray() << QByteArray() << true;
			QTest::newRow("v2 - fingerprint") << QByteArray(R"("IFDInterface_WebSocket_v2")") << fingerprint << QByteArray::fromHex(fingerprint) << false;
			QTest::newRow("v2 - certificate") << QByteArray(R"("IFDInterface_WebSocket_v2")") << certificate << QByteArray::fromHex(fingerprint) << false;
			QTest::newRow("v2 - not empty") << QByteArray(R"("IFDInterface_WebSocket_v2")") << QByteArray("0123456789ABCDEF") << QByteArray::fromHex("0123456789ABCDEF") << false;
			QTest::newRow("v2 - empty") << QByteArray(R"("IFDInterface_WebSocket_v2")") << QByteArray() << QByteArray() << true;
			QTest::newRow("02 - fingerprint") << QByteArray(R"("IFDInterface_WebSocket_v0", "IFDInterface_WebSocket_v2")") << fingerprint << QByteArray::fromHex(fingerprint) << false;
			QTest::newRow("02 - certificate") << QByteArray(R"("IFDInterface_WebSocket_v0", "IFDInterface_WebSocket_v2")") << certificate << QByteArray::fromHex(fingerprint) << false;
			QTest::newRow("02 - not empty") << QByteArray(R"("IFDInterface_WebSocket_v0", "IFDInterface_WebSocket_v2")") << QByteArray("0123456789ABCDEF") << QByteArray::fromHex("0123456789ABCDEF") << false;
			QTest::newRow("02 - empty") << QByteArray(R"("IFDInterface_WebSocket_v0", "IFDInterface_WebSocket_v2")") << QByteArray() << QByteArray() << true;

		}


		void IfdId()
		{
			QFETCH(QByteArray, json_version);
			QFETCH(QByteArray, json_ifdid);
			QFETCH(QByteArray, ifdid);
			QFETCH(bool, incomplete);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
								"IFDID": "[IFDID]",
								"IFDName": "Sony Xperia Z5 compact",
								"SupportedAPI": [[VERSION]],
								"msg": "REMOTE_IFD",
								"pairing": true,
								"port": 24728
							   })");
			message.replace("[IFDID]", json_ifdid);
			message.replace("[VERSION]", json_version);

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);

			QCOMPARE(discovery.isIncomplete(), incomplete);
			QCOMPARE(discovery.getIfdId(), ifdid);

			QCOMPARE(logSpy.count(), incomplete ? 1 : 0);
			if (incomplete)
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of IFDID should not be empty")));
			}
		}


		void setAddresses()
		{
			Discovery discovery(
				QStringLiteral("Sony Xperia Z5 compact"),
				QByteArray::fromHex(QByteArrayLiteral("0123456789ABCDEF")),
				24728,
				{IfdVersion::Version::v2});
			QVERIFY(!discovery.isIncomplete());

			discovery.setAddresses({QHostAddress()});
			QVERIFY(discovery.getAddresses().isEmpty());

			discovery.setAddresses({QHostAddress("192.168.1.10"_L1)});
			QCOMPARE(discovery.getAddresses().size(), 1);
			QCOMPARE(discovery.getAddresses().at(0), QUrl("wss://192.168.1.10:24728"_L1));
		}


		void setAddressesOnInvalid()
		{
			Discovery discovery((QJsonObject()));
			QVERIFY(discovery.isIncomplete());

			discovery.setAddresses({QHostAddress()});
			QVERIFY(discovery.getAddresses().isEmpty());

			discovery.setAddresses({QHostAddress("192.168.1.10"_L1)});
			QVERIFY(discovery.getAddresses().isEmpty());
		}


};

QTEST_GUILESS_MAIN(test_Discovery)
#include "test_Discovery.moc"
