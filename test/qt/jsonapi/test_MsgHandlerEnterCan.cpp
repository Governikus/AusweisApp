/*!
 * \brief Unit tests for \ref MsgHandlerEnterCan
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandlerEnterCan.h"

#include "MsgHandlerEnterPassword.h"

#include "MessageDispatcher.h"
#include "ReaderManager.h"

#include "MockReaderManagerPlugIn.h"

#include <QtPlugin>
#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;

class test_MsgHandlerEnterCan
	: public QObject
{
	Q_OBJECT

	static void setValidCanState(MessageDispatcher& pDispatcher,
			bool pSelectReader = true,
			bool pBasicReader = true,
			const PacePasswordId pPasswordID = PacePasswordId::PACE_CAN)
	{
		setValidState(pDispatcher, pSelectReader, pBasicReader, pPasswordID);
	}


	private Q_SLOTS:
		void initTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->init();
			readerManager->getPlugInInfos(); // just to wait until initialization finished
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void cleanup()
		{
			MockReaderManagerPlugIn::getInstance().removeAllReader();
		}


		void undefined()
		{
			MessageDispatcher dispatcher;
			setValidCanState(dispatcher);

			const QByteArray msg(R"({"cmd": "SET_CAN"})");
			QCOMPARE(dispatcher.processCommand(msg), addReaderData(R"({"error":"Value cannot be undefined","msg":"ENTER_CAN"})"));
		}


		void invalid()
		{
			MessageDispatcher dispatcher;
			setValidCanState(dispatcher);

			const QByteArray msg(R"({"cmd": "SET_CAN", "value": 123456})");
			QCOMPARE(dispatcher.processCommand(msg), addReaderData(R"({"error":"Invalid value","msg":"ENTER_CAN"})"));
		}


		void badInput()
		{
			MessageDispatcher dispatcher;
			setValidCanState(dispatcher);

			QByteArray msg(R"({"cmd": "SET_CAN", "value": "12345"})");
			const QByteArray expected(addReaderData(R"({"error":"You must provide 6 digits","msg":"ENTER_CAN"})"));
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_CAN", "value": "1234567"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_CAN", "value": "abcdef"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_CAN", "value": ""})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_CAN", "value": "123456a"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_CAN", "value": "12x456"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void badState()
		{
			MessageDispatcher dispatcher;
			setValidCanState(dispatcher, true, true, PacePasswordId::UNKNOWN);

			QByteArray msg(R"({"cmd": "SET_CAN", "value": "12345"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"SET_CAN","msg":"BAD_STATE"})"));

			msg = R"({"cmd": "SET_CAN", "value": "123456"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"SET_CAN","msg":"BAD_STATE"})"));
		}


		void noDirectResponseIfCanLooksValid()
		{
			MessageDispatcher dispatcher;
			setValidCanState(dispatcher);

			const QByteArray msg(R"({"cmd": "SET_CAN", "value": "123456"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
		}


		void pinPadReader()
		{
			MessageDispatcher dispatcher;
			setValidCanState(dispatcher, true, false);

			QByteArray msg(R"({"cmd": "SET_CAN", "value": "111111"})");
			QByteArray expected(addReaderData(R"({"error":"Value cannot be defined","msg":"ENTER_CAN"})", true));
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = QByteArray(R"({"cmd": "SET_CAN", "value": ""})");
			expected = QByteArray(addReaderData(R"({"error":"Value cannot be defined","msg":"ENTER_CAN"})", true));
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = QByteArray(R"({"cmd": "SET_CAN"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerEnterCan)
#include "test_MsgHandlerEnterCan.moc"
