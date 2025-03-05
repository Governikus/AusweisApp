/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "MessageDispatcher.h"
#include "MockReaderManagerPlugin.h"
#include "ReaderManager.h"
#include "states/StateSelectReader.h"
#include "states/StateUnfortunateCardPosition.h"

#include "TestWorkflowContext.h"

#include <QSignalSpy>
#include <QTest>
#include <QtPlugin>

Q_IMPORT_PLUGIN(MockReaderManagerPlugin)

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_MsgHandlerPause
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
			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void unfortunateCardPosition()
		{
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);

			QByteArray msg(R"({"cmd": "SET_API_LEVEL", "level": 1})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"current":1,"msg":"API_LEVEL"})"));

			context->setCurrentState(StateBuilder::generateStateName<StateSelectReader>());
			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateUnfortunateCardPosition>()), QByteArray());

			context->setCurrentState(StateBuilder::generateStateName<StateUnfortunateCardPosition>());
			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateUnfortunateCardPosition>()), QByteArray());

			msg = (R"({"cmd": "SET_API_LEVEL", "level": 2})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"current":2,"msg":"API_LEVEL"})"));

			context->setCurrentState(StateBuilder::generateStateName<StateSelectReader>());
			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateUnfortunateCardPosition>()), QByteArray());

			context->setCurrentState(StateBuilder::generateStateName<StateUnfortunateCardPosition>());
			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateUnfortunateCardPosition>()), QByteArray());

			msg = (R"({"cmd": "SET_API_LEVEL", "level": 3})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"current":3,"msg":"API_LEVEL"})"));

			context->setCurrentState(StateBuilder::generateStateName<StateSelectReader>());
			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateUnfortunateCardPosition>()), QByteArray(R"({"cause":"BadCardPosition","msg":"PAUSE"})"));

			context->setCurrentState(StateBuilder::generateStateName<StateUnfortunateCardPosition>());
			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateUnfortunateCardPosition>()), QByteArray(R"({"cause":"BadCardPosition","msg":"PAUSE"})"));

			context->setCurrentState(StateBuilder::generateStateName<StateSelectReader>());
			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateSelectReader>()), QByteArray(R"({"msg":"INSERT_CARD"})"));
			QCOMPARE(QByteArray(dispatcher.processCommand(QByteArray(R"({"cmd":"CONTINUE"})"))), QByteArray(R"({"error":"CONTINUE","msg":"BAD_STATE"})"));

			context->setCurrentState(StateBuilder::generateStateName<StateUnfortunateCardPosition>());
			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateUnfortunateCardPosition>()), QByteArray(R"({"cause":"BadCardPosition","msg":"PAUSE"})"));
			QCOMPARE(QByteArray(dispatcher.processCommand(QByteArray(R"({"cmd":"CONTINUE"})"))), QByteArray());
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerPause)
#include "test_MsgHandlerPause.moc"
