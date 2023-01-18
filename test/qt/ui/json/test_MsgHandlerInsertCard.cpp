/*!
 * \brief Unit tests for \ref MsgHandlerInsertCard
 *
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandlerInsertCard.h"

#include "MessageDispatcher.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"
#include "states/StateSelectReader.h"

#include "TestWorkflowContext.h"

#include <QTest>
#include <QtPlugin>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;

class test_MsgHandlerInsertCard
	: public QObject
{
	Q_OBJECT

	static void setContext(MessageDispatcher& pDispatcher)
	{
		QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
		QCOMPARE(pDispatcher.init(context), MsgType::VOID);
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


		void ctor()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			QCOMPARE(context->isStateApproved(), false);

			MsgDispatcherContext ctx;
			ctx.setWorkflowContext(context);
			MsgHandlerInsertCard msg(ctx);

			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"INSERT_CARD\"}"));
			QCOMPARE(context->isStateApproved(), true);
		}


		void noReader()
		{
			MessageDispatcher dispatcher;
			setContext(dispatcher);

			QCOMPARE(dispatcher.processStateChange(AbstractState::getClassName<StateSelectReader>()), QByteArray("{\"msg\":\"INSERT_CARD\"}"));
		}


		void readerWithoutCard()
		{
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 1");
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 2");
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 3");

			MessageDispatcher dispatcher;
			setContext(dispatcher);

			QCOMPARE(dispatcher.processStateChange(AbstractState::getClassName<StateSelectReader>()), QByteArray("{\"msg\":\"INSERT_CARD\"}"));
		}


		void readerWithCard()
		{
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 1");
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader CARD");
			reader->setCard(MockCardConfig());

			MessageDispatcher dispatcher;
			setContext(dispatcher);

			QCOMPARE(dispatcher.processStateChange(AbstractState::getClassName<StateSelectReader>()), QByteArray());
		}


		void setCard()
		{
			MessageDispatcher dispatcher;
			setContext(dispatcher);
			QCOMPARE(dispatcher.processStateChange(AbstractState::getClassName<StateSelectReader>()), QByteArray(R"({"msg":"INSERT_CARD"})"));

			QByteArray msg(R"({"cmd": "SET_API_LEVEL", "level": 1})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"current":1,"msg":"API_LEVEL"})"));

			msg = R"({"cmd": "SET_CARD", "name": "dummy"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"SET_CARD","msg":"UNKNOWN_COMMAND"})"));

			msg = (R"({"cmd": "SET_API_LEVEL", "level": 2})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"current":2,"msg":"API_LEVEL"})"));

			msg = R"({"cmd": "SET_CARD"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"Name cannot be undefined","msg":"INSERT_CARD"})"));

			msg = R"({"cmd": "SET_CARD", "name": false})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"Invalid name","msg":"INSERT_CARD"})"));

			MockReaderManagerPlugIn::getInstance().addReader("MockReaderNfc", ReaderManagerPlugInType::NFC);

			msg = R"({"cmd": "SET_CARD", "name": "dummy"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"Unknown reader name","msg":"INSERT_CARD"})"));

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReaderSmart", ReaderManagerPlugInType::SMART);

			msg = R"({"cmd": "SET_CARD", "name": "MockReaderSmart"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"Card is not insertable","msg":"INSERT_CARD"})"));

			auto info = reader->getReaderInfo();
			info.setCardInfo(CardInfo(CardType::EID_CARD));
			info.shelveCard();
			reader->setReaderInfo(info);

			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
		}


		void simulator()
		{
			MessageDispatcher dispatcher;
			setContext(dispatcher);
			QCOMPARE(dispatcher.processStateChange(AbstractState::getClassName<StateSelectReader>()), QByteArray(R"({"msg":"INSERT_CARD"})"));

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReaderSimulator", ReaderManagerPlugInType::SIMULATOR);
			auto info = reader->getReaderInfo();
			info.setCardInfo(CardInfo(CardType::EID_CARD));
			info.shelveCard();
			reader->setReaderInfo(info);

			QByteArray msg = R"({"cmd": "SET_CARD", "name": "MockReaderSimulator", "simulator": []})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"Parameter 'simulator' is invalid","msg":"INSERT_CARD"})"));

			msg = R"({"cmd": "SET_CARD", "name": "MockReaderSimulator", "simulator": "crap"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"Parameter 'simulator' is invalid","msg":"INSERT_CARD"})"));

			msg = R"({"cmd": "SET_CARD", "name": "MockReaderSimulator", "simulator": null})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"Parameter 'simulator' is invalid","msg":"INSERT_CARD"})"));

			msg = R"({"cmd": "SET_CARD", "name": "MockReaderSimulator", "simulator": {}})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			msg = R"({"cmd": "SET_CARD", "name": "MockReaderSimulator"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerInsertCard)
#include "test_MsgHandlerInsertCard.moc"
