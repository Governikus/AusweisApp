/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdMessage.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_IfdMessage
	: public QObject
{
	Q_OBJECT

	private:
		void checkMissingContextHandle(IfdMessageType type)
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray input("{\n"
							 "    \"msg\": \"%1\"\n"
							 "}\n");
			const auto& obj = IfdMessage::parseByteArray(input.replace("%1", QTest::currentDataTag()));
			QVERIFY(!obj.isEmpty());

			IfdMessage msg(obj);
			QCOMPARE(msg.getType(), type);
			QCOMPARE(msg.getContextHandle(), QString());

			if (type == IfdMessageType::IFDEstablishContext)
			{
				QVERIFY(!msg.isIncomplete());
				QCOMPARE(logSpy.count(), 0);
				return;
			}

			QVERIFY(msg.isIncomplete());

			if (type != IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 1);
				QVERIFY(logSpy.at(0).at(0).toString().contains(QString("Missing value \"ContextHandle\"")));
				return;
			}

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains(QString("Invalid messageType received: \"")));
			QVERIFY(logSpy.at(1).at(0).toString().contains(QString("Missing value \"ContextHandle\"")));
		}


		void checkEmptyContextHandle(IfdMessageType type)
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray input("{\n"
							 "    \"ContextHandle\": \"\",\n"
							 "    \"msg\": \"%1\"\n"
							 "}\n");
			const auto& obj = IfdMessage::parseByteArray(input.replace("%1", QTest::currentDataTag()));
			QVERIFY(!obj.isEmpty());

			IfdMessage msg(obj);
			QCOMPARE(msg.getType(), type);
			QCOMPARE(msg.getContextHandle(), QString());

			if (type != IfdMessageType::UNDEFINED)
			{
				QVERIFY(!msg.isIncomplete());
				QCOMPARE(logSpy.count(), 0);
				return;
			}

			QVERIFY(msg.isIncomplete());
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains(QString("Invalid messageType received: \"")));
		}


		void checkValidContextHandle(IfdMessageType type)
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray input("{\n"
							 "    \"ContextHandle\": \"TestContext\",\n"
							 "    \"msg\": \"%1\"\n"
							 "}\n");
			const auto& obj = IfdMessage::parseByteArray(input.replace("%1", QTest::currentDataTag()));
			QVERIFY(!obj.isEmpty());

			IfdMessage msg(obj);
			QCOMPARE(msg.getType(), type);

			if (type == IfdMessageType::IFDEstablishContext)
			{
				QCOMPARE(msg.getContextHandle(), QString());
				QVERIFY(!msg.isIncomplete());
				QCOMPARE(logSpy.count(), 0);
				return;
			}

			QCOMPARE(msg.getContextHandle(), QString("TestContext"));

			if (type != IfdMessageType::UNDEFINED)
			{
				QVERIFY(!msg.isIncomplete());
				QCOMPARE(logSpy.count(), 0);
				return;
			}

			QVERIFY(msg.isIncomplete());
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains(QString("Invalid messageType received: \"")));
		}

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void invalidJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray input("FooBar");
			const auto& obj = IfdMessage::parseByteArray(input);
			QVERIFY(obj.isEmpty());

			IfdMessage msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 5);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Json parsing failed. 1 : \"illegal value\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Expected object at top level"));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"ContextHandle\""));
		}


		void missingObject()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray input("[]");
			const auto& obj = IfdMessage::parseByteArray(input);
			QVERIFY(obj.isEmpty());

			IfdMessage msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 4);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Expected object at top level"));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ContextHandle\""));
		}


		void emptyObject()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray input("{}");
			const auto& obj = IfdMessage::parseByteArray(input);
			QVERIFY(obj.isEmpty());

			IfdMessage msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 4);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Expected object at top level"));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ContextHandle\""));
		}


		void missingMessageType()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray input("{\n"
							 "    \"ContextHandle\": \"TestContext\"\n"
							 "}\n");
			const auto& obj = IfdMessage::parseByteArray(input.replace(50, 2, QTest::currentDataTag()));
			QVERIFY(!obj.isEmpty());

			IfdMessage msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains(QString("Missing value \"msg\"")));
			QVERIFY(logSpy.at(1).at(0).toString().contains(QString("Invalid messageType received: \"\"")));
		}


		void messageType_data()
		{
			QTest::addColumn<IfdMessageType>("type");

			QTest::newRow("") << IfdMessageType::UNDEFINED;
			const auto& typeList = Enum<IfdMessageType>::getList();
			for (const auto& type : typeList)
			{
				QTest::newRow(getEnumName(type).data()) << type;
			}
			QTest::newRow("unknown") << IfdMessageType::UNDEFINED;
		}


		void messageType()
		{
			QFETCH(IfdMessageType, type);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray input("{\n"
							 "    \"ContextHandle\": \"TestContext\",\n"
							 "    \"msg\": \"%1\"\n"
							 "}\n");
			const auto& obj = IfdMessage::parseByteArray(input.replace("%1", QTest::currentDataTag()));
			QVERIFY(!obj.isEmpty());

			IfdMessage msg(obj);
			QCOMPARE(msg.getType(), type);

			if (type != IfdMessageType::UNDEFINED)
			{
				QVERIFY(!msg.isIncomplete());
				QCOMPARE(logSpy.count(), 0);
				return;
			}

			QVERIFY(msg.isIncomplete());
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains(QString("Invalid messageType received: \"")));
		}


		void contextHandle_data()
		{
			QTest::addColumn<IfdMessageType>("type");

			QTest::newRow("") << IfdMessageType::UNDEFINED;
			const auto& msgTypes = Enum<IfdMessageType>::getList();
			for (const auto& type : msgTypes)
			{
				QTest::newRow(getEnumName(type).data()) << type;
			}
			QTest::newRow("unknown") << IfdMessageType::UNDEFINED;
		}


		void contextHandle()
		{
			QFETCH(IfdMessageType, type);

			checkMissingContextHandle(type);
			checkEmptyContextHandle(type);
			checkValidContextHandle(type);
		}


};

QTEST_GUILESS_MAIN(test_IfdMessage)
#include "test_IfdMessage.moc"
