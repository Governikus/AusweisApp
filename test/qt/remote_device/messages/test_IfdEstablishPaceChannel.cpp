/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdEstablishPaceChannel.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_IfdEstablishPaceChannel
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

			IfdEstablishPaceChannel msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 6);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"SlotHandle\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"InputData\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("The value of msg should be IFDEstablishPACEChannel"));
		}


		void values()
		{
			const IfdEstablishPaceChannel ifdEstablishPaceChannel(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("abcd1234")
				);

			QVERIFY(!ifdEstablishPaceChannel.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannel.getType(), RemoteCardMessageType::IFDEstablishPACEChannel);
			QCOMPARE(ifdEstablishPaceChannel.getContextHandle(), QString());
			QCOMPARE(ifdEstablishPaceChannel.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdEstablishPaceChannel.getInputData(), QByteArray::fromHex("abcd1234"));
		}


		void toJson()
		{
			const IfdEstablishPaceChannel ifdEstablishPaceChannel(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("abcd1234")
				);

			const QByteArray& byteArray = ifdEstablishPaceChannel.toByteArray(QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"InputData\": \"abcd1234\",\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDEstablishPACEChannel\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 4);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDEstablishPACEChannel"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			QCOMPARE(obj.value(QLatin1String("InputData")).toString(), QStringLiteral("abcd1234"));
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"InputData\": \"abcd1234\",\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDEstablishPACEChannel\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannel ifdEstablishPaceChannel(obj);
			QVERIFY(!ifdEstablishPaceChannel.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannel.getType(), RemoteCardMessageType::IFDEstablishPACEChannel);
			QCOMPARE(ifdEstablishPaceChannel.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishPaceChannel.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdEstablishPaceChannel.getInputData(), QByteArray::fromHex("abcd1234"));

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
							   "    \"InputData\": \"abcd1234\",\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdEstablishPaceChannel ifdEstablishPaceChannel(obj);

			if (type == RemoteCardMessageType::IFDEstablishPACEChannel)
			{
				QVERIFY(!ifdEstablishPaceChannel.isIncomplete());
				QCOMPARE(ifdEstablishPaceChannel.getType(), RemoteCardMessageType::IFDEstablishPACEChannel);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdEstablishPaceChannel.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannel.getType(), type);

			if (type == RemoteCardMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDEstablishPACEChannel"));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDEstablishPACEChannel"));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"InputData\": 1,\n"
									 "    \"SlotHandle\": 2,\n"
									 "    \"msg\": \"IFDEstablishPACEChannel\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannel ifdEstablishPaceChannel(obj);
			QVERIFY(ifdEstablishPaceChannel.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannel.getType(), RemoteCardMessageType::IFDEstablishPACEChannel);
			QCOMPARE(ifdEstablishPaceChannel.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishPaceChannel.getSlotHandle(), QString());
			QCOMPARE(ifdEstablishPaceChannel.getInputData(), QByteArray());

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotHandle\" should be of type \"string\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"InputData\" should be of type \"string\""));
		}


};

QTEST_GUILESS_MAIN(test_IfdEstablishPaceChannel)
#include "test_IfdEstablishPaceChannel.moc"
