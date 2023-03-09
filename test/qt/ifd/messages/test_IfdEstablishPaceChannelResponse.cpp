/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdEstablishPaceChannelResponse.h"

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


class test_IfdEstablishPaceChannelResponse
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

			IfdEstablishPaceChannelResponse msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), v0Supported ? 8 : 9);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"msg\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ContextHandle\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ResultMajor\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ResultMinor\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"SlotHandle\"")));
			if (!v0Supported)
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ResultCode\"")));
			}
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"OutputData\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDEstablishPACEChannelResponse")));
		}


		void values()
		{
			const IfdEstablishPaceChannelResponse ifdEstablishPaceChannelResponse(
				QStringLiteral("SlotHandle"),
				EstablishPaceChannelOutput()
				);

			QVERIFY(!ifdEstablishPaceChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannelResponse.getType(), IfdMessageType::IFDEstablishPACEChannelResponse);
			QCOMPARE(ifdEstablishPaceChannelResponse.getContextHandle(), QString());
			QCOMPARE(ifdEstablishPaceChannelResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdEstablishPaceChannelResponse.getOutputData(), EstablishPaceChannelOutput());
			QVERIFY(!ifdEstablishPaceChannelResponse.resultHasError());
			QCOMPARE(ifdEstablishPaceChannelResponse.getResultMinor(), ECardApiResult::Minor::null);
		}


		void toJson_data()
		{
			QTest::addColumn<IfdVersion::Version>("version");
			QTest::addColumn<QByteArray>("outputData");
			QTest::addColumn<QByteArray>("resultCode");

			QTest::newRow("Unknown") << IfdVersion::Version::Unknown << QByteArray("3012a1060404f0100001a20404020000a30231009000") << QByteArray();
			QTest::newRow("v0") << IfdVersion::Version::v0 << QByteArray("3012a1060404f0100001a20404020000a30231009000") << QByteArray();
			QTest::newRow("v2") << IfdVersion::Version::v2 << QByteArray("000002003100") << QByteArray("    \"ResultCode\": \"010010f0\",\n");
		}


		void toJson()
		{
			QFETCH(IfdVersion::Version, version);
			QFETCH(QByteArray, outputData);
			QFETCH(QByteArray, resultCode);

			const IfdEstablishPaceChannelResponse ifdEstablishPaceChannelResponse(
				QStringLiteral("SlotHandle"),
				EstablishPaceChannelOutput()
				);

			const QByteArray& byteArray = ifdEstablishPaceChannelResponse.toByteArray(version, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"OutputData\": \"[DATA]\",\n"
							   "[CODE]"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDEstablishPACEChannelResponse\"\n"
							   "}\n").replace("[DATA]", outputData).replace("[CODE]", resultCode));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();

			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDEstablishPACEChannelResponse"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			if (version >= IfdVersion::Version::v2)
			{
				QCOMPARE(obj.size(), 7);
				QCOMPARE(obj.value(QLatin1String("ResultCode")).toString(), QStringLiteral("010010f0"));
				QCOMPARE(obj.value(QLatin1String("OutputData")).toString(), QStringLiteral("000002003100"));
			}
			else
			{
				QCOMPARE(obj.size(), 6);
				QVERIFY(!obj.contains(QLatin1String("ResultCode")));
				QCOMPARE(obj.value(QLatin1String("OutputData")).toString(), QStringLiteral("3012a1060404f0100001a20404020000a30231009000"));
			}
		}


		void fromJson_data()
		{
			QTest::addColumn<QByteArray>("outputData");
			QTest::addColumn<bool>("withResultCode");
			QTest::addColumn<bool>("incomplete");

			EstablishPaceChannelOutput outputData(CardReturnCode::OK);
			QTest::newRow("CCID - With ResultCode") << outputData.toCcid() << true << true;
			QTest::newRow("CCID - Without ResultCode") << outputData.toCcid() << false << !IfdVersion(IfdVersion::Version::v0).isSupported();
			QTest::newRow("PCSC - With ResultCode") << outputData.toOutputData() << true << false;
			QTest::newRow("PCSC - Without ResultCode") << outputData.toOutputData() << false << true;

		}


		void fromJson()
		{
			QFETCH(QByteArray, outputData);
			QFETCH(bool, withResultCode);
			QFETCH(bool, incomplete);

			const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
										"ContextHandle": "TestContext",
										"OutputData": "[DATA]",
										[RESULT]
										"ResultMajor": "[OK]",
										"ResultMinor": null,
										"SlotHandle": "SlotHandle",
										"msg": "IFDEstablishPACEChannelResponse"
									 })");
			message.replace("[OK]", RESULT_OK());
			message.replace("[DATA]", outputData.toHex());
			message.replace("[RESULT]", withResultCode ? QByteArray(R"("ResultCode": "00000000",)") : QByteArray());

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannelResponse ifdEstablishPaceChannelResponse(obj);
			QCOMPARE(ifdEstablishPaceChannelResponse.isIncomplete(), incomplete);
			QCOMPARE(ifdEstablishPaceChannelResponse.getType(), IfdMessageType::IFDEstablishPACEChannelResponse);
			QCOMPARE(ifdEstablishPaceChannelResponse.getContextHandle(), QString("TestContext"));
			QCOMPARE(ifdEstablishPaceChannelResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdEstablishPaceChannelResponse.getOutputData().getPaceReturnCode(), incomplete && !withResultCode ? CardReturnCode::COMMAND_FAILED : CardReturnCode::OK);
			QVERIFY(!ifdEstablishPaceChannelResponse.resultHasError());
			QCOMPARE(ifdEstablishPaceChannelResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), !v0Supported && !withResultCode ? 1 : incomplete && withResultCode ? 6 : 4);
			if (incomplete)
			{
				if (!v0Supported && !withResultCode)
				{
					QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ResultCode\"")));
					return;
				}

				if (!v0Supported || withResultCode)
				{
					QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of OutputData should be as defined in the result value table of [PC/SC], Part 10 AMD1, section 2.6.16")));
					return;
				}

				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of OutputData should be as defined in TR-03119 section D.3")));
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
									"OutputData": "000002003100",
									"ResultCode" : "00000000",
									"ResultMajor": "[OK]",
									"ResultMinor": null,
									"SlotHandle": "SlotHandle",
									"msg": "[TYPE]"
							   })");
			message.replace("[OK]", RESULT_OK());
			message.replace("[TYPE]", QTest::currentDataTag());

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannelResponse ifdEstablishPaceChannelResponse(obj);

			if (type == IfdMessageType::IFDEstablishPACEChannelResponse)
			{
				QVERIFY(!ifdEstablishPaceChannelResponse.isIncomplete());
				QCOMPARE(ifdEstablishPaceChannelResponse.getType(), IfdMessageType::IFDEstablishPACEChannelResponse);

				QCOMPARE(logSpy.count(), 4);

				return;
			}

			QVERIFY(ifdEstablishPaceChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannelResponse.getType(), type);

			if (type == IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"UNDEFINED\"")));
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDEstablishPACEChannelResponse")));

				return;
			}

			QCOMPARE(logSpy.count(), 5);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDEstablishPACEChannelResponse")));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
										"ContextHandle": "TestContext",
										"ResultCode" : 1,
										"OutputData": 2,
										"ResultMajor": "[OK]",
										"ResultMinor": null,
										"SlotHandle": 3,
										"msg": "IFDEstablishPACEChannelResponse"
									 })");
			message.replace("[OK]", RESULT_OK());

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannelResponse ifdEstablishPaceChannelResponse(obj);
			QVERIFY(ifdEstablishPaceChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannelResponse.getType(), IfdMessageType::IFDEstablishPACEChannelResponse);
			QCOMPARE(ifdEstablishPaceChannelResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishPaceChannelResponse.getSlotHandle(), QString());
			QCOMPARE(ifdEstablishPaceChannelResponse.getOutputData(), EstablishPaceChannelOutput());
			QVERIFY(!ifdEstablishPaceChannelResponse.resultHasError());
			QCOMPARE(ifdEstablishPaceChannelResponse.getResultMinor(), ECardApiResult::Minor::null);

			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"SlotHandle\" should be of type \"string\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"ResultCode\" should be of type \"string\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"OutputData\" should be of type \"string\"")));
		}


		void wrongResultCode()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
									"ContextHandle": "TestContext",
									"ResultCode" : "Hello World!",
									"OutputData": "000002003100",
									"ResultMajor": "[OK]",
									"ResultMinor": null,
									"SlotHandle": "SlotHandle",
									"msg": "IFDEstablishPACEChannelResponse"
							   })");
			message.replace("[OK]", RESULT_OK());

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannelResponse ifdEstablishPaceChannelResponse(obj);

			QVERIFY(ifdEstablishPaceChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannelResponse.getType(), IfdMessageType::IFDEstablishPACEChannelResponse);
			QCOMPARE(ifdEstablishPaceChannelResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishPaceChannelResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdEstablishPaceChannelResponse.getOutputData(), EstablishPaceChannelOutput());
			QVERIFY(!ifdEstablishPaceChannelResponse.resultHasError());
			QCOMPARE(ifdEstablishPaceChannelResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of ResultCode should be as defined in the result value table of [PC/SC], Part 10 AMD1, section 2.5.12")));
		}


		void wrongOutputData_data()
		{
			QTest::addColumn<bool>("withResultCode");

			QTest::newRow("withResultCode") << true;
			if (IfdVersion(IfdVersion::Version::v0).isSupported())
			{
				QTest::newRow("withoutResultCode") << false;
			}
		}


		void wrongOutputData()
		{
			QFETCH(bool, withResultCode);

			const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
									"ContextHandle": "TestContext",
									[RESULT]
									"OutputData": "Hello World!",
									"ResultMajor": "[OK]",
									"ResultMinor": null,
									"SlotHandle": "SlotHandle",
									"msg": "IFDEstablishPACEChannelResponse"
							   })");
			message.replace("[OK]", RESULT_OK());
			message.replace("[RESULT]", withResultCode ? QByteArray(R"("ResultCode": "00000000",)") : QByteArray());

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannelResponse ifdEstablishPaceChannelResponse(obj);

			QVERIFY(ifdEstablishPaceChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannelResponse.getType(), IfdMessageType::IFDEstablishPACEChannelResponse);
			QCOMPARE(ifdEstablishPaceChannelResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishPaceChannelResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdEstablishPaceChannelResponse.getOutputData(), EstablishPaceChannelOutput(withResultCode ? CardReturnCode::OK : CardReturnCode::COMMAND_FAILED));
			QVERIFY(!ifdEstablishPaceChannelResponse.resultHasError());
			QCOMPARE(ifdEstablishPaceChannelResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), withResultCode ? 3 : 2);
			if (!v0Supported || withResultCode)
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of OutputData should be as defined in the result value table of [PC/SC], Part 10 AMD1, section 2.6.16")));
			}
			else
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of OutputData should be as defined in TR-03119 section D.3")));
			}
		}


};

QTEST_GUILESS_MAIN(test_IfdEstablishPaceChannelResponse)
#include "test_IfdEstablishPaceChannelResponse.moc"
