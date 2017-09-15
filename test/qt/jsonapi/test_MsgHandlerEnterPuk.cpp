/*!
 * \brief Unit tests for \ref MsgHandlerEnterPuk
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "messages/MsgHandlerEnterPuk.h"

#include "MessageDispatcher.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"

#include <QtPlugin>
#include <QTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;

class test_MsgHandlerEnterPuk
	: public QObject
{
	Q_OBJECT

	static void setValidState(MessageDispatcher& pDispatcher, const QString& pState = QStringLiteral("StateEstablishPacePuk"))
	{
		QSharedPointer<WorkflowContext> context(new WorkflowContext());
		pDispatcher.init(context);

		QByteArray expected;
		if (pState == QLatin1String("StateEstablishPacePuk"))
		{
			expected = "{\"msg\":\"ENTER_PUK\"}";
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


		void stateMsg()
		{
			MessageDispatcher dispatcher;
			setValidState(dispatcher);
		}


		void undefined()
		{
			MessageDispatcher dispatcher;
			setValidState(dispatcher);

			QByteArray msg("{\"cmd\": \"SET_PUK\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Value cannot be undefined\",\"msg\":\"ENTER_PUK\"}"));
		}


		void invalid()
		{
			MessageDispatcher dispatcher;
			setValidState(dispatcher);

			QByteArray msg("{\"cmd\": \"SET_PUK\", \"value\": 12345667890}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Invalid value\",\"msg\":\"ENTER_PUK\"}"));
		}


		void badInput()
		{
			MessageDispatcher dispatcher;
			setValidState(dispatcher);

			QByteArray msg("{\"cmd\": \"SET_PUK\", \"value\": \"123456\"}");
			QByteArray expected("{\"error\":\"You must provide 10 digits\",\"msg\":\"ENTER_PUK\"}");
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"SET_PUK\", \"value\": \"12345\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void badState()
		{
			MessageDispatcher dispatcher;
			setValidState(dispatcher, QStringLiteral("invalid"));

			QByteArray msg("{\"cmd\": \"SET_PUK\", \"value\": \"12345\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"SET_PUK\",\"msg\":\"BAD_STATE\"}"));

			msg = "{\"cmd\": \"SET_PUK\", \"value\": \"123456\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"SET_PUK\",\"msg\":\"BAD_STATE\"}"));
		}


		void noDirectResponseIfPukLooksValid()
		{
			MessageDispatcher dispatcher;
			setValidState(dispatcher);

			QByteArray msg("{\"cmd\": \"SET_PUK\", \"value\": \"1234567890\"}");
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
			QCOMPARE(dispatcher.processStateChange("StateEstablishPacePuk"), QByteArray("{\"msg\":\"ENTER_PUK\"}"));
			QByteArray msg = "{\"cmd\": \"SET_PUK\", \"value\": \"654321\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"You must provide 10 digits\",\"msg\":\"ENTER_PUK\"}"));


			context->setReaderName("MockReader CARD");
			QCOMPARE(dispatcher.processStateChange("StateEstablishPacePuk"), QByteArray("{\"msg\":\"ENTER_PUK\",\"reader\":{\"attached\":true,\"card\":{\"deactivated\":false,\"inoperative\":false,\"retryCounter\":-1},\"name\":\"MockReader CARD\"}}"));
			msg = "{\"cmd\": \"SET_PUK\", \"value\": \"654321\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"You must provide 10 digits\",\"msg\":\"ENTER_PUK\",\"reader\":{\"attached\":true,\"card\":{\"deactivated\":false,\"inoperative\":false,\"retryCounter\":-1},\"name\":\"MockReader CARD\"}}"));
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerEnterPuk)
#include "test_MsgHandlerEnterPuk.moc"
