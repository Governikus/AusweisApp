/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "messages/Discovery.h"

#include "LogHandler.h"
#include "TestFileHelper.h"

#include <QtTest>


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
			QCOMPARE(msg.getIfdId(), QString());
			QVERIFY(msg.getPort() == 0);
			QCOMPARE(msg.getSupportedApis(), QVector<IfdVersion::Version>());
			QCOMPARE(msg.getPairing(), false);

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
				QStringLiteral("0123456789ABCDEF"),
				24728,
				{IfdVersion::Version::v0, IfdVersion::Version::v2}
				);

			QVERIFY(!discovery.isIncomplete());
			QCOMPARE(discovery.getType(), IfdMessageType::UNDEFINED);
			QCOMPARE(discovery.getContextHandle(), QString());
			QCOMPARE(discovery.getIfdName(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(discovery.getIfdId(), QStringLiteral("0123456789ABCDEF"));
			QVERIFY(discovery.getPort() == static_cast<quint16>(24728));
			QCOMPARE(discovery.getSupportedApis(), QVector<IfdVersion::Version>({IfdVersion::Version::v0, IfdVersion::Version::v2}));
			QVERIFY(!discovery.getPairing());

			discovery.setPairing(true);
			QVERIFY(discovery.getPairing());
		}


		void toJson_data()
		{
			QTest::addColumn<IfdVersion::Version>("version");
			QTest::addColumn<bool>("pairing");
			QTest::addColumn<QByteArray>("json");
			QTest::newRow("Unknown - Pairing enabled") << IfdVersion::Version::Unknown << true << QByteArray();
			QTest::newRow("Unknown - Pairing disabled") << IfdVersion::Version::Unknown << false << QByteArray();
			QTest::newRow("v0 - Pairing enabled") << IfdVersion::Version::v0 << true << QByteArray();
			QTest::newRow("v0 - Pairing disabled") << IfdVersion::Version::v0 << false << QByteArray();
			QTest::newRow("v2 - Pairing enabled") << IfdVersion::Version::v2 << true << QByteArray("    \"pairing\": true,\n");
			QTest::newRow("v2 - Pairing disabled") << IfdVersion::Version::v2 << false << QByteArray("    \"pairing\": false,\n");
		}


		void toJson()
		{
			QFETCH(IfdVersion::Version, version);
			QFETCH(bool, pairing);
			QFETCH(QByteArray, json);

			Discovery discovery(
				QStringLiteral("Sony Xperia Z5 compact"),
				QStringLiteral("0123456789ABCDEF"),
				24728,
				{IfdVersion::Version::v0, IfdVersion::Version::v2}
				);
			discovery.setPairing(pairing);

			const QByteArray& byteArray = discovery.toByteArray(version);
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"IFDID\": \"0123456789ABCDEF\",\n"
							   "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
							   "    \"SupportedAPI\": [\n"
							   "        \"IFDInterface_WebSocket_v0\",\n"
							   "        \"IFDInterface_WebSocket_v2\"\n"
							   "    ],\n"
							   "    \"msg\": \"REMOTE_IFD\",\n"
							   "[PAIRING]"
							   "    \"port\": 24728\n"
							   "}\n").replace("[PAIRING]", json));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), version >= IfdVersion::Version::v2 ? 6 : 5);
			QCOMPARE(obj.value(QLatin1String("IFDName")).toString(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(obj.value(QLatin1String("IFDID")).toString(), QStringLiteral("0123456789ABCDEF"));
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("REMOTE_IFD"));
			QCOMPARE(obj.value(QLatin1String("pairing")).toBool(!pairing), version < IfdVersion::Version::v2 ? !pairing : pairing);
			QCOMPARE(obj.value(QLatin1String("port")).toInt(), 24728);
			const QJsonValue apiLevels = obj.value(QLatin1String("SupportedAPI"));
			QVERIFY(apiLevels.isArray());
			QCOMPARE(apiLevels.toArray().size(), 2);
			QCOMPARE(apiLevels.toArray().at(0).toString(), QStringLiteral("IFDInterface_WebSocket_v0"));
			QCOMPARE(apiLevels.toArray().at(1).toString(), QStringLiteral("IFDInterface_WebSocket_v2"));
		}


		void fromJson_data()
		{
			QTest::addColumn<QByteArray>("json");
			QTest::addColumn<bool>("pairing");
			QTest::newRow("Pairing enabled") << QByteArray(R"("pairing": true,)") << true;
			QTest::newRow("Pairing disabled") << QByteArray(R"("pairing": false,)") << false;
		}


		void fromJson()
		{
			QFETCH(QByteArray, json);
			QFETCH(bool, pairing);

			QByteArray message(R"({
									"IFDID": "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF",
									"IFDName": "Sony Xperia Z5 compact",
									"SupportedAPI": [
										"IFDInterface_WebSocket_v0"
									],
									"msg": "REMOTE_IFD",
									[PAIRING]
									"port": 24728
							   })");
			message.replace("[PAIRING]", json);

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(!discovery.isIncomplete());
			QCOMPARE(discovery.getType(), IfdMessageType::UNDEFINED);
			QCOMPARE(discovery.getContextHandle(), QString());
			QCOMPARE(discovery.getIfdName(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(discovery.getIfdId(), QStringLiteral("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"));
			QVERIFY(discovery.getPort() == static_cast<quint16>(24728));
			QCOMPARE(discovery.getSupportedApis(), QVector<IfdVersion::Version>({IfdVersion::Version::v0}));
			QCOMPARE(discovery.getPairing(), pairing);
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

			QByteArray message(R"({
									"IFDID": "[IFDID]",
									"IFDName": "Sony Xperia Z5 compact",
									"SupportedAPI": [
										[VERSION]
									],
									"msg": "REMOTE_IFD",
									[PAIRING]
									"port": 24728
							   })");
			message.replace("[VERSION]", json_version);
			message.replace("[IFDID]", json_ifdid);
			message.replace("[PAIRING]", json_pairing);

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QCOMPARE(discovery.isIncomplete(), incomplete);
			QCOMPARE(discovery.getPairing(), pairing);

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

			QByteArray message(R"({
									"IFDName": "Sony Xperia Z5 compact",
									"IFDID": "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF",
									"port": 24728,
									"SupportedAPI": ["IFDInterface_WebSocket_v0"],
									"pairing": true,
									"msg": "%1"
							   })");
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

			const QByteArray message(R"({
										"ContextHandle": "TestContext",
										"IFDID": "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF",
										"IFDName": "Sony Xperia Z5 compact",
										"SupportedAPI": [
											"IFDInterface_WebSocket_v0"
										],
										"msg": "REMOTE_IFD",
										"pairing": true,
										"port": 24728
									 })");

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
										"port": "4"
									 })");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(discovery.isIncomplete());
			QCOMPARE(discovery.getIfdName(), QString());
			QCOMPARE(discovery.getIfdId(), QString());
			QVERIFY(discovery.getPort() == 0);
			QCOMPARE(discovery.getSupportedApis(), QVector<IfdVersion::Version>());
			QCOMPARE(discovery.getPairing(), false);

			QCOMPARE(logSpy.count(), 5);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"IFDName\" should be of type \"string\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"IFDID\" should be of type \"string\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"port\" should be of type \"number\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"SupportedAPI\" should be of type \"array\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"pairing\" should be of type \"boolean\"")));
		}


		void wrongApiType()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message(R"({
										"ContextHandle": "TestContext",
										"IFDID": "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF",
										"IFDName": "Sony Xperia Z5 compact",
										"SupportedAPI": [
											0,
											"IFDInterface_WebSocket_v0"
										],
										"msg": "REMOTE_IFD",
										"pairing": true,
										"port": 24728
									 })");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(discovery.isIncomplete());
			QCOMPARE(discovery.getSupportedApis(), QVector<IfdVersion::Version>({IfdVersion::Version::v0}));

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"SupportedAPI\" should be of type \"string array\"")));
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

			QTest::newRow("v0 - fingerprint") << QByteArray(R"("IFDInterface_WebSocket_v0")") << fingerprint << fingerprint << false;
			QTest::newRow("v0 - certificate") << QByteArray(R"("IFDInterface_WebSocket_v0")") << certificate << fingerprint << false;
			QTest::newRow("v0 - not empty") << QByteArray(R"("IFDInterface_WebSocket_v0")") << QByteArray("0123456789ABCDEF") << QByteArray("0123456789ABCDEF") << false;
			QTest::newRow("v0 - empty") << QByteArray(R"("IFDInterface_WebSocket_v0")") << QByteArray() << QByteArray() << true;
			QTest::newRow("v2 - fingerprint") << QByteArray(R"("IFDInterface_WebSocket_v2")") << fingerprint << fingerprint << false;
			QTest::newRow("v2 - certificate") << QByteArray(R"("IFDInterface_WebSocket_v2")") << certificate << fingerprint << false;
			QTest::newRow("v2 - not empty") << QByteArray(R"("IFDInterface_WebSocket_v2")") << QByteArray("0123456789ABCDEF") << QByteArray("0123456789ABCDEF") << false;
			QTest::newRow("v2 - empty") << QByteArray(R"("IFDInterface_WebSocket_v2")") << QByteArray() << QByteArray() << true;
			QTest::newRow("02 - fingerprint") << QByteArray(R"("IFDInterface_WebSocket_v0", "IFDInterface_WebSocket_v2")") << fingerprint << fingerprint << false;
			QTest::newRow("02 - certificate") << QByteArray(R"("IFDInterface_WebSocket_v0", "IFDInterface_WebSocket_v2")") << certificate << fingerprint << false;
			QTest::newRow("02 - not empty") << QByteArray(R"("IFDInterface_WebSocket_v0", "IFDInterface_WebSocket_v2")") << QByteArray("0123456789ABCDEF") << QByteArray("0123456789ABCDEF") << false;
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
			QCOMPARE(discovery.getIfdId(), QString::fromLatin1(ifdid));

			QCOMPARE(logSpy.count(), incomplete ? 1 : 0);
			if (incomplete)
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of IFDID should not be emtpy")));
			}
		}


		void sameIfdId()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray messageHash(R"({
											"IFDID": "0575e99867361c26442ece18bed6f955ab7dd269ae8f42d3a21af0e734c3d8d9",
											"IFDName": "Sony Xperia Z5 compact",
											"SupportedAPI": ["IFDInterface_WebSocket_v0"],
											"msg": "REMOTE_IFD",
											"pairing": true,
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
											"SupportedAPI": ["IFDInterface_WebSocket_v2"],
											"msg": "REMOTE_IFD",
											"pairing": true,
											"port": 24728
										 })");
			const QJsonObject& objCert = QJsonDocument::fromJson(messageCert).object();
			const Discovery discoveryCert(objCert);
			QCOMPARE(logSpy.count(), 0);

			QCOMPARE(discoveryHash.getIfdId(), discoveryCert.getIfdId());
		}


};

QTEST_GUILESS_MAIN(test_Discovery)
#include "test_Discovery.moc"
