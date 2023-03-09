/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref MsgHandlerEnterNewPin
 */

#include "messages/MsgHandlerEnterNewPin.h"

#include "MsgHandlerEnterPassword.h"

#include "MessageDispatcher.h"
#include "ReaderManager.h"
#include "context/ChangePinContext.h"

#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
#endif

#include "MockReaderManagerPlugIn.h"

#include <QtPlugin>
#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;

Q_DECLARE_METATYPE(QSharedPointer<WorkflowContext>)


class test_MsgHandlerEnterNewPin
	: public QObject
{
	Q_OBJECT

	static void setValidPinState(MessageDispatcher& pDispatcher,
			bool pSelectReader = true,
			bool pBasicReader = true,
			const PacePasswordId pPasswordID = PacePasswordId::PACE_PIN,
			const QLatin1String& pState = QLatin1String("StateEnterNewPacePin"),
			const QSharedPointer<WorkflowContext> pContext = QSharedPointer<ChangePinContext>::create())
	{
		setValidState(pDispatcher, pSelectReader, pBasicReader, pPasswordID, pState, pContext);
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


		void noDirectResponseIfPinLooksValid_data()
		{
			QTest::addColumn<QSharedPointer<WorkflowContext>>("ctx");

			QTest::newRow("ChangePinContext") << QSharedPointer<ChangePinContext>::create().objectCast<WorkflowContext>();

#if __has_include("context/PersonalizationContext.h")
			QTest::newRow("PersonalizationContext") << QSharedPointer<PersonalizationContext>::create(QString("dummy")).objectCast<WorkflowContext>();
#endif
		}


		void noDirectResponseIfPinLooksValid()
		{
			QFETCH(QSharedPointer<WorkflowContext>, ctx);

			MessageDispatcher dispatcher;
			setValidPinState(dispatcher, true, true, PacePasswordId::PACE_PIN, QLatin1String("StateEnterNewPacePin"), ctx);

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
