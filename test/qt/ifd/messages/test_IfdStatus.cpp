/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdStatus.h"

#include "AppSettings.h"
#include "LogHandler.h"
#include "TestFileHelper.h"
#include "VolatileSettings.h"

#include <QtTest>


using namespace governikus;


Q_DECLARE_METATYPE(IfdVersion::Version)


class test_IfdStatus
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);
		}


		void invalidJson()
		{
			const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			IfdStatus msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), v0Supported ? 10 : 9);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"msg\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ContextHandle\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"SlotName\"")));
			if (v0Supported)
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"PINCapabilities\"")));
			}
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"PINPad\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"MaxAPDULength\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ConnectedReader\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"CardAvailable\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDStatus")));
		}


		void values()
		{
			ReaderInfo info(QStringLiteral("SlotName"), ReaderManagerPlugInType::PCSC, CardInfo(CardType::NONE));
			info.setBasicReader(false);
			const IfdStatus ifdStatus(info);

			QVERIFY(!ifdStatus.isIncomplete());
			QCOMPARE(ifdStatus.getType(), IfdMessageType::IFDStatus);
			QCOMPARE(ifdStatus.getContextHandle(), QString());
			QCOMPARE(ifdStatus.getSlotName(), QStringLiteral("SlotName"));
			QVERIFY(ifdStatus.hasPinPad());
			QCOMPARE(ifdStatus.getMaxApduLength(), 500);
			QVERIFY(ifdStatus.getConnectedReader());
			QVERIFY(!ifdStatus.getCardAvailable());
		}


		void toJson_data()
		{
			QTest::addColumn<IfdVersion::Version>("version");
			QTest::addColumn<bool>("pinpad");
			QTest::addColumn<QByteArray>("json");
			QTest::newRow("Unknown - Pinpad enabled") << IfdVersion::Version::Unknown << true << QByteArray("    \"PINCapabilities\": {\n"
																											"        \"Destroy\": false,\n"
																											"        \"PACE\": true,\n"
																											"        \"eID\": false,\n"
																											"        \"eSign\": false\n"
																											"    },\n");
			QTest::newRow("Unknown - Pinpad disabled") << IfdVersion::Version::Unknown << false << QByteArray("    \"PINCapabilities\": {\n"
																											  "        \"Destroy\": false,\n"
																											  "        \"PACE\": false,\n"
																											  "        \"eID\": false,\n"
																											  "        \"eSign\": false\n"
																											  "    },\n");
			QTest::newRow("v0 - Pinpad enabled") << IfdVersion::Version::v0 << true << QByteArray("    \"PINCapabilities\": {\n"
																								  "        \"Destroy\": false,\n"
																								  "        \"PACE\": true,\n"
																								  "        \"eID\": false,\n"
																								  "        \"eSign\": false\n"
																								  "    },\n");
			QTest::newRow("v0 - Pinpad disabled") << IfdVersion::Version::v0 << false << QByteArray("    \"PINCapabilities\": {\n"
																									"        \"Destroy\": false,\n"
																									"        \"PACE\": false,\n"
																									"        \"eID\": false,\n"
																									"        \"eSign\": false\n"
																									"    },\n");
			QTest::newRow("v2 - Pinpad enabled") << IfdVersion::Version::v2 << true << QByteArray("    \"PINPad\": true,\n");
			QTest::newRow("v2 - Pinpad disabled") << IfdVersion::Version::v2 << false << QByteArray("    \"PINPad\": false,\n");
		}


		void toJson()
		{
			QFETCH(IfdVersion::Version, version);
			QFETCH(bool, pinpad);
			QFETCH(QByteArray, json);

			ReaderInfo info(QStringLiteral("SlotName"), ReaderManagerPlugInType::PCSC, CardInfo(CardType::NONE));
			info.setBasicReader(!pinpad);
			const IfdStatus ifdStatus(info);

			const QByteArray& byteArray = ifdStatus.toByteArray(version, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"CardAvailable\": false,\n"
							   "    \"ConnectedReader\": true,\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"EFATR\": null,\n"
							   "    \"EFDIR\": null,\n"
							   "    \"MaxAPDULength\": 500,\n"
							   "[PINPAD]"
							   "    \"SlotName\": \"SlotName\",\n"
							   "    \"msg\": \"IFDStatus\"\n"
							   "}\n").replace("[PINPAD]", json));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 9);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDStatus"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotName")).toString(), QStringLiteral("SlotName"));
			if (version >= IfdVersion::Version::v2)
			{
				QCOMPARE(obj.value(QLatin1String("PINPad")).toBool(), pinpad);
			}
			else
			{
				const QJsonObject cap = obj.value(QLatin1String("PINCapabilities")).toObject();
				QCOMPARE(cap.size(), 4);
				QCOMPARE(cap.value(QLatin1String("PACE")).toBool(), pinpad);
				QVERIFY(!cap.value(QLatin1String("eID")).toBool());
				QVERIFY(!cap.value(QLatin1String("eSign")).toBool());
				QVERIFY(!cap.value(QLatin1String("Destroy")).toBool());
			}
			QCOMPARE(obj.value(QLatin1String("MaxAPDULength")).toInt(), 500);
			QVERIFY(obj.value(QLatin1String("ConnectedReader")).toBool());
			QVERIFY(!obj.value(QLatin1String("CardAvailable")).toBool());
		}


		void fromJson_data()
		{
			const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();

			QTest::addColumn<QByteArray>("json");
			QTest::addColumn<bool>("pinpad");
			QTest::addColumn<bool>("incomplete");
			QTest::newRow("Enabled - Cap") << QByteArray(R"("PINCapabilities": { "Destroy": false, "PACE": true, "eID": false,  "eSign": false },)") << v0Supported << !v0Supported;
			QTest::newRow("Enabled - Pad") << QByteArray(R"("PINPad": true,)") << true << false;
			QTest::newRow("Disabled - Cap") << QByteArray(R"("PINCapabilities": { "Destroy": false, "PACE": false, "eID": false, "eSign": false },)") << false << !v0Supported;
			QTest::newRow("Disabled - Pad") << QByteArray(R"("PINPad": false,)") << false << false;
			QTest::newRow("Enabled - Both") << QByteArray(R"("PINCapabilities": { "Destroy": false, "PACE": true, "eID": false, "eSign": false }, "PINPad": true,)") << true << false;
			QTest::newRow("Disabled - Both") << QByteArray(R"("PINCapabilities": { "Destroy": false, "PACE": false, "eID": false, "eSign": false }, "PINPad": false,)") << false << false;
			QTest::newRow("En/Dis - Both") << QByteArray(R"("PINCapabilities": { "Destroy": false, "PACE": true, "eID": false, "eSign": false }, "PINPad": false,)") << false << false;
			QTest::newRow("Dis/En - Both") << QByteArray(R"("PINCapabilities": { "Destroy": false, "PACE": false, "eID": false, "eSign": false }, "PINPad": true,)") << true << false;
		}


		void fromJson()
		{
			QFETCH(QByteArray, json);
			QFETCH(bool, pinpad);
			QFETCH(bool, incomplete);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
									"CardAvailable": false,
									"ConnectedReader": false,
									"ContextHandle": "TestContext",
									"EFATR": null,
									"EFDIR": null,
									"MaxAPDULength": 500,
									[PINPAD]
									"SlotName": "SlotName",
									"msg": "IFDStatus"
							   })");
			message.replace("[PINPAD]", json);

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdStatus ifdStatus(obj);
			QCOMPARE(ifdStatus.isIncomplete(), incomplete);
			QCOMPARE(ifdStatus.getType(), IfdMessageType::IFDStatus);
			QCOMPARE(ifdStatus.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdStatus.getSlotName(), QStringLiteral("SlotName"));
			QCOMPARE(ifdStatus.hasPinPad(), pinpad);
			QCOMPARE(ifdStatus.getMaxApduLength(), 500);
			QVERIFY(!ifdStatus.getConnectedReader());
			QVERIFY(!ifdStatus.getCardAvailable());

			QCOMPARE(logSpy.count(), incomplete ? 1 : 0);
			if (incomplete)
			{
				QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"PINPad\""));
			}
		}


		void msgField_data()
		{
			QTest::addColumn<IfdMessageType>("type");

			const auto& msgTypes = Enum<IfdMessageType>::getList();
			for (const auto& type : msgTypes)
			{
				QTest::newRow(getEnumName(type).data()) << type;
			}
		}


		void msgField()
		{
			QFETCH(IfdMessageType, type);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
									"CardAvailable": false,
									"ConnectedReader": false,
									"ContextHandle": "TestContext",
									"EFATR": null,
									"EFDIR": null,
									"MaxAPDULength": 500,
									"PINCapabilities": {
										"Destroy": false,
										"PACE": true,
										"eID": false,
										"eSign": true
									},
									"PINPad": true,
									"SlotName": "SlotName",
									"msg": "%1"
							   })");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdStatus ifdStatus(obj);

			if (type == IfdMessageType::IFDStatus)
			{
				QVERIFY(!ifdStatus.isIncomplete());
				QCOMPARE(ifdStatus.getType(), IfdMessageType::IFDStatus);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdStatus.isIncomplete());
			QCOMPARE(ifdStatus.getType(), type);

			if (type == IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDStatus"));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDStatus"));
		}


		void wrongTypes()
		{
			const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message(R"({
										"CardAvailable": 1,
										"ConnectedReader": 2,
										"ContextHandle": "TestContext",
										"EFATR": null,
										"EFDIR": null,
										"MaxAPDULength": "3",
										"PINCapabilities": 4,
										"PINPad": 5,
										"SlotName": 6,
										"msg": "IFDStatus"
									 })");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdStatus ifdStatus(obj);
			QVERIFY(ifdStatus.isIncomplete());
			QCOMPARE(ifdStatus.getType(), IfdMessageType::IFDStatus);
			QCOMPARE(ifdStatus.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdStatus.getSlotName(), QString());
			QVERIFY(!ifdStatus.hasPinPad());
			QCOMPARE(ifdStatus.getMaxApduLength(), -1);
			QVERIFY(!ifdStatus.getConnectedReader());
			QVERIFY(!ifdStatus.getCardAvailable());

			QCOMPARE(logSpy.count(), v0Supported ? 6 : 5);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"SlotName\" should be of type \"string\"")));
			if (v0Supported)
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"PINCapabilities\" should be of type \"object\"")));
			}
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"PINPad\" should be of type \"boolean\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"MaxAPDULength\" should be of type \"number\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"ConnectedReader\" should be of type \"boolean\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"CardAvailable\" should be of type \"boolean\"")));
		}


		void wrongPINCapabilityTypes()
		{
			if (!IfdVersion(IfdVersion::Version::v0).isSupported())
			{
				QSKIP("PINCapabilities was only supported with IFDInterface_WebSocket_v0");
			}

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message(R"({
										"CardAvailable": false,
										"ConnectedReader": false,
										"ContextHandle": "TestContext",
										"EFATR": null,
										"EFDIR": null,
										"MaxAPDULength": 500,
										"PINCapabilities": {
											"Destroy": "1",
											"PACE": "2",
											"eID": "3",
											"eSign": "4"
										},
										"SlotName": "SlotName",
										"msg": "IFDStatus"
									 })");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdStatus ifdStatus(obj);
			QVERIFY(ifdStatus.isIncomplete());
			QCOMPARE(ifdStatus.getType(), IfdMessageType::IFDStatus);
			QCOMPARE(ifdStatus.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdStatus.getSlotName(), QStringLiteral("SlotName"));
			QVERIFY(!ifdStatus.hasPinPad());
			QCOMPARE(ifdStatus.getMaxApduLength(), 500);
			QVERIFY(!ifdStatus.getConnectedReader());
			QVERIFY(!ifdStatus.getCardAvailable());

			QCOMPARE(logSpy.count(), 4);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"PACE\" should be of type \"boolean\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"eID\" should be of type \"boolean\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"eSign\" should be of type \"boolean\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"Destroy\" should be of type \"boolean\"")));
		}


		void constructionWithReaderInfo_data()
		{
			QTest::addColumn<ReaderManagerPlugInType>("type");
			QTest::addColumn<bool>("isBasicReader");
			QTest::addColumn<int>("maxApduLength");
			QTest::addColumn<bool>("cardAvailable");
			QTest::addColumn<bool>("pinPadMode");

			QTest::newRow("NFC - Basic reader without card") << ReaderManagerPlugInType::NFC << true << 500 << false << false;
			QTest::newRow("NFC - Basic reader with card") << ReaderManagerPlugInType::NFC << true << 500 << true << false;
			QTest::newRow("NFC - Basic reader with card without extended length") << ReaderManagerPlugInType::NFC << true << 200 << true << false;
			QTest::newRow("NFC - Basic reader with card and pin pad mode") << ReaderManagerPlugInType::NFC << true << 500 << true << true;
			QTest::newRow("NFC - Comfort reader with card and enabled pin pad mode") << ReaderManagerPlugInType::NFC << false << 500 << true << true;
			QTest::newRow("NFC - Comfort reader with card and disabled pin pad mode") << ReaderManagerPlugInType::NFC << false << 500 << true << false;

			QTest::newRow("PCSC - Basic reader with pin pad mode") << ReaderManagerPlugInType::PCSC << true << 500 << false << true;
			QTest::newRow("PCSC - Basic reader without pin pad mode") << ReaderManagerPlugInType::PCSC << true << 500 << false << false;
			QTest::newRow("PCSC - Comfort reader with pin pad mode") << ReaderManagerPlugInType::PCSC << false << 500 << false << true;
			QTest::newRow("PCSC - Comfort reader without pin pad mode") << ReaderManagerPlugInType::PCSC << false << 500 << false << false;

			QTest::newRow("REMOTE_IFD - Basic reader with pin pad mode") << ReaderManagerPlugInType::REMOTE_IFD << true << 500 << false << true;
			QTest::newRow("REMOTE_IFD - Basic reader without pin pad mode") << ReaderManagerPlugInType::REMOTE_IFD << true << 500 << false << false;
			QTest::newRow("REMOTE_IFD - Comfort reader with pin pad mode") << ReaderManagerPlugInType::REMOTE_IFD << false << 500 << false << true;
			QTest::newRow("REMOTE_IFD - Comfort reader without pin pad mode") << ReaderManagerPlugInType::REMOTE_IFD << false << 500 << false << false;

			QTest::newRow("LOCAL_IFD - Basic reader with pin pad mode") << ReaderManagerPlugInType::LOCAL_IFD << true << 500 << false << true;
			QTest::newRow("LOCAL_IFD - Basic reader without pin pad mode") << ReaderManagerPlugInType::LOCAL_IFD << true << 500 << false << false;
			QTest::newRow("LOCAL_IFD - Comfort reader with pin pad mode") << ReaderManagerPlugInType::LOCAL_IFD << false << 500 << false << true;
			QTest::newRow("LOCAL_IFD - Comfort reader without pin pad mode") << ReaderManagerPlugInType::LOCAL_IFD << false << 500 << false << false;

			QTest::newRow("SMART - Basic reader with pin pad mode") << ReaderManagerPlugInType::SMART << true << 500 << false << true;
			QTest::newRow("SMART - Basic reader without pin pad mode") << ReaderManagerPlugInType::SMART << true << 500 << false << false;
			QTest::newRow("SMART - Comfort reader with pin pad mode") << ReaderManagerPlugInType::SMART << false << 500 << false << true;
			QTest::newRow("SMART - Comfort reader without pin pad mode") << ReaderManagerPlugInType::SMART << false << 500 << false << false;

			QTest::newRow("SIMULATOR - Basic reader with pin pad mode") << ReaderManagerPlugInType::SIMULATOR << true << 500 << false << true;
			QTest::newRow("SIMULATOR - Basic reader without pin pad mode") << ReaderManagerPlugInType::SIMULATOR << true << 500 << false << false;
			QTest::newRow("SIMULATOR - Comfort reader with pin pad mode") << ReaderManagerPlugInType::SIMULATOR << false << 500 << false << true;
			QTest::newRow("SIMULATOR - Comfort reader without pin pad mode") << ReaderManagerPlugInType::SIMULATOR << false << 500 << false << false;
		}


		void constructionWithReaderInfo()
		{
			const QString slotName("SlotName");
			QFETCH(ReaderManagerPlugInType, type);
			QFETCH(bool, isBasicReader);
			QFETCH(int, maxApduLength);
			QFETCH(bool, cardAvailable);
			QFETCH(bool, pinPadMode);

			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(pinPadMode);

			const CardInfo cardInfo(cardAvailable ? CardType::EID_CARD : CardType::NONE);

			ReaderInfo readerInfo(slotName, type, cardInfo);
			readerInfo.setBasicReader(isBasicReader);
			readerInfo.setMaxApduLength(maxApduLength);

			const IfdStatus ifdStatus(readerInfo);
			QVERIFY(!ifdStatus.isIncomplete());
			QCOMPARE(ifdStatus.getType(), IfdMessageType::IFDStatus);
			QCOMPARE(ifdStatus.getContextHandle(), QString());
			QCOMPARE(ifdStatus.getSlotName(), slotName);
			const bool isNfcOrSmart = type == ReaderManagerPlugInType::NFC || type == ReaderManagerPlugInType::SMART;
			QCOMPARE(ifdStatus.hasPinPad(), !isBasicReader || (isNfcOrSmart && pinPadMode));
			QCOMPARE(ifdStatus.getMaxApduLength(), maxApduLength);
			QCOMPARE(ifdStatus.getConnectedReader(), true);
			QCOMPARE(ifdStatus.getCardAvailable(), cardAvailable);
		}


};

QTEST_GUILESS_MAIN(test_IfdStatus)
#include "test_IfdStatus.moc"
