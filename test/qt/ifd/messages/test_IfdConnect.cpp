/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdConnect.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_IfdConnect
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

			IfdConnect msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 6);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"SlotName\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"exclusive\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("The value of msg should be IFDConnect"));
		}


		void values()
		{
			const IfdConnect ifdConnect(
				QStringLiteral("SlotName")
				);

			QVERIFY(!ifdConnect.isIncomplete());
			QCOMPARE(ifdConnect.getType(), IfdMessageType::IFDConnect);
			QCOMPARE(ifdConnect.getContextHandle(), QString());
			QCOMPARE(ifdConnect.getSlotName(), QStringLiteral("SlotName"));
			QVERIFY(ifdConnect.isExclusive());
		}


		void toJson()
		{
			const IfdConnect ifdConnect(
				QStringLiteral("SlotName")
				);

			const QByteArray& byteArray = ifdConnect.toByteArray(IfdVersion::Version::v0, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"SlotName\": \"SlotName\",\n"
							   "    \"exclusive\": true,\n"
							   "    \"msg\": \"IFDConnect\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 4);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDConnect"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotName")).toString(), QStringLiteral("SlotName"));
			QCOMPARE(obj.value(QLatin1String("exclusive")).toBool(), true);
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"SlotName\": \"SlotName\",\n"
									 "    \"exclusive\": true,\n"
									 "    \"msg\": \"IFDConnect\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdConnect ifdConnect(obj);
			QVERIFY(!ifdConnect.isIncomplete());
			QCOMPARE(ifdConnect.getType(), IfdMessageType::IFDConnect);
			QCOMPARE(ifdConnect.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdConnect.getSlotName(), QStringLiteral("SlotName"));
			QVERIFY(ifdConnect.isExclusive());

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
							   "    \"SlotName\": \"SlotName\",\n"
							   "    \"exclusive\": true,\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdConnect ifdConnect(obj);

			if (type == IfdMessageType::IFDConnect)
			{
				QVERIFY(!ifdConnect.isIncomplete());
				QCOMPARE(ifdConnect.getType(), IfdMessageType::IFDConnect);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdConnect.isIncomplete());
			QCOMPARE(ifdConnect.getType(), type);

			if (type == IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDConnect"));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDConnect"));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"SlotName\": 1,\n"
									 "    \"exclusive\": 2,\n"
									 "    \"msg\": \"IFDConnect\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdConnect ifdConnect(obj);
			QVERIFY(ifdConnect.isIncomplete());
			QCOMPARE(ifdConnect.getType(), IfdMessageType::IFDConnect);
			QCOMPARE(ifdConnect.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdConnect.getSlotName(), QString());
			QVERIFY(!ifdConnect.isExclusive());

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotName\" should be of type \"string\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"exclusive\" should be of type \"boolean\""));
		}


};

QTEST_GUILESS_MAIN(test_IfdConnect)
#include "test_IfdConnect.moc"
