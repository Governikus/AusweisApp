/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandlerStatus.h"

#include "MessageDispatcher.h"
#include "ReaderManager.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"
#include "states/StateConnectCard.h"
#include "states/StateEditAccessRights.h"
#include "states/StateEnterNewPacePin.h"
#include "states/StateEnterPacePassword.h"
#include "states/StateSelectReader.h"

#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
#endif

#include "TestAuthContext.h"
#include "TestWorkflowContext.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_MsgHandlerStatus
	: public QObject
{
	Q_OBJECT

	const QByteArray cmd = QByteArray(R"({"cmd": "GET_STATUS"})");


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


		void apiLevel()
		{
			const QSharedPointer<ChangePinContext> context(new ChangePinContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::CHANGE_PIN);

			const QByteArray supported(R"({"msg":"STATUS","progress":0,"state":null,"workflow":"CHANGE_PIN"})");

			// default
			QCOMPARE(dispatcher.processCommand(cmd), supported);
			QCOMPARE(dispatcher.processProgressChange(), supported);

			// API v1
			QByteArray msg(R"({"cmd": "SET_API_LEVEL", "level": 1})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"current":1,"msg":"API_LEVEL"})"));
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"error":"GET_STATUS","msg":"UNKNOWN_COMMAND"})"));
			QCOMPARE(dispatcher.processProgressChange(), QByteArray());

			// API v2
			msg = (R"({"cmd": "SET_API_LEVEL", "level": 2})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"current":2,"msg":"API_LEVEL"})"));
			QCOMPARE(dispatcher.processCommand(cmd), supported);
			QCOMPARE(dispatcher.processProgressChange(), supported);
		}


		void noWorkflow()
		{
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"msg":"STATUS","progress":null,"state":null,"workflow":null})"));
		}


		void nonSupportedWorkflow()
		{
			const QSharedPointer<SelfAuthContext> context(new SelfAuthContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::AUTH);
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"msg":"STATUS","progress":0,"state":null,"workflow":"UNKNOWN"})"));
		}


		void changePinWorkflow()
		{
			const QSharedPointer<ChangePinContext> context(new ChangePinContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::CHANGE_PIN);
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"msg":"STATUS","progress":0,"state":null,"workflow":"CHANGE_PIN"})"));
		}


		void personalizationWorkflow()
		{
#if __has_include("context/PersonalizationContext.h")
			const auto& context = QSharedPointer<PersonalizationContext>::create(QString());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::PERSONALIZATION);
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"msg":"STATUS","progress":0,"state":null,"workflow":"PERSONALIZATION"})"));
#endif
		}


		void authWorkflow()
		{
			MessageDispatcher dispatcher;
			setContext(dispatcher);
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"msg":"STATUS","progress":0,"state":null,"workflow":"AUTH"})"));
		}


		void workflowProgress()
		{
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);

			const QByteArray zero(R"({"msg":"STATUS","progress":0,"state":null,"workflow":"AUTH"})");
			QCOMPARE(dispatcher.processCommand(cmd), zero);
			QCOMPARE(dispatcher.processProgressChange(), zero);

			context->setProgress(50, QString());
			const QByteArray half(R"({"msg":"STATUS","progress":50,"state":null,"workflow":"AUTH"})");
			QCOMPARE(dispatcher.processCommand(cmd), half);
			QCOMPARE(dispatcher.processProgressChange(), half);
		}


		void stateAccessRights()
		{
			const QSharedPointer<TestAuthContext> context(new TestAuthContext(":/paos/DIDAuthenticateEAC1.xml"_L1));

			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::AUTH);

			QVERIFY(QByteArray(dispatcher.processStateChange(StateBuilder::generateStateName<StateEditAccessRights>())).contains(QByteArray(R"("msg":"ACCESS_RIGHTS")")));
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"msg":"STATUS","progress":0,"state":"ACCESS_RIGHTS","workflow":"AUTH"})"));
		}


		void statePin()
		{
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			context->setEstablishPaceChannelType(PacePasswordId::PACE_PIN);
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);

			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateEnterPacePassword>()), QByteArray(R"({"msg":"ENTER_PIN"})"));
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"msg":"STATUS","progress":0,"state":"ENTER_PIN","workflow":"AUTH"})"));
		}


		void stateNewPin()
		{
			const QSharedPointer<ChangePinContext> context(new ChangePinContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::CHANGE_PIN);

			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateEnterNewPacePin>()), QByteArray(R"({"msg":"ENTER_NEW_PIN"})"));
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"msg":"STATUS","progress":0,"state":"ENTER_NEW_PIN","workflow":"CHANGE_PIN"})"));
		}


		void stateCan()
		{
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			context->setEstablishPaceChannelType(PacePasswordId::PACE_CAN);
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);

			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateEnterPacePassword>()), QByteArray(R"({"msg":"ENTER_CAN"})"));
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"msg":"STATUS","progress":0,"state":"ENTER_CAN","workflow":"AUTH"})"));
		}


		void statePuk()
		{
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			context->setEstablishPaceChannelType(PacePasswordId::PACE_PUK);
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);

			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateEnterPacePassword>()), QByteArray(R"({"msg":"ENTER_PUK"})"));
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"msg":"STATUS","progress":0,"state":"ENTER_PUK","workflow":"AUTH"})"));
		}


		void stateInsertCard()
		{
			MessageDispatcher dispatcher;
			setContext(dispatcher);

			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateSelectReader>()), QByteArray(R"({"msg":"INSERT_CARD"})"));
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"msg":"STATUS","progress":0,"state":"INSERT_CARD","workflow":"AUTH"})"));

			QCOMPARE(dispatcher.processStateChange(StateBuilder::generateStateName<StateConnectCard>()), QByteArray());
			QCOMPARE(dispatcher.processCommand(cmd), QByteArray(R"({"msg":"STATUS","progress":0,"state":null,"workflow":"AUTH"})"));
		}


		void action_data()
		{
			QTest::addColumn<Action>("action");
			QTest::addColumn<QString>("name");

			QTest::newRow("AUTH") << Action::AUTH << QStringLiteral("AUTH");
			QTest::newRow("PIN") << Action::PIN << QStringLiteral("CHANGE_PIN");
			QTest::newRow("PERSONALIZATION") << Action::PERSONALIZATION << QStringLiteral("PERSONALIZATION");
			QTest::newRow("SELF") << Action::SELF << QStringLiteral("UNKNOWN");
			QTest::newRow("REMOTE_SERVICE") << Action::REMOTE_SERVICE << QStringLiteral("UNKNOWN");

			QCOMPARE(Enum<Action>::getCount(), 5);
		}


		void action()
		{
			QFETCH(Action, action);
			QFETCH(QString, name);

			MsgDispatcherContext ctx;
			ctx.setWorkflowContext(QSharedPointer<TestWorkflowContext>::create(false, action));
			MsgHandlerStatus handler(ctx);
			QCOMPARE(handler.getWorkflow(ctx), QJsonValue(name));
		}


};


QTEST_GUILESS_MAIN(test_MsgHandlerStatus)
#include "test_MsgHandlerStatus.moc"
