/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdDisconnect.h"

#include "LogHandler.h"

#include <QtTest>


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
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"SlotHandle\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("The value of msg should be IFDDisconnect"));
		}


		void values()
		{
			const IfdDisconnect ifdDisconnect(
				QStringLiteral("SlotHandle")
				);

			QVERIFY(!ifdDisconnect.isIncomplete());
			QCOMPARE(ifdDisconnect.getType(), RemoteCardMessageType::IFDDisconnect);
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
			QCOMPARE(ifdDisconnect.getType(), RemoteCardMessageType::IFDDisconnect);
			QCOMPARE(ifdDisconnect.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdDisconnect.getSlotHandle(), QStringLiteral("SlotHandle"));

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

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdDisconnect ifdDisconnect(obj);

			if (type == RemoteCardMessageType::IFDDisconnect)
			{
				QVERIFY(!ifdDisconnect.isIncomplete());
				QCOMPARE(ifdDisconnect.getType(), RemoteCardMessageType::IFDDisconnect);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdDisconnect.isIncomplete());
			QCOMPARE(ifdDisconnect.getType(), type);

			if (type == RemoteCardMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDDisconnect"));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDDisconnect"));
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
			QCOMPARE(ifdDisconnect.getType(), RemoteCardMessageType::IFDDisconnect);
			QCOMPARE(ifdDisconnect.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdDisconnect.getSlotHandle(), QString());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotHandle\" should be of type \"string\""));
		}


};

QTEST_GUILESS_MAIN(test_IfdDisconnect)
#include "test_IfdDisconnect.moc"
