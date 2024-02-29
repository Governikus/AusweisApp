/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdModifyPinResponse.h"

#include "LogHandler.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_IfdModifyPinResponse
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

			IfdModifyPinResponse msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 8);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""_L1));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""_L1));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""_L1));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ResultMajor\""_L1));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"ResultMinor\""_L1));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Missing value \"SlotHandle\""_L1));
			QVERIFY(logSpy.at(6).at(0).toString().contains("Missing value \"OutputData\""_L1));
			QVERIFY(logSpy.at(7).at(0).toString().contains("The value of msg should be IFDModifyPINResponse"_L1));
		}


		void values()
		{
			const IfdModifyPinResponse ifdModifyPinResponse(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("ABCD1234")
				);

			QVERIFY(!ifdModifyPinResponse.isIncomplete());
			QCOMPARE(ifdModifyPinResponse.getType(), IfdMessageType::IFDModifyPINResponse);
			QCOMPARE(ifdModifyPinResponse.getContextHandle(), QString());
			QCOMPARE(ifdModifyPinResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdModifyPinResponse.getOutputData(), QByteArray::fromHex("ABCD1234"));
			QVERIFY(!ifdModifyPinResponse.resultHasError());
			QCOMPARE(ifdModifyPinResponse.getResultMinor(), ECardApiResult::Minor::null);
		}


		void toJson()
		{
			const IfdModifyPinResponse ifdModifyPinResponse(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("ABCD1234")
				);

			const QByteArray& byteArray = ifdModifyPinResponse.toByteArray(IfdVersion::Version::v0, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"OutputData\": \"abcd1234\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDModifyPINResponse\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 6);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDModifyPINResponse"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			QCOMPARE(obj.value(QLatin1String("OutputData")).toString(), QStringLiteral("abcd1234"));
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"OutputData\": \"ABCD1234\",\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDModifyPINResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdModifyPinResponse ifdModifyPinResponse(obj);
			QVERIFY(!ifdModifyPinResponse.isIncomplete());
			QCOMPARE(ifdModifyPinResponse.getType(), IfdMessageType::IFDModifyPINResponse);
			QCOMPARE(ifdModifyPinResponse.getContextHandle(), "TestContext"_L1);
			QCOMPARE(ifdModifyPinResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdModifyPinResponse.getOutputData(), QByteArray::fromHex("ABCD1234"));
			QVERIFY(!ifdModifyPinResponse.resultHasError());
			QCOMPARE(ifdModifyPinResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 0);
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

			QByteArray message("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"OutputData\": \"ABCD1234\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdModifyPinResponse ifdModifyPinResponse(obj);

			if (type == IfdMessageType::IFDModifyPINResponse)
			{
				QVERIFY(!ifdModifyPinResponse.isIncomplete());
				QCOMPARE(ifdModifyPinResponse.getType(), IfdMessageType::IFDModifyPINResponse);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdModifyPinResponse.isIncomplete());
			QCOMPARE(ifdModifyPinResponse.getType(), type);

			if (type == IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""_L1));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDModifyPINResponse"_L1));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDModifyPINResponse"_L1));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"OutputData\": 1,\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": 2,\n"
									 "    \"msg\": \"IFDModifyPINResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdModifyPinResponse ifdModifyPinResponse(obj);
			QVERIFY(ifdModifyPinResponse.isIncomplete());
			QCOMPARE(ifdModifyPinResponse.getType(), IfdMessageType::IFDModifyPINResponse);
			QCOMPARE(ifdModifyPinResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdModifyPinResponse.getSlotHandle(), QString());
			QCOMPARE(ifdModifyPinResponse.getOutputData(), QByteArray());
			QVERIFY(!ifdModifyPinResponse.resultHasError());
			QCOMPARE(ifdModifyPinResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotHandle\" should be of type \"string\""_L1));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"OutputData\" should be of type \"string\""_L1));
		}


		void test_GetReturnCode_data()
		{
			QTest::addColumn<ECardApiResult::Minor>("minor");
			QTest::addColumn<CardReturnCode>("code");

			QTest::newRow("timeout") << ECardApiResult::Minor::IFDL_Timeout_Error << CardReturnCode::INPUT_TIME_OUT;
			QTest::newRow("cancellation") << ECardApiResult::Minor::IFDL_CancellationByUser << CardReturnCode::CANCELLATION_BY_USER;
			QTest::newRow("mismatch") << ECardApiResult::Minor::IFDL_IO_RepeatedDataMismatch << CardReturnCode::NEW_PIN_MISMATCH;
			QTest::newRow("unknownPinFormat") << ECardApiResult::Minor::IFDL_IO_UnknownPINFormat << CardReturnCode::NEW_PIN_INVALID_LENGTH;
			QTest::newRow("unknownApiFunction") << ECardApiResult::Minor::AL_Unknown_API_Function << CardReturnCode::PROTOCOL_ERROR;
			QTest::newRow("unknownError") << ECardApiResult::Minor::AL_Unknown_Error << CardReturnCode::UNKNOWN;
			QTest::newRow("default") << ECardApiResult::Minor::SAL_Invalid_Key << CardReturnCode::COMMAND_FAILED;
		}


		void test_GetReturnCode()
		{
			QFETCH(ECardApiResult::Minor, minor);
			QFETCH(CardReturnCode, code);
			const QString slotHandle = QStringLiteral("slothandle");
			const QByteArray output("output");

			const IfdModifyPinResponse response1(slotHandle, output, ECardApiResult::Minor::null);
			const IfdModifyPinResponse response2(slotHandle, output, minor);

			QCOMPARE(response1.getReturnCode(), CardReturnCode::OK);
			QCOMPARE(response2.getReturnCode(), code);
		}


};

QTEST_GUILESS_MAIN(test_IfdModifyPinResponse)
#include "test_IfdModifyPinResponse.moc"
