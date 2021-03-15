/*!
 * \brief Unit tests for \ref MsgHandlerEnterNewPin
 *
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandlerEnterNewPin.h"

#include "MsgHandlerEnterPassword.h"

#include "context/ChangePinContext.h"
#include "MessageDispatcher.h"
#include "ReaderManager.h"

#include "MockReaderManagerPlugIn.h"

#include <QtPlugin>
#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;


class test_MsgHandlerEnterNewPin
	: public QObject
{
	Q_OBJECT

	static void setValidPinState(MessageDispatcher& pDispatcher,
			bool pSelectReader = true,
			bool pBasicReader = true,
			const PacePasswordId pPasswordID = PacePasswordId::PACE_PIN,
			const QLatin1String& pState = QLatin1String("StateEnterNewPacePin"))
	{
		setValidState(pDispatcher, pSelectReader, pBasicReader, pPasswordID, pState,
				QSharedPointer<ChangePinContext>::create());
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


		void notInserted()
		{
			MessageDispatcher dispatcher;
			setValidPinState(dispatcher, false);

			const QByteArray msg(R"({"cmd": "SET_NEW_PIN"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"No card inserted","msg":"ENTER_NEW_PIN"})"));
		}


		void undefined()
		{
			MessageDispatcher dispatcher;
			setValidPinState(dispatcher);

			const QByteArray msg(R"({"cmd": "SET_NEW_PIN"})");
			QCOMPARE(dispatcher.processCommand(msg), addReaderData(R"({"error":"Value cannot be undefined","msg":"ENTER_NEW_PIN"})"));
		}


		void invalid()
		{
			MessageDispatcher dispatcher;
			setValidPinState(dispatcher);

			const QByteArray msg(R"({"cmd": "SET_NEW_PIN", "value": 123456})");
			QCOMPARE(dispatcher.processCommand(msg), addReaderData(R"({"error":"Invalid value","msg":"ENTER_NEW_PIN"})"));
		}


		void badInput()
		{
			MessageDispatcher dispatcher;
			setValidPinState(dispatcher);

			QByteArray msg(R"({"cmd": "SET_NEW_PIN", "value": "12345"})");
			const QByteArray expected(addReaderData(R"({"error":"You must provide 6 digits","msg":"ENTER_NEW_PIN"})"));
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_NEW_PIN", "value": "1234567"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_NEW_PIN", "value": "abcdef"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_NEW_PIN", "value": ""})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_NEW_PIN", "value": "123456a"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "SET_NEW_PIN", "value": "12x456"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void badState()
		{
			MessageDispatcher dispatcher;
			setValidPinState(dispatcher, true, true, PacePasswordId::UNKNOWN, QLatin1String("StateEnterPacePassword"));

			QByteArray msg(R"({"cmd": "SET_NEW_PIN", "value": "12345"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"SET_NEW_PIN","msg":"BAD_STATE"})"));

			msg = R"({"cmd": "SET_NEW_PIN", "value": "123456"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"SET_NEW_PIN","msg":"BAD_STATE"})"));
		}


		void noDirectResponseIfPinLooksValid()
		{
			MessageDispatcher dispatcher;
			setValidPinState(dispatcher);

			const QByteArray msg(R"({"cmd": "SET_NEW_PIN", "value": "123456"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
		}


		void pinPadReader()
		{
			MessageDispatcher dispatcher;
			setValidPinState(dispatcher, true, false);

			QByteArray msg(R"({"cmd": "SET_NEW_PIN", "value": "111111"})");
			QByteArray expected(addReaderData(R"({"error":"Value cannot be defined","msg":"ENTER_NEW_PIN"})", true));
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = QByteArray(R"({"cmd": "SET_NEW_PIN", "value": ""})");
			expected = QByteArray(addReaderData(R"({"error":"Value cannot be defined","msg":"ENTER_NEW_PIN"})", true));
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = QByteArray(R"({"cmd": "SET_NEW_PIN"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerEnterNewPin)
#include "test_MsgHandlerEnterNewPin.moc"
