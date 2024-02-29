/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdDisconnect.h"

#include "LogHandler.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_IfdDisconnect
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

			IfdDisconnect msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 5);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""_L1));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""_L1));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""_L1));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"SlotHandle\""_L1));
			QVERIFY(logSpy.at(4).at(0).toString().contains("The value of msg should be IFDDisconnect"_L1));
		}


		void values()
		{
			const IfdDisconnect ifdDisconnect(
				QStringLiteral("SlotHandle")
				);

			QVERIFY(!ifdDisconnect.isIncomplete());
			QCOMPARE(ifdDisconnect.getType(), IfdMessageType::IFDDisconnect);
			QCOMPARE(ifdDisconnect.getContextHandle(), QString());
			QCOMPARE(ifdDisconnect.getSlotHandle(), QStringLiteral("SlotHandle"));
		}


		void toJson()
		{
			const IfdDisconnect ifdDisconnect(
				QStringLiteral("SlotHandle")
				);

			const QByteArray& byteArray = ifdDisconnect.toByteArray(IfdVersion::Version::v0, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDDisconnect\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 3);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDDisconnect"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDDisconnect\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdDisconnect ifdDisconnect(obj);
			QVERIFY(!ifdDisconnect.isIncomplete());
			QCOMPARE(ifdDisconnect.getType(), IfdMessageType::IFDDisconnect);
			QCOMPARE(ifdDisconnect.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdDisconnect.getSlotHandle(), QStringLiteral("SlotHandle"));

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
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdDisconnect ifdDisconnect(obj);

			if (type == IfdMessageType::IFDDisconnect)
			{
				QVERIFY(!ifdDisconnect.isIncomplete());
				QCOMPARE(ifdDisconnect.getType(), IfdMessageType::IFDDisconnect);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdDisconnect.isIncomplete());
			QCOMPARE(ifdDisconnect.getType(), type);

			if (type == IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""_L1));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDDisconnect"_L1));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDDisconnect"_L1));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"SlotHandle\": 1,\n"
									 "    \"msg\": \"IFDDisconnect\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdDisconnect ifdDisconnect(obj);
			QVERIFY(ifdDisconnect.isIncomplete());
			QCOMPARE(ifdDisconnect.getType(), IfdMessageType::IFDDisconnect);
			QCOMPARE(ifdDisconnect.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdDisconnect.getSlotHandle(), QString());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotHandle\" should be of type \"string\""_L1));
		}


};

QTEST_GUILESS_MAIN(test_IfdDisconnect)
#include "test_IfdDisconnect.moc"
