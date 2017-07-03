/*!
 * \brief Unit tests for \ref MsgHandlerEnterCan
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "messages/MsgHandlerEnterCan.h"

#include "MessageDispatcher.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"

#include <QTest>
#include <QtPlugin>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;

class test_MsgHandlerEnterCan
	: public QObject
{
	Q_OBJECT

	static void setValidCanState(MessageDispatcher& pDispatcher, const QString& pState = QStringLiteral("StateEstablishPaceCan"))
	{
		QSharedPointer<WorkflowContext> context(new WorkflowContext());
		pDispatcher.init(context);

		QByteArray expected;
		if (pState == QLatin1String("StateEstablishPaceCan"))
		{
			expected = "{\"msg\":\"ENTER_CAN\"}";
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
			setValidCanState(dispatcher);

			QByteArray msg("{\"cmd\": \"SET_CAN\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Value cannot be undefined\",\"msg\":\"ENTER_CAN\"}"));
		}


		void invalid()
		{
			MessageDispatcher dispatcher;
			setValidCanState(dispatcher);

			QByteArray msg("{\"cmd\": \"SET_CAN\", \"can\": 123456}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Invalid value\",\"msg\":\"ENTER_CAN\"}"));
		}


		void badInput()
		{
			MessageDispatcher dispatcher;
			setValidCanState(dispatcher);

			QByteArray msg("{\"cmd\": \"SET_CAN\", \"can\": \"12345\"}");
			QByteArray expected("{\"error\":\"You must provide 6 digits\",\"msg\":\"ENTER_CAN\"}");
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"SET_CAN\", \"can\": \"1234567\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"SET_CAN\", \"can\": \"abcdef\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"SET_CAN\", \"can\": \"\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"SET_CAN\", \"can\": \"123456a\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"SET_CAN\", \"can\": \"12x456\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void badState()
		{
			MessageDispatcher dispatcher;
			setValidCanState(dispatcher, QStringLiteral("invalid"));

			QByteArray msg("{\"cmd\": \"SET_CAN\", \"can\": \"12345\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"SET_CAN\",\"msg\":\"BAD_STATE\"}"));

			msg = "{\"cmd\": \"SET_CAN\", \"can\": \"123456\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"SET_CAN\",\"msg\":\"BAD_STATE\"}"));
		}


		void noDirectResponseIfCanLooksValid()
		{
			MessageDispatcher dispatcher;
			setValidCanState(dispatcher);

			QByteArray msg("{\"cmd\": \"SET_CAN\", \"can\": \"123456\"}");
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
			QCOMPARE(dispatcher.processStateChange("StateEstablishPaceCan"), QByteArray("{\"msg\":\"ENTER_CAN\"}"));
			QByteArray msg = "{\"cmd\": \"SET_CAN\", \"can\": \"54321\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"You must provide 6 digits\",\"msg\":\"ENTER_CAN\"}"));


			context->setReaderName("MockReader CARD");
			QCOMPARE(dispatcher.processStateChange("StateEstablishPaceCan"), QByteArray("{\"msg\":\"ENTER_CAN\",\"reader\":{\"attached\":true,\"card\":{\"deactivated\":false,\"inserted\":true,\"retryCounter\":-1},\"name\":\"MockReader CARD\"}}"));
			msg = "{\"cmd\": \"SET_CAN\", \"can\": \"54321\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"You must provide 6 digits\",\"msg\":\"ENTER_CAN\",\"reader\":{\"attached\":true,\"card\":{\"deactivated\":false,\"inserted\":true,\"retryCounter\":-1},\"name\":\"MockReader CARD\"}}"));
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerEnterCan)
#include "test_MsgHandlerEnterCan.moc"
