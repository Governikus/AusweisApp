/*!
 * \brief Unit tests for \ref MsgHandlerEnterPin
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandlerEnterPin.h"

#include "MessageDispatcher.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"

#include <QtPlugin>
#include <QTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;

class test_MsgHandlerEnterPin
	: public QObject
{
	Q_OBJECT

	static void setValidPinState(MessageDispatcher& pDispatcher, const QString& pState = QStringLiteral("StateEstablishPacePin"))
	{
		QSharedPointer<WorkflowContext> context(new WorkflowContext());
		pDispatcher.init(context);

		QByteArray expected;
		if (pState == QLatin1String("StateEstablishPacePin"))
		{
			expected = "{\"msg\":\"ENTER_PIN\"}";
		}

		QCOMPARE(pDispatcher.processStateChange(pState), expected);
	}


	private Q_SLOTS:
		void initTestCase()
		{
			ReaderManager::getInstance().init();
			ReaderManager::getInstance().getPlugInInfos(); // just to wait until initialization finished
		}


		void cleanupTestCase()
		{
			ReaderManager::getInstance().shutdown();
		}


		void undefined()
		{
			MessageDispatcher dispatcher;
			setValidPinState(dispatcher);

			QByteArray msg("{\"cmd\": \"SET_PIN\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Value cannot be undefined\",\"msg\":\"ENTER_PIN\"}"));
		}


		void invalid()
		{
			MessageDispatcher dispatcher;
			setValidPinState(dispatcher);

			QByteArray msg("{\"cmd\": \"SET_PIN\", \"value\": 123456}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Invalid value\",\"msg\":\"ENTER_PIN\"}"));
		}


		void badInput()
		{
			MessageDispatcher dispatcher;
			setValidPinState(dispatcher);

			QByteArray msg("{\"cmd\": \"SET_PIN\", \"value\": \"12345\"}");
			QByteArray expected("{\"error\":\"You must provide 6 digits\",\"msg\":\"ENTER_PIN\"}");
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"SET_PIN\", \"value\": \"1234567\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"SET_PIN\", \"value\": \"abcdef\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"SET_PIN\", \"value\": \"\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"SET_PIN\", \"value\": \"123456a\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"SET_PIN\", \"value\": \"12x456\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void badState()
		{
			MessageDispatcher dispatcher;
			setValidPinState(dispatcher, QStringLiteral("invalid"));

			QByteArray msg("{\"cmd\": \"SET_PIN\", \"value\": \"12345\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"SET_PIN\",\"msg\":\"BAD_STATE\"}"));

			msg = "{\"cmd\": \"SET_PIN\", \"value\": \"123456\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"SET_PIN\",\"msg\":\"BAD_STATE\"}"));
		}


		void noDirectResponseIfPinLooksValid()
		{
			MessageDispatcher dispatcher;
			setValidPinState(dispatcher);

			QByteArray msg("{\"cmd\": \"SET_PIN\", \"value\": \"123456\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
		}


		void readerInfo()
		{
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader CARD");
			reader->setCard(MockCardConfig());

			QSharedPointer<WorkflowContext> context(new WorkflowContext());
			MessageDispatcher dispatcher;
			dispatcher.init(context);

			context->setReaderName("MockReader");
			QCOMPARE(dispatcher.processStateChange("StateEstablishPacePin"), QByteArray("{\"msg\":\"ENTER_PIN\"}"));
			QByteArray msg = "{\"cmd\": \"SET_PIN\", \"value\": \"54321\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"You must provide 6 digits\",\"msg\":\"ENTER_PIN\"}"));


			context->setReaderName("MockReader CARD");
			QCOMPARE(dispatcher.processStateChange("StateEstablishPacePin"), QByteArray("{\"msg\":\"ENTER_PIN\",\"reader\":{\"attached\":true,\"card\":{\"deactivated\":false,\"inoperative\":false,\"retryCounter\":-1},\"name\":\"MockReader CARD\"}}"));
			msg = "{\"cmd\": \"SET_PIN\", \"value\": \"54321\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"You must provide 6 digits\",\"msg\":\"ENTER_PIN\",\"reader\":{\"attached\":true,\"card\":{\"deactivated\":false,\"inoperative\":false,\"retryCounter\":-1},\"name\":\"MockReader CARD\"}}"));
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerEnterPin)
#include "test_MsgHandlerEnterPin.moc"
