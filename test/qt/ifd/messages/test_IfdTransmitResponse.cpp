/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdTransmitResponse.h"

#include "LogHandler.h"
#include "TestFileHelper.h"

#include <QtTest>


using namespace governikus;


Q_DECLARE_METATYPE(IfdVersion::Version)


namespace
{
// https://bugreports.qt.io/browse/QTBUG-89363
inline QByteArray RESULT_OK()
{
	return QByteArray("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok");
}


} // namespace


class test_IfdTransmitResponse
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

			IfdTransmitResponse msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), v0Supported ? 9 : 8);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"msg\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ContextHandle\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ResultMajor\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ResultMinor\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"SlotHandle\"")));
			if (v0Supported)
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ResponseAPDUs\"")));
			}
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ResponseAPDU\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDTransmitResponse")));
		}


		void values()
		{
			const IfdTransmitResponse ifdTransmitResponse(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("9000")
				);

			QVERIFY(!ifdTransmitResponse.isIncomplete());
			QCOMPARE(ifdTransmitResponse.getType(), IfdMessageType::IFDTransmitResponse);
			QCOMPARE(ifdTransmitResponse.getContextHandle(), QString());
			QCOMPARE(ifdTransmitResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdTransmitResponse.getResponseApdu(), QByteArray::fromHex("9000"));
			QVERIFY(!ifdTransmitResponse.resultHasError());
			QCOMPARE(ifdTransmitResponse.getResultMinor(), ECardApiResult::Minor::null);
		}


		void toJson_data()
		{
			QTest::addColumn<IfdVersion::Version>("version");
			QTest::addColumn<QByteArray>("json");
			QTest::newRow("Unknown") << IfdVersion::Version::Unknown << QByteArray("    \"ResponseAPDUs\": [\n"
																				   "        \"9000\"\n"
																				   "    ],\n");
			QTest::newRow("v0") << IfdVersion::Version::v0 << QByteArray("    \"ResponseAPDUs\": [\n"
																		 "        \"9000\"\n"
																		 "    ],\n");
			QTest::newRow("v2") << IfdVersion::Version::v2 << QByteArray("    \"ResponseAPDU\": \"9000\",\n");
		}


		void toJson()
		{
			QFETCH(IfdVersion::Version, version);
			QFETCH(QByteArray, json);

			const IfdTransmitResponse ifdTransmitResponse(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("9000")
				);

			const QByteArray& byteArray = ifdTransmitResponse.toByteArray(version, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "[APDU]"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDTransmitResponse\"\n"
							   "}\n").replace("[APDU]", json));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 6);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDTransmitResponse"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			if (version >= IfdVersion::Version::v2)
			{
				QCOMPARE(obj.value(QLatin1String("ResponseAPDU")).toString(), QStringLiteral("9000"));
			}
			else
			{
				const QJsonArray array = obj.value(QLatin1String("ResponseAPDUs")).toArray();
				QCOMPARE(array.size(), 1);
				QCOMPARE(array.at(0).toString(), QStringLiteral("9000"));
			}
			QCOMPARE(obj.value(QLatin1String("ResultMajor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"));
			QCOMPARE(obj.value(QLatin1String("ResultMinor")).toString(), QString());
		}


		void fromJson_data()
		{
			QTest::addColumn<QByteArray>("json");
			QTest::addColumn<bool>("incomplete");
			QTest::newRow("ResponseAPDUs") << QByteArray(R"("ResponseAPDUs": [ "9000" ],)") << !IfdVersion(IfdVersion::Version::v0).isSupported();
			QTest::newRow("ResponseAPDU") << QByteArray(R"("ResponseAPDU": "9000",)") << false;
			QTest::newRow("Equal - Both") << QByteArray(R"("ResponseAPDUs": [ "9000" ], "ResponseAPDU": "9000",)") << false;
			QTest::newRow("Not Equal - Both") << QByteArray(R"("ResponseAPDUs": [ "9001" ], "ResponseAPDU": "9000",)") << false;
		}


		void fromJson()
		{
			QFETCH(QByteArray, json);
			QFETCH(bool, incomplete);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
										"ContextHandle": "TestContext",
										[APDU]
										"ResultMajor": "[OK]",
										"ResultMinor": null,
										"SlotHandle": "SlotHandle",
										"msg": "IFDTransmitResponse"
									 })");
			message.replace("[OK]", RESULT_OK());
			message.replace("[APDU]", json);

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmitResponse ifdTransmitResponse(obj);
			QCOMPARE(ifdTransmitResponse.isIncomplete(), incomplete);
			QCOMPARE(ifdTransmitResponse.getType(), IfdMessageType::IFDTransmitResponse);
			QCOMPARE(ifdTransmitResponse.getContextHandle(), QString("TestContext"));
			QCOMPARE(ifdTransmitResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdTransmitResponse.getResponseApdu(), incomplete ? QByteArray() : QByteArray::fromHex("9000"));
			QVERIFY(!ifdTransmitResponse.resultHasError());
			QCOMPARE(ifdTransmitResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), incomplete ? 1 : 0);
			if (incomplete)
			{
				QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"ResponseAPDU\""));
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
									"ContextHandle": "TestContext",
									"ResponseAPDUs": [
										"9000"
									],
									"ResponseAPDU": "9000",
									"ResultMajor": "[OK]",
									"ResultMinor": null,
									"SlotHandle": "SlotHandle",
									"msg": "%1"
							   })");
			message.replace("[OK]", RESULT_OK());

			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdTransmitResponse ifdTransmitResponse(obj);

			if (type == IfdMessageType::IFDTransmitResponse)
			{
				QVERIFY(!ifdTransmitResponse.isIncomplete());
				QCOMPARE(ifdTransmitResponse.getType(), IfdMessageType::IFDTransmitResponse);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdTransmitResponse.isIncomplete());
			QCOMPARE(ifdTransmitResponse.getType(), type);

			if (type == IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"UNDEFINED\"")));
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDTransmitResponse")));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDTransmitResponse")));
		}


		void wrongTypes()
		{
			const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
										"ContextHandle": "TestContext",
										"ResponseAPDUs": 1,
										"ResponseAPDU": 2,
										"ResultMajor": "[OK]",
										"ResultMinor": null,
										"SlotHandle": 3,
										"msg": "IFDTransmitResponse"
									 })");
			message.replace("[OK]", RESULT_OK());

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmitResponse ifdTransmitResponse(obj);
			QVERIFY(ifdTransmitResponse.isIncomplete());
			QCOMPARE(ifdTransmitResponse.getType(), IfdMessageType::IFDTransmitResponse);
			QCOMPARE(ifdTransmitResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmitResponse.getSlotHandle(), QString());
			QCOMPARE(ifdTransmitResponse.getResponseApdu(), QByteArray());
			QVERIFY(!ifdTransmitResponse.resultHasError());
			QCOMPARE(ifdTransmitResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), v0Supported ? 3 : 2);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"SlotHandle\" should be of type \"string\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"ResponseAPDU\" should be of type \"string\"")));
			if (v0Supported)
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"ResponseAPDUs\" should be of type \"array\"")));
			}
		}


		void wrongResponseApdusType()
		{
			if (!IfdVersion(IfdVersion::Version::v0).isSupported())
			{
				QSKIP("ResponseAPDUs was only supported with IFDInterface_WebSocket_v0");
			}

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
										"ContextHandle": "TestContext",
										"ResponseAPDUs": [
											1
										],
										"ResultMajor": "[OK]",
										"ResultMinor": null,
										"SlotHandle": "SlotHandle",
										"msg": "IFDTransmitResponse"
									 })");
			message.replace("[OK]", RESULT_OK());

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmitResponse ifdTransmitResponse(obj);
			QVERIFY(ifdTransmitResponse.isIncomplete());
			QCOMPARE(ifdTransmitResponse.getType(), IfdMessageType::IFDTransmitResponse);
			QCOMPARE(ifdTransmitResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmitResponse.getSlotHandle(), QString("SlotHandle"));
			QCOMPARE(ifdTransmitResponse.getResponseApdu(), QByteArray());
			QVERIFY(!ifdTransmitResponse.resultHasError());
			QCOMPARE(ifdTransmitResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"ResponseAPDUs\" should be of type \"string array\"")));
		}


		void multipleApdus()
		{
			if (!IfdVersion(IfdVersion::Version::v0).isSupported())
			{
				QSKIP("ResponseAPDUs was only supported with IFDInterface_WebSocket_v0");
			}

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
										"ContextHandle": "TestContext",
										"ResponseAPDUs": [
											"9000",
											"6300"
										],
										"ResultMajor": "[OK]",
										"ResultMinor": null,
										"SlotHandle": "SlotHandle",
										"msg": "IFDTransmitResponse"
									 })");
			message.replace("[OK]", RESULT_OK());

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmitResponse ifdTransmitResponse(obj);
			QVERIFY(!ifdTransmitResponse.isIncomplete());
			QCOMPARE(ifdTransmitResponse.getType(), IfdMessageType::IFDTransmitResponse);
			QCOMPARE(ifdTransmitResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmitResponse.getSlotHandle(), QString("SlotHandle"));
			QCOMPARE(ifdTransmitResponse.getResponseApdu(), QByteArray::fromHex("9000"));
			QVERIFY(!ifdTransmitResponse.resultHasError());
			QCOMPARE(ifdTransmitResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Only using the first ResponseAPDU. Command chaining ist not supported yet")));
		}


};

QTEST_GUILESS_MAIN(test_IfdTransmitResponse)
#include "test_IfdTransmitResponse.moc"
