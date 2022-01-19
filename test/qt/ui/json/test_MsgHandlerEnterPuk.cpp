/*!
 * \brief Unit tests for \ref MsgHandlerEnterPuk
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandlerEnterPuk.h"

#include "MsgHandlerEnterPassword.h"

#include "MessageDispatcher.h"
#include "ReaderManager.h"

#include "MockReaderManagerPlugIn.h"

#include <QtPlugin>
#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;

class test_MsgHandlerEnterPuk
	: public QObject
{
	Q_OBJECT

	static void setValidPukState(MessageDispatcher& pDispatcher,
			bool pSelectReader = true,
			bool pBasicReader = true,
			const PacePasswordId pPasswordID = PacePasswordId::PACE_PUK)
	{
		setValidState(pDispatcher, pSelectReader, pBasicReader, pPasswordID);
	}

	private Q_SLOTS:
		void initTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->init();
			readerManager->isScanRunning(); // just to wait until initialization finished
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void cleanup()
		{
			MockReaderManagerPlugIn::getInstance().removeAllReader();
		}


		void stateMsg()
		{
			MessageDispatcher dispatcher;
			setValidPukState(dispatcher);
		}


		void undefined()
		{
			MessageDispatcher dispatcher;
			setValidPukState(dispatcher);

			const QByteArray msg(R"({"cmd": "SET_PUK"})");
			QCOMPARE(dispatcher.processCommand(msg), addReaderData(R"({"error":"Value cannot be undefined","msg":"ENTER_PUK"})"));
		}


		void invalid()
		{
			MessageDispatcher dispatcher;
			setValidPukState(dispatcher);

			const QByteArray msg(R"({"cmd": "SET_PUK", "value": 12345667890})");
			QCOMPARE(dispatcher.processCommand(msg), addReaderData(R"({"error":"Invalid value","msg":"ENTER_PUK"})"));
		}


		void badInput()
		{
			MessageDispatcher dispatcher;
			setValidPukState(dispatcher);

			QByteArray msg(R"({"cmd": "SET_PUK", "value": "123456"})");
			const QByteArray expected(addReaderData(R"({"error":"You must provide 10 digits","msg":"ENTER_PUK"})"));
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = (R"({"cmd": "SET_PUK", "value": "12345"})");
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void badState()
		{
			MessageDispatcher dispatcher;
			setValidPukState(dispatcher, true, true, PacePasswordId::UNKNOWN);

			QByteArray msg(R"({"cmd": "SET_PUK", "value": "123456789"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"SET_PUK\",\"msg\":\"BAD_STATE\"}"));

			msg = (R"({"cmd": "SET_PUK", "value": "1234567890"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"SET_PUK\",\"msg\":\"BAD_STATE\"}"));
		}


		void noDirectResponseIfPukLooksValid()
		{
			MessageDispatcher dispatcher;
			setValidPukState(dispatcher);

			const QByteArray msg(R"({"cmd": "SET_PUK", "value": "1234567890"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
		}


		void pinPadReader()
		{
			MessageDispatcher dispatcher;
			setValidPukState(dispatcher, true, false);

			QByteArray msg(R"({"cmd": "SET_PUK", "value": "1234567890"})");
			QByteArray expected(addReaderData(R"({"error":"Value cannot be defined","msg":"ENTER_PUK"})", true));
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = QByteArray(R"({"cmd": "SET_PUK", "value": ""})");
			expected = QByteArray(addReaderData(R"({"error":"Value cannot be defined","msg":"ENTER_PUK"})", true));
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = QByteArray(R"({"cmd": "SET_PUK"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerEnterPuk)
#include "test_MsgHandlerEnterPuk.moc"
