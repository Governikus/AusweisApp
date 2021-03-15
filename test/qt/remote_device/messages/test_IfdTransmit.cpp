/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdTransmit.h"

#include "LogHandler.h"
#include "TestFileHelper.h"

#include <QtTest>


using namespace governikus;


Q_DECLARE_METATYPE(IfdVersion::Version)


class test_IfdTransmit
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
			const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			IfdTransmit msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), v0Supported ? 7 : 6);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"msg\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ContextHandle\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"SlotHandle\"")));
			if (v0Supported)
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"CommandAPDUs\"")));
			}
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"InputAPDU\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDTransmit")));
		}


		void values()
		{
			const IfdTransmit ifdTransmit(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("00a402022f00"),
				QStringLiteral("Test")
				);

			QVERIFY(!ifdTransmit.isIncomplete());
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getContextHandle(), QString());
			QCOMPARE(ifdTransmit.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray::fromHex("00a402022f00"));
			QCOMPARE(ifdTransmit.getDisplayText(), QStringLiteral("Test"));
		}


		void toJson_data()
		{
			QTest::addColumn<IfdVersion::Version>("version");
			QTest::addColumn<QByteArray>("json1");
			QTest::addColumn<QByteArray>("json2");
			QTest::newRow("Unknown") << IfdVersion::Version::Unknown << QByteArray("    \"CommandAPDUs\": [\n"
																				   "        {\n"
																				   "            \"AcceptableStatusCodes\": null,\n"
																				   "            \"InputAPDU\": \"00a402022f00\"\n"
																				   "        }\n"
																				   "    ],\n") << QByteArray();
			QTest::newRow("v0") << IfdVersion::Version::v0 << QByteArray("    \"CommandAPDUs\": [\n"
																		 "        {\n"
																		 "            \"AcceptableStatusCodes\": null,\n"
																		 "            \"InputAPDU\": \"00a402022f00\"\n"
																		 "        }\n"
																		 "    ],\n") << QByteArray();
			QTest::newRow("v2") << IfdVersion::Version::v2 << QByteArray() << QByteArray("    \"InputAPDU\": \"00a402022f00\",\n");
		}


		void toJson()
		{
			QFETCH(IfdVersion::Version, version);
			QFETCH(QByteArray, json1);
			QFETCH(QByteArray, json2);

			const IfdTransmit ifdTransmit(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("00a402022f00")
				);

			const QByteArray& byteArray = ifdTransmit.toByteArray(version, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "[APDU1]"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "[APDU2]"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDTransmit\"\n"
							   "}\n").replace("[APDU1]", json1).replace("[APDU2]", json2));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 4);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDTransmit"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			if (version >= IfdVersion::Version::v2)
			{
				QCOMPARE(obj.value(QLatin1String("InputAPDU")).toString(), QStringLiteral("00a402022f00"));
			}
			else
			{
				const QJsonArray array = obj.value(QLatin1String("CommandAPDUs")).toArray();
				QCOMPARE(array.size(), 1);
				const QJsonObject com = array.at(0).toObject();
				QCOMPARE(com.size(), 2);
				QVERIFY(com.value(QLatin1String("AcceptableStatusCodes")).isNull());
				QCOMPARE(com.value(QLatin1String("InputAPDU")).toString(), QStringLiteral("00a402022f00"));
			}
		}


		void fromJson_data()
		{
			QTest::addColumn<QByteArray>("json");
			QTest::addColumn<QByteArray>("apdu");
			QTest::addColumn<bool>("incomplete");
			QTest::newRow("CommandAPDUs") << QByteArray(R"("CommandAPDUs": [ { "AcceptableStatusCodes": null, "InputAPDU": "00a402022f00" } ],)") << QByteArray("00a402022f00") << !IfdVersion(IfdVersion::Version::v0).isSupported();
			QTest::newRow("InputAPDU") << QByteArray(R"("InputAPDU": "00a402022f00",)") << QByteArray("00a402022f00") << false;
			QTest::newRow("Equal - Both") << QByteArray(R"("CommandAPDUs": [ { "AcceptableStatusCodes": null, "InputAPDU": "00a402022f00" } ], "InputAPDU": "00a402022f00",)") << QByteArray("00a402022f00") << false;
			QTest::newRow("Not Equal - Both") << QByteArray(R"("CommandAPDUs": [ { "AcceptableStatusCodes": null, "InputAPDU": "00a402022f01" } ], "InputAPDU": "00a402022f00",)") << QByteArray("00a402022f00") << false;
		}


		void fromJson()
		{
			QFETCH(QByteArray, json);
			QFETCH(QByteArray, apdu);
			QFETCH(bool, incomplete);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
									[APDU]
									"ContextHandle": "TestContext",
									"SlotHandle": "SlotHandle",
									"msg": "IFDTransmit"
								 })");
			message.replace("[APDU]", json);

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmit ifdTransmit(obj);
			QCOMPARE(ifdTransmit.isIncomplete(), incomplete);
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmit.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdTransmit.getInputApdu(), incomplete ? QByteArray() : QByteArray::fromHex(apdu));

			QCOMPARE(logSpy.count(), incomplete ? 1 : 0);
			if (incomplete)
			{
				QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"InputAPDU\""));
			}
		}


		void msgField_data()
		{
			QTest::addColumn<RemoteCardMessageType>("type");

			const auto& msgTypes = Enum<RemoteCardMessageType>::getList();
			for (const auto& type : msgTypes)
			{
				QTest::newRow(getEnumName(type).data()) << type;
			}
		}


		void msgField()
		{
			QFETCH(RemoteCardMessageType, type);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
									"CommandAPDUs": [
										{
											"AcceptableStatusCodes": null,
											"InputAPDU": "00a402022f00"
										}
									],
									"InputAPDU": "00a402022f00",
									"ContextHandle": "TestContext",
									"SlotHandle": "SlotHandle",
									"msg": "%1"
							   })");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdTransmit ifdTransmit(obj);

			if (type == RemoteCardMessageType::IFDTransmit)
			{
				QVERIFY(!ifdTransmit.isIncomplete());
				QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdTransmit.isIncomplete());
			QCOMPARE(ifdTransmit.getType(), type);

			if (type == RemoteCardMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"UNDEFINED\"")));
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDTransmit")));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDTransmit")));
		}


		void displayText_data()
		{
			QTest::addColumn<IfdVersion::Version>("version");
			QTest::addColumn<QString>("displayText");
			QTest::addColumn<bool>("included");

			QTest::newRow("vu - Null") << IfdVersion::Version::Unknown << QString() << false;
			QTest::newRow("vu - Empty") << IfdVersion::Version::Unknown << QString("") << false;
			QTest::newRow("vu - Text") << IfdVersion::Version::Unknown << QString("Text") << false;
			QTest::newRow("v0 - Null") << IfdVersion::Version::v0 << QString() << false;
			QTest::newRow("v0 - Empty") << IfdVersion::Version::v0 << QString("") << false;
			QTest::newRow("v0 - Text") << IfdVersion::Version::v0 << QString("Text") << false;
			QTest::newRow("v2 - Null") << IfdVersion::Version::v2 << QString() << false;
			QTest::newRow("v2 - Empty") << IfdVersion::Version::v2 << QString("") << true;
			QTest::newRow("v2 - Text") << IfdVersion::Version::v2 << QString("Text") << true;
		}


		void displayText()
		{
			QFETCH(IfdVersion::Version, version);
			QFETCH(QString, displayText);
			QFETCH(bool, included);

			const IfdTransmit transmit(QStringLiteral("SlotHandle"), QByteArray("00a402022f00"), displayText);
			const QByteArray message = transmit.toByteArray(version, QStringLiteral("TestContext"));
			const QJsonObject& obj = QJsonDocument::fromJson(message).object();

			QCOMPARE(obj.contains(QLatin1String("DisplayText")), included);
			QCOMPARE(obj.value(QLatin1String("DisplayText")).toString(QStringLiteral("FAIL")), included ? displayText : QStringLiteral("FAIL"));

			const IfdTransmit transmit1(obj);
			const QString text = transmit1.getDisplayText();
			QCOMPARE(text.isNull(), !included);
			QCOMPARE(text, included ? displayText : QString());
		}


		void wrongTypes()
		{
			const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message(R"({
										"CommandAPDUs": 1,
										"InputAPDU": 2,
										"ContextHandle": "TestContext",
										"SlotHandle": 3,
										"msg": "IFDTransmit"
									 })");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmit ifdTransmit(obj);
			QVERIFY(ifdTransmit.isIncomplete());
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmit.getSlotHandle(), QString());
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray());

			QCOMPARE(logSpy.count(), v0Supported ? 3 : 2);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"SlotHandle\" should be of type \"string\"")));
			if (v0Supported)
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"CommandAPDUs\" should be of type \"array\"")));
			}
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"InputAPDU\" should be of type \"string\"")));
		}


		void wrongCommandApdusType()
		{
			if (!IfdVersion(IfdVersion::Version::v0).isSupported())
			{
				QSKIP("CommandAPDUs was only supported with IFDInterface_WebSocket_v0");
			}

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message(R"({
										"CommandAPDUs": [
											1
										],
										"ContextHandle": "TestContext",
										"SlotHandle": "SlotHandle",
										"msg": "IFDTransmit"
									 })");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmit ifdTransmit(obj);
			QVERIFY(ifdTransmit.isIncomplete());
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmit.getSlotHandle(), QString("SlotHandle"));
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"CommandAPDUs\" should be of type \"object array\"")));
		}


		void wrongCommandApdusSubTypes()
		{
			if (!IfdVersion(IfdVersion::Version::v0).isSupported())
			{
				QSKIP("CommandAPDUs was only supported with IFDInterface_WebSocket_v0");
			}

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message(R"({
										"CommandAPDUs": [
											{
												"AcceptableStatusCodes": 1,
												"InputAPDU": 2
											}
										],
										"ContextHandle": "TestContext",
										"SlotHandle": "SlotHandle",
										"msg": "IFDTransmit"
									 })");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmit ifdTransmit(obj);
			QVERIFY(ifdTransmit.isIncomplete());
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmit.getSlotHandle(), QString("SlotHandle"));
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"InputAPDU\" should be of type \"string\"")));
		}


		void multipleApdus()
		{
			if (!IfdVersion(IfdVersion::Version::v0).isSupported())
			{
				QSKIP("CommandAPDUs was only supported with IFDInterface_WebSocket_v0");
			}

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message(R"({
										"CommandAPDUs": [
											{
												"AcceptableStatusCodes": "9000",
												"InputAPDU": "00a402022f00"
											},
											{
												"AcceptableStatusCodes": "6300",
												"InputAPDU": "00a402022f01"
											}
										],
										"ContextHandle": "TestContext",
										"SlotHandle": "SlotHandle",
										"msg": "IFDTransmit"
									 })");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmit ifdTransmit(obj);
			QVERIFY(!ifdTransmit.isIncomplete());
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmit.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray::fromHex("00a402022f00"));

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Only using the first CommandAPDU. Command chaining ist not supported yet")));
		}


};

QTEST_GUILESS_MAIN(test_IfdTransmit)
#include "test_IfdTransmit.moc"
