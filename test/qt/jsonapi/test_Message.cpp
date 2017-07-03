/*!
 * \brief Unit tests for \ref MessageDispatcher
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MessageDispatcher.h"

#include "InternalActivationContext.h"
#include "VersionInfo.h"

#include <QSignalSpy>
#include <QtTest>

using namespace governikus;

class test_Message
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
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

			msg = "{\"crap\": \"\"}";
			QByteArray expected("{\"error\":\"Command cannot be undefined\",\"msg\":\"INVALID\"}");
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

			QByteArray msg("{\"cmd\": \"UnknownRequestedCommand321\", \"request\": \"abc123\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"UnknownRequestedCommand321\",\"msg\":\"UNKNOWN_COMMAND\",\"request\":\"abc123\"}"));

			msg = "{\"cmd\": \"UnknownRequestedCommand321\", \"request\": 987}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"UnknownRequestedCommand321\",\"msg\":\"UNKNOWN_COMMAND\",\"request\":987}"));

			msg = "{\"cmd\": \"nothing\", \"request\": {\"blubb\": \"identifier\", \"something\": null}}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"nothing\",\"msg\":\"UNKNOWN_COMMAND\",\"request\":{\"blubb\":\"identifier\",\"something\":null}}"));
		}


		void noRequestWithUnknowCommand()
		{
			QByteArray msg("{\"cmd\": \"UnknownRequestedCommand123\"}");
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"UnknownRequestedCommand123\",\"msg\":\"UNKNOWN_COMMAND\"}"));
		}


		void info()
		{
			QByteArray msg("{\"cmd\": \"GET_INFO\"}");
			MessageDispatcher dispatcher;
			auto versionInfo = VersionInfo::getInstance().toJson(QJsonDocument::Compact);

			const auto& result = dispatcher.processCommand(msg);
			QVERIFY(result.contains(versionInfo));
			QVERIFY(result.contains("\"VersionInfo\":{"));
			QVERIFY(result.contains("\"msg\":\"INFO\""));
		}


		void createMsgHandlerReader()
		{
			MessageDispatcher dispatcher;
			const auto& msg = dispatcher.createMsgReader("dummy reader");
			QCOMPARE(msg, QByteArray("{\"attached\":false,\"msg\":\"READER\",\"name\":\"dummy reader\"}"));
		}


		void processStateEstablishPacePin()
		{
			MessageDispatcher dispatcher;
			dispatcher.init(QSharedPointer<WorkflowContext>(new WorkflowContext()));

			const auto& msg = dispatcher.processStateChange(QStringLiteral("StateEstablishPacePin"));
			QCOMPARE(msg, QByteArray("{\"msg\":\"ENTER_PIN\"}"));
		}


		void processStateApprovedOfUnhandledState()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			MessageDispatcher dispatcher;
			dispatcher.init(context);

			QVERIFY(!context->isStateApproved());
			QCOMPARE(dispatcher.processStateChange(QStringLiteral("SomeUnknownState")), QByteArray());
			QVERIFY(context->isStateApproved());
		}


		void processUnhandledStateAndHandledCmd()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			MessageDispatcher dispatcher;
			dispatcher.init(context);

			QVERIFY(!context->isStateApproved());
			QCOMPARE(dispatcher.processStateChange(QStringLiteral("SomeUnknownState")), QByteArray());
			QVERIFY(context->isStateApproved());
			context->setStateApproved(false); // reset

			auto msg = QByteArray("{\"cmd\":\"SET_CAN\",\"value\": \"12345\"}");
			auto expectedBadState = QByteArray("{\"error\":\"SET_CAN\",\"msg\":\"BAD_STATE\"}");
			QCOMPARE(dispatcher.processCommand(msg), expectedBadState);

			QVERIFY(!dispatcher.processStateChange(QStringLiteral("StateEstablishPaceCan")).isEmpty());
			QVERIFY(!context->isStateApproved());

			auto expectedEnterCan = QByteArray("{\"error\":\"You must provide 6 digits\",\"msg\":\"ENTER_CAN\"}");
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
			MessageDispatcher dispatcher;
			dispatcher.init(QSharedPointer<WorkflowContext>(new WorkflowContext()));

			const auto& msg = dispatcher.processStateChange(QString());
			QCOMPARE(msg, QByteArray("{\"error\":\"Unexpected condition\",\"msg\":\"INTERNAL_ERROR\"}"));
		}


		void acceptResultsInBadState()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			MessageDispatcher dispatcher;
			dispatcher.init(context);

			QVERIFY(!context->isStateApproved());
			QByteArray msg = "{\"cmd\": \"ACCEPT\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"ACCEPT\",\"msg\":\"BAD_STATE\"}"));
			QVERIFY(!context->isStateApproved());
		}


		void cancelResultsInBadState()
		{
			MessageDispatcher dispatcher;
			QByteArray msg = "{\"cmd\": \"CANCEL\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"CANCEL\",\"msg\":\"BAD_STATE\"}"));
		}


		void cancel()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			QSignalSpy spy(context.data(), &WorkflowContext::fireCancelWorkflow);

			MessageDispatcher dispatcher;
			dispatcher.init(context);

			QVERIFY(!context->isStateApproved());
			QByteArray msg = "{\"cmd\": \"CANCEL\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
			QVERIFY(!context->isStateApproved());

			QCOMPARE(spy.count(), 1);
		}


		void finishAuthContext()
		{
			const QSharedPointer<AuthContext> context(new AuthContext(new InternalActivationContext(QUrl("http://dummy"))));
			context->setStatus(GlobalStatus::Code::No_Error);
			context->setRefreshUrl(QUrl("http://dummy"));
			MessageDispatcher dispatcher;
			dispatcher.init(context);

			QCOMPARE(dispatcher.finish(), QByteArray("{\"msg\":\"AUTH\",\"result\":{\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\"},\"url\":\"http://dummy\"}"));
		}


		void finishWorkflowContext()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			MessageDispatcher dispatcher;
			dispatcher.init(context);

			QCOMPARE(dispatcher.finish(), QByteArray());
		}


};

QTEST_GUILESS_MAIN(test_Message)
#include "test_Message.moc"
