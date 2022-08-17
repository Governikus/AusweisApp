/*!
 * \brief Unit tests for \ref MsgHandlerApiLevel
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandlerApiLevel.h"

#include "MessageDispatcher.h"

#include <QtTest>

using namespace governikus;

class test_MsgHandlerApiLevel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void available()
		{
			MsgContext context;
			context.setApiLevel(MsgLevel::v1);
			MsgHandlerApiLevel msg(qAsConst(context));
			QCOMPARE(msg.toJson(), QByteArray("{\"available\":[1,2],\"current\":1,\"msg\":\"API_LEVEL\"}"));
		}


		void latest()
		{
			MsgContext context;
			context.setApiLevel(MsgHandler::DEFAULT_MSG_LEVEL);
			MsgHandlerApiLevel msg(qAsConst(context));
			QCOMPARE(msg.toJson(), QByteArray("{\"available\":[1,2],\"current\":2,\"msg\":\"API_LEVEL\"}"));
		}


		void asCmd()
		{
			MessageDispatcher dispatcher;
			QByteArray msg = R"({"cmd": "GET_API_LEVEL"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"available\":[1,2],\"current\":2,\"msg\":\"API_LEVEL\"}"));
		}


		void setLevelWithoutLevelEntry()
		{
			QByteArray msg(R"({"cmd": "SET_API_LEVEL"})");
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"current\":2,\"error\":\"Level cannot be undefined\",\"msg\":\"API_LEVEL\"}"));
		}


		void setInvalidLevel()
		{
			MessageDispatcher dispatcher;
			QByteArray expected(R"({"current":2,"error":"Invalid level","msg":"API_LEVEL"})");

			QByteArray msg(R"({"cmd": "SET_API_LEVEL", "level": ""})");
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_API_LEVEL", "level": "crap"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_API_LEVEL", "level": null})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_API_LEVEL", "level": true})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_API_LEVEL", "level": false})";
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void setUnknownLevel()
		{
			MessageDispatcher dispatcher;
			QByteArray expected(R"({"current":2,"error":"Unknown level","msg":"API_LEVEL"})");

			QByteArray msg(R"({"cmd": "SET_API_LEVEL", "level": -1})");
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_API_LEVEL", "level": 0})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_API_LEVEL", "level": 99999999})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_API_LEVEL", "level": 0.1})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_API_LEVEL", "level": 1.1})";
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void setValidLevel()
		{
			QByteArray msg(R"({"cmd": "SET_API_LEVEL", "level": 1})");
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"current\":1,\"msg\":\"API_LEVEL\"}"));
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerApiLevel)
#include "test_MsgHandlerApiLevel.moc"
