/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdGetStatus.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_IfdGetStatus
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

			IfdGetStatus msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 5);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"SlotName\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("The value of msg should be IFDGetStatus"));
		}


		void values()
		{
			const IfdGetStatus ifdGetStatus(
				QStringLiteral("SlotName")
				);

			QVERIFY(!ifdGetStatus.isIncomplete());
			QCOMPARE(ifdGetStatus.getType(), RemoteCardMessageType::IFDGetStatus);
			QCOMPARE(ifdGetStatus.getContextHandle(), QString());
			QCOMPARE(ifdGetStatus.getSlotName(), QStringLiteral("SlotName"));
		}


		void toJson()
		{
			const IfdGetStatus ifdGetStatus(
				QStringLiteral("SlotName")
				);

			const QByteArray& byteArray = ifdGetStatus.toByteArray(QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"SlotName\": \"SlotName\",\n"
							   "    \"msg\": \"IFDGetStatus\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 3);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDGetStatus"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotName")).toString(), QStringLiteral("SlotName"));
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"SlotName\": \"SlotName\",\n"
									 "    \"msg\": \"IFDGetStatus\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdGetStatus ifdGetStatus(obj);
			QVERIFY(!ifdGetStatus.isIncomplete());
			QCOMPARE(ifdGetStatus.getType(), RemoteCardMessageType::IFDGetStatus);
			QCOMPARE(ifdGetStatus.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdGetStatus.getSlotName(), QStringLiteral("SlotName"));

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
							   "    \"SlotName\": \"SlotName\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdGetStatus ifdGetStatus(obj);

			if (type == RemoteCardMessageType::IFDGetStatus)
			{
				QVERIFY(!ifdGetStatus.isIncomplete());
				QCOMPARE(ifdGetStatus.getType(), RemoteCardMessageType::IFDGetStatus);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdGetStatus.isIncomplete());
			QCOMPARE(ifdGetStatus.getType(), type);

			if (type == RemoteCardMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDGetStatus"));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDGetStatus"));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"SlotName\": 1,\n"
									 "    \"msg\": \"IFDGetStatus\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdGetStatus ifdGetStatus(obj);
			QVERIFY(ifdGetStatus.isIncomplete());
			QCOMPARE(ifdGetStatus.getType(), RemoteCardMessageType::IFDGetStatus);
			QCOMPARE(ifdGetStatus.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdGetStatus.getSlotName(), QString());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotName\" should be of type \"string\""));
		}


};

QTEST_GUILESS_MAIN(test_IfdGetStatus)
#include "test_IfdGetStatus.moc"
