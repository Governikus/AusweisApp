/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdMessage.h"

#include "LogHandler.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
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
				QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"ContextHandle\""_L1));
				return;
			}

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \""_L1));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Missing value \"ContextHandle\""_L1));
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
			QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \""_L1));
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

			QCOMPARE(msg.getContextHandle(), "TestContext"_L1);

			if (type != IfdMessageType::UNDEFINED)
			{
				QVERIFY(!msg.isIncomplete());
				QCOMPARE(logSpy.count(), 0);
				return;
			}

			QVERIFY(msg.isIncomplete());
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \""_L1));
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
#if (QT_VERSION >= QT_VERSION_CHECK(6, 9, 0))
			QVERIFY(logSpy.at(0).at(0).toString().contains("Json parsing failed. 0 : \"illegal number\""_L1));
#else
			QVERIFY(logSpy.at(0).at(0).toString().contains("Json parsing failed. 1 : \"illegal value\""_L1));
#endif
			QVERIFY(logSpy.at(1).at(0).toString().contains("Expected object at top level"_L1));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"msg\""_L1));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Invalid messageType received: \"\""_L1));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"ContextHandle\""_L1));
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
			QVERIFY(logSpy.at(0).at(0).toString().contains("Expected object at top level"_L1));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Missing value \"msg\""_L1));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Invalid messageType received: \"\""_L1));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ContextHandle\""_L1));
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
			QVERIFY(logSpy.at(0).at(0).toString().contains("Expected object at top level"_L1));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Missing value \"msg\""_L1));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Invalid messageType received: \"\""_L1));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ContextHandle\""_L1));
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
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""_L1));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""_L1));
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
			QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \""_L1));
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
