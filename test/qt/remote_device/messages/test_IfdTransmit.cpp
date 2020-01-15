/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdTransmit.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


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
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			IfdTransmit msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 6);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"SlotHandle\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"CommandAPDUs\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("The value of msg should be IFDTransmit"));
		}


		void values()
		{
			const IfdTransmit ifdTransmit(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("00a402022f00")
				);

			QVERIFY(!ifdTransmit.isIncomplete());
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getContextHandle(), QString());
			QCOMPARE(ifdTransmit.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray::fromHex("00a402022f00"));
		}


		void toJson()
		{
			const IfdTransmit ifdTransmit(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("00a402022f00")
				);

			const QByteArray& byteArray = ifdTransmit.toByteArray(QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"CommandAPDUs\": [\n"
							   "        {\n"
							   "            \"AcceptableStatusCodes\": null,\n"
							   "            \"InputAPDU\": \"00a402022f00\"\n"
							   "        }\n"
							   "    ],\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDTransmit\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 4);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDTransmit"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			const QJsonArray array = obj.value(QLatin1String("CommandAPDUs")).toArray();
			QCOMPARE(array.size(), 1);
			const QJsonObject com = array.at(0).toObject();
			QCOMPARE(com.size(), 2);
			QVERIFY(com.value(QLatin1String("AcceptableStatusCodes")).isNull());
			QCOMPARE(com.value(QLatin1String("InputAPDU")).toString(), QStringLiteral("00a402022f00"));
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"CommandAPDUs\": [\n"
									 "        {\n"
									 "            \"AcceptableStatusCodes\": null,\n"
									 "            \"InputAPDU\": \"00a402022f00\"\n"
									 "        }\n"
									 "    ],\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDTransmit\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmit ifdTransmit(obj);
			QVERIFY(!ifdTransmit.isIncomplete());
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmit.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray::fromHex("00a402022f00"));

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
							   "    \"CommandAPDUs\": [\n"
							   "        {\n"
							   "            \"AcceptableStatusCodes\": null,\n"
							   "            \"InputAPDU\": \"00a402022f00\"\n"
							   "        }\n"
							   "    ],\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
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
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDTransmit"));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDTransmit"));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"CommandAPDUs\": 1,\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"SlotHandle\": 2,\n"
									 "    \"msg\": \"IFDTransmit\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmit ifdTransmit(obj);
			QVERIFY(ifdTransmit.isIncomplete());
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmit.getSlotHandle(), QString());
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray());

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotHandle\" should be of type \"string\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"CommandAPDUs\" should be of type \"array\""));
		}


		void wrongCommandApdusType()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"CommandAPDUs\": [\n"
									 "        1\n"
									 "    ],\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDTransmit\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmit ifdTransmit(obj);
			QVERIFY(ifdTransmit.isIncomplete());
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmit.getSlotHandle(), QString("SlotHandle"));
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"CommandAPDUs\" should be of type \"object array\""));
		}


		void wrongCommandApdusSubTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"CommandAPDUs\": [\n"
									 "        {\n"
									 "            \"AcceptableStatusCodes\": 1,\n"
									 "            \"InputAPDU\": 2\n"
									 "        }\n"
									 "    ],\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDTransmit\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmit ifdTransmit(obj);
			QVERIFY(ifdTransmit.isIncomplete());
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmit.getSlotHandle(), QString("SlotHandle"));
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"InputAPDU\" should be of type \"string\""));
		}


		void multipleApdus()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"CommandAPDUs\": [\n"
									 "        {\n"
									 "            \"AcceptableStatusCodes\": \"9000\",\n"
									 "            \"InputAPDU\": \"00a402022f00\"\n"
									 "        },\n"
									 "        {\n"
									 "            \"AcceptableStatusCodes\": \"6300\",\n"
									 "            \"InputAPDU\": \"00a402022f01\"\n"
									 "        }\n"
									 "    ],\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDTransmit\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdTransmit ifdTransmit(obj);
			QVERIFY(!ifdTransmit.isIncomplete());
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdTransmit.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray::fromHex("00a402022f00"));

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Only using the first CommandAPDU. Command chaining ist not supported yet"));
		}


};

QTEST_GUILESS_MAIN(test_IfdTransmit)
#include "test_IfdTransmit.moc"
