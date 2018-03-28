/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdModifyPin.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_IfdModifyPin
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			LogHandler::getInstance().init();
		}


		void invalidJson()
		{
			QSignalSpy logSpy(&LogHandler::getInstance(), &LogHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			IfdModifyPin msg(obj);

			QCOMPARE(logSpy.count(), 4);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"SlotHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"InputData\""));
		}


		void values()
		{
			const IfdModifyPin ifdModifyPin(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("abcd1234")
				);

			QCOMPARE(ifdModifyPin.getType(), RemoteCardMessageType::IFDModifyPIN);
			QCOMPARE(ifdModifyPin.getContextHandle(), QString());
			QCOMPARE(ifdModifyPin.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdModifyPin.getInputData(), QByteArray::fromHex("abcd1234"));
		}


		void toJson()
		{
			const IfdModifyPin ifdModifyPin(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("abcd1234")
				);

			const QJsonDocument& doc = ifdModifyPin.toJson(QStringLiteral("TestContext"));
			QVERIFY(doc.isObject());
			QCOMPARE(doc.toJson(),
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"InputData\": \"abcd1234\",\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDModifyPIN\"\n"
							   "}\n"));

			const QJsonObject obj = doc.object();
			QCOMPARE(obj.size(), 4);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDModifyPIN"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			QCOMPARE(obj.value(QLatin1String("InputData")).toString(), QStringLiteral("abcd1234"));
		}


		void fromJson()
		{
			QSignalSpy logSpy(&LogHandler::getInstance(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"InputData\": \"abcd1234\",\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDModifyPIN\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdModifyPin ifdModifyPin(obj);
			QCOMPARE(ifdModifyPin.getType(), RemoteCardMessageType::IFDModifyPIN);
			QCOMPARE(ifdModifyPin.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdModifyPin.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdModifyPin.getInputData(), QByteArray::fromHex("abcd1234"));

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

			QSignalSpy logSpy(&LogHandler::getInstance(), &LogHandler::fireLog);

			QByteArray message("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"InputData\": \"abcd1234\",\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdModifyPin ifdModifyPin(obj);

			if (type == RemoteCardMessageType::IFDModifyPIN)
			{
				QCOMPARE(ifdModifyPin.getType(), RemoteCardMessageType::IFDModifyPIN);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdModifyPin.isValid());
			QCOMPARE(ifdModifyPin.getType(), type);

			QCOMPARE(logSpy.count(), 0);
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(&LogHandler::getInstance(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"InputData\": 1,\n"
									 "    \"SlotHandle\": 2,\n"
									 "    \"msg\": \"IFDModifyPIN\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdModifyPin ifdModifyPin(obj);
			QCOMPARE(ifdModifyPin.getType(), RemoteCardMessageType::IFDModifyPIN);
			QCOMPARE(ifdModifyPin.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdModifyPin.getSlotHandle(), QString());
			QCOMPARE(ifdModifyPin.getInputData(), QByteArray());

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotHandle\" should be of type \"string\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"InputData\" should be of type \"string\""));
		}


};

QTEST_GUILESS_MAIN(test_IfdModifyPin)
#include "test_IfdModifyPin.moc"
