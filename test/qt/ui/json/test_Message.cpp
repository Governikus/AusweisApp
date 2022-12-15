/*!
 * \brief Unit tests for \ref MessageDispatcher
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "MessageDispatcher.h"

#include "ReaderManager.h"
#include "context/AuthContext.h"
#include "context/InternalActivationContext.h"
#include "states/StateEnterPacePassword.h"

#include "TestWorkflowContext.h"

#include <QSignalSpy>
#include <QtTest>


using namespace governikus;


class test_Message
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<ReaderManager>()->init();
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void invalidCrap()
		{
			QByteArray msg("crap");
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"illegal value (offset: 1)\",\"msg\":\"INVALID\"}"));
		}


		void invalidJsonCrap()
		{
			MessageDispatcher dispatcher;

			QByteArray msg("{crap}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"unterminated object (offset: 2)\",\"msg\":\"INVALID\"}"));

			msg = R"({"crap": ""})";
			QByteArray expected(R"({"error":"Command cannot be undefined","msg":"INVALID"})");
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": false}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": 1.1}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": null}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{}";
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void requestWithUnknownCommand()
		{
			MessageDispatcher dispatcher;

			QByteArray msg(R"({"cmd": "UnknownRequestedCommand321", "request": "abc123"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"UnknownRequestedCommand321\",\"msg\":\"UNKNOWN_COMMAND\",\"request\":\"abc123\"}"));

			msg = R"({"cmd": "UnknownRequestedCommand321", "request": 987})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"UnknownRequestedCommand321\",\"msg\":\"UNKNOWN_COMMAND\",\"request\":987}"));

			msg = R"({"cmd": "nothing", "request": {"blubb": "identifier", "something": null}})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"nothing\",\"msg\":\"UNKNOWN_COMMAND\",\"request\":{\"blubb\":\"identifier\",\"something\":null}}"));
		}


		void noRequestWithUnknowCommand()
		{
			QByteArray msg(R"({"cmd": "UnknownRequestedCommand123"})");
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"UnknownRequestedCommand123\",\"msg\":\"UNKNOWN_COMMAND\"}"));
		}


		void createMsgHandlerReader()
		{
			MessageDispatcher dispatcher;
			const auto& msg = dispatcher.processReaderChange(ReaderInfo("dummy reader"));
			QCOMPARE(msg.size(), 1);
			QCOMPARE(msg.first(), QByteArray(R"({"attached":false,"msg":"READER","name":"dummy reader"})"));
		}


		void processStateEnterPacePassword()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);

			dispatcher.mContext.getContext()->setEstablishPaceChannelType(PacePasswordId::PACE_PIN);
			const auto& msg = dispatcher.processStateChange(AbstractState::getClassName<StateEnterPacePassword>());
			QCOMPARE(msg, QByteArray("{\"msg\":\"ENTER_PIN\"}"));
		}


		void processStateApprovedOfUnhandledState()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);

			QVERIFY(!context->isStateApproved());
			QCOMPARE(dispatcher.processStateChange(QStringLiteral("SomeUnknownState")), QByteArray());
			QVERIFY(context->isStateApproved());
		}


		void processUnhandledStateAndHandledCmd()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);

			context->setReaderName("dummy");
			QVERIFY(!context->isStateApproved());
			QCOMPARE(dispatcher.processStateChange(QStringLiteral("SomeUnknownState")), QByteArray());
			QVERIFY(context->isStateApproved());
			context->setStateApproved(false); // reset

			auto msg = QByteArray(R"({"cmd":"SET_CAN","value": "12345"})");
			auto expectedBadState = QByteArray(R"({"error":"SET_CAN","msg":"BAD_STATE"})");
			QCOMPARE(dispatcher.processCommand(msg), expectedBadState);

			dispatcher.mContext.getContext()->setEstablishPaceChannelType(PacePasswordId::PACE_CAN);
			QVERIFY(!QByteArray(dispatcher.processStateChange(AbstractState::getClassName<StateEnterPacePassword>())).isEmpty());
			QVERIFY(!context->isStateApproved());

			auto expectedEnterCan = QByteArray(R"({"error":"You must provide 6 digits","msg":"ENTER_CAN"})");
			QCOMPARE(dispatcher.processCommand(msg), expectedEnterCan);

			QVERIFY(!context->isStateApproved());
			QCOMPARE(dispatcher.processStateChange(QStringLiteral("SomeUnknownState")), QByteArray());
			QVERIFY(context->isStateApproved());

			QCOMPARE(dispatcher.processCommand(msg), expectedBadState);
		}


		void processStateWithoutContext()
		{
			MessageDispatcher dispatcher;

			const auto& msg = dispatcher.processStateChange(QStringLiteral("SomeState"));
			QCOMPARE(msg, QByteArray("{\"error\":\"Unexpected condition\",\"msg\":\"INTERNAL_ERROR\"}"));
		}


		void processEmptyState()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);

			const auto& msg = dispatcher.processStateChange(QString());
			QCOMPARE(msg, QByteArray("{\"error\":\"Unexpected condition\",\"msg\":\"INTERNAL_ERROR\"}"));
		}


		void acceptResultsInBadState()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);

			QVERIFY(!context->isStateApproved());
			QByteArray msg = R"({"cmd": "ACCEPT"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"ACCEPT\",\"msg\":\"BAD_STATE\"}"));
			QVERIFY(!context->isStateApproved());
		}


		void cancelResultsInBadState()
		{
			MessageDispatcher dispatcher;
			QByteArray msg = R"({"cmd": "CANCEL"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"CANCEL\",\"msg\":\"BAD_STATE\"}"));
		}


		void cancel()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			QSignalSpy spy(context.data(), &WorkflowContext::fireCancelWorkflow);

			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);

			QVERIFY(!context->isStateApproved());
			QVERIFY(!context->isWorkflowCancelled());

			QTest::ignoreMessage(QtDebugMsg, "Process type: CANCEL");
			QTest::ignoreMessage(QtWarningMsg, "Killing the current workflow.");
			const QByteArray msg = R"({"cmd": "CANCEL"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QVERIFY(context->isStateApproved());
			QVERIFY(context->isWorkflowCancelled());
			QCOMPARE(spy.count(), 1);
		}


		void finishAuthContext()
		{
			const QSharedPointer<AuthContext> context(new AuthContext(QSharedPointer<InternalActivationContext>::create(QUrl("http://dummy"))));
			context->setStatus(GlobalStatus::Code::No_Error);
			context->setRefreshUrl(QUrl("http://dummy"));
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::AUTH);

			QCOMPARE(dispatcher.finish(), QByteArray("{\"msg\":\"AUTH\",\"result\":{\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\"},\"url\":\"http://dummy\"}"));
		}


		void finishWorkflowContext()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);

			QCOMPARE(dispatcher.finish(), QByteArray());
		}


};

QTEST_GUILESS_MAIN(test_Message)
#include "test_Message.moc"
