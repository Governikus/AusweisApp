/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdTransmitResponse.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


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
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			IfdTransmitResponse msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 8);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ResultMajor\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"ResultMinor\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Missing value \"SlotHandle\""));
			QVERIFY(logSpy.at(6).at(0).toString().contains("Missing value \"ResponseAPDUs\""));
			QVERIFY(logSpy.at(7).at(0).toString().contains("The value of msg should be IFDTransmitResponse"));
		}


		void values()
		{
			const IfdTransmitResponse ifdTransmitResponse(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("9000")
				);

			QVERIFY(!ifdTransmitResponse.isIncomplete());
			QCOMPARE(ifdTransmitResponse.getType(), RemoteCardMessageType::IFDTransmitResponse);
			QCOMPARE(ifdTransmitResponse.getContextHandle(), QString());
			QCOMPARE(ifdTransmitResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdTransmitResponse.getResponseApdu(), QByteArray::fromHex("9000"));
			QVERIFY(!ifdTransmitResponse.resultHasError());
			QCOMPARE(ifdTransmitResponse.getResultMinor(), ECardApiResult::Minor::null);
		}


		void toJson()
		{
			const IfdTransmitResponse ifdTransmitResponse(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("9000")
				);

			const QByteArray& byteArray = ifdTransmitResponse.toByteArray(QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"ResponseAPDUs\": [\n"
							   "        \"9000\"\n"
							   "    ],\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDTransmitResponse\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 6);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDTransmitResponse"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			const QJsonArray array = obj.value(QLatin1String("ResponseAPDUs")).toArray();
			QCOMPARE(array.size(), 1);
			QCOMPARE(array.at(0).toString(), QStringLiteral("9000"));
			QCOMPARE(obj.value(QLatin1String("ResultMajor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"));
			QCOMPARE(obj.value(QLatin1String("ResultMinor")).toString(), QString());
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"ResponseAPDUs\": [\n"
									 "        \"9000\"\n"
									 "    ],\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDTransmitResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmitResponse ifdTransmitResponse(obj);
			QVERIFY(!ifdTransmitResponse.isIncomplete());
			QCOMPARE(ifdTransmitResponse.getType(), RemoteCardMessageType::IFDTransmitResponse);
			QCOMPARE(ifdTransmitResponse.getContextHandle(), QString("TestContext"));
			QCOMPARE(ifdTransmitResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdTransmitResponse.getResponseApdu(), QByteArray::fromHex("9000"));
			QVERIFY(!ifdTransmitResponse.resultHasError());
			QCOMPARE(ifdTransmitResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 0);
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

			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			QByteArray message("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"ResponseAPDUs\": [\n"
							   "        \"9000\"\n"
							   "    ],\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdTransmitResponse ifdTransmitResponse(obj);

			if (type == RemoteCardMessageType::IFDTransmitResponse)
			{
				QVERIFY(!ifdTransmitResponse.isIncomplete());
				QCOMPARE(ifdTransmitResponse.getType(), RemoteCardMessageType::IFDTransmitResponse);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdTransmitResponse.isIncomplete());
			QCOMPARE(ifdTransmitResponse.getType(), type);

			if (type == RemoteCardMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDTransmitResponse"));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDTransmitResponse"));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"ResponseAPDUs\": 1,\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": 2,\n"
									 "    \"msg\": \"IFDTransmitResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmitResponse ifdTransmitResponse(obj);
			QVERIFY(ifdTransmitResponse.isIncomplete());
			QCOMPARE(ifdTransmitResponse.getType(), RemoteCardMessageType::IFDTransmitResponse);
			QCOMPARE(ifdTransmitResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmitResponse.getSlotHandle(), QString());
			QCOMPARE(ifdTransmitResponse.getResponseApdu(), QByteArray());
			QVERIFY(!ifdTransmitResponse.resultHasError());
			QCOMPARE(ifdTransmitResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotHandle\" should be of type \"string\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"ResponseAPDUs\" should be of type \"array\""));
		}


		void wrongResponseApdusType()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"ResponseAPDUs\": [\n"
									 "        1\n"
									 "    ],\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDTransmitResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmitResponse ifdTransmitResponse(obj);
			QVERIFY(ifdTransmitResponse.isIncomplete());
			QCOMPARE(ifdTransmitResponse.getType(), RemoteCardMessageType::IFDTransmitResponse);
			QCOMPARE(ifdTransmitResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmitResponse.getSlotHandle(), QString("SlotHandle"));
			QCOMPARE(ifdTransmitResponse.getResponseApdu(), QByteArray());
			QVERIFY(!ifdTransmitResponse.resultHasError());
			QCOMPARE(ifdTransmitResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"ResponseAPDUs\" should be of type \"string array\""));
		}


		void multipleApdus()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"ResponseAPDUs\": [\n"
									 "        \"9000\",\n"
									 "        \"6300\"\n"
									 "    ],\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDTransmitResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmitResponse ifdTransmitResponse(obj);
			QVERIFY(!ifdTransmitResponse.isIncomplete());
			QCOMPARE(ifdTransmitResponse.getType(), RemoteCardMessageType::IFDTransmitResponse);
			QCOMPARE(ifdTransmitResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmitResponse.getSlotHandle(), QString("SlotHandle"));
			QCOMPARE(ifdTransmitResponse.getResponseApdu(), QByteArray::fromHex("9000"));
			QVERIFY(!ifdTransmitResponse.resultHasError());
			QCOMPARE(ifdTransmitResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Only using the first ResponseAPDU. Command chaining ist not supported yet"));
		}


};

QTEST_GUILESS_MAIN(test_IfdTransmitResponse)
#include "test_IfdTransmitResponse.moc"
