/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#include "controller/AppController.h"

#include "context/ChangePinContext.h"
#include "context/RemoteServiceContext.h"
#include "controller/ChangePinController.h"
#include "controller/RemoteServiceController.h"

#include "MockActivationContext.h"

#include <QSharedPointer>
#include <QtTest>


using namespace governikus;


class test_AppController
	: public QObject
{
	Q_OBJECT
	QSharedPointer<AppController> mController;

	private Q_SLOTS:
		void init()
		{
			mController.reset(new AppController());
		}


		void cleanup()
		{
			mController.clear();
		}


		void test_StartNewWorkflow()
		{
			QSignalSpy spy(mController.data(), &AppController::fireWorkflowStarted);
			const QSharedPointer<ChangePinContext> context(new ChangePinContext());
			QTest::ignoreMessage(QtInfoMsg, "Starting new workflow PIN");
			QTest::ignoreMessage(QtDebugMsg, "Start governikus::ChangePinController");
			QVERIFY(mController->startNewWorkflow<ChangePinController>(Action::PIN, context));
			QCOMPARE(mController->mActiveController->getContext(), context);
			QCOMPARE(mController->mCurrentAction, Action::PIN);
			QCOMPARE(spy.count(), 1);

			QTest::ignoreMessage(QtWarningMsg, "Cannot start governikus::ChangePinController | Current action:  PIN");
			QVERIFY(!mController->startNewWorkflow<ChangePinController>(Action::PIN, context));
		}


		void test_OnWorkflowFinishedNoWaitingRequest()
		{
			QSignalSpy spyWorkflowFinished(mController.data(), &AppController::fireWorkflowFinished);

			mController->onRemoteServiceRequested();
			QTest::ignoreMessage(QtDebugMsg, "governikus::RemoteServiceController done");
			QTest::ignoreMessage(QtInfoMsg, "Finished workflow REMOTE_SERVICE");
			mController->onWorkflowFinished();
			QCOMPARE(spyWorkflowFinished.count(), 1);
		}


		void test_OnWorkflowFinishedPIN()
		{
			QSignalSpy spyWorkflowFinished(mController.data(), &AppController::fireWorkflowFinished);

			mController->onChangePinRequested();
			mController->onChangePinRequested();
			QTest::ignoreMessage(QtDebugMsg, "governikus::ChangePinController done");
			QTest::ignoreMessage(QtInfoMsg, "Finished workflow PIN");
			QTest::ignoreMessage(QtDebugMsg, "Running waiting action now.");
			QTest::ignoreMessage(QtInfoMsg, "Starting new workflow PIN");
			mController->onWorkflowFinished();
			QCOMPARE(spyWorkflowFinished.count(), 1);
			QVERIFY(!mController->mWaitingRequest);
			QCOMPARE(mController->mCurrentAction, Action::PIN);
		}


		void test_OnWorkflowFinishedAUTH()
		{
			QSignalSpy spyWorkflowFinished(mController.data(), &AppController::fireWorkflowFinished);

			QSharedPointer<MockActivationContext> context(new MockActivationContext());
			mController->onSelfAuthenticationRequested();
			mController->mActiveController->getContext()->setWorkflowFinished(true);
			mController->onAuthenticationRequest(context);
			QTest::ignoreMessage(QtDebugMsg, "governikus::SelfAuthController done");
			QTest::ignoreMessage(QtInfoMsg, "Finished workflow SELF");
			QTest::ignoreMessage(QtDebugMsg, "Running waiting action now.");
			QTest::ignoreMessage(QtInfoMsg, "Starting new workflow AUTH");
			mController->onWorkflowFinished();
			QCOMPARE(spyWorkflowFinished.count(), 1);
			QVERIFY(!mController->mWaitingRequest);
			QCOMPARE(mController->mCurrentAction, Action::AUTH);
		}


		void test_OnAuthenticationRequestSELF()
		{
			const QSharedPointer<MockActivationContext> context(new MockActivationContext());

			mController->onSelfAuthenticationRequested();
			mController->mActiveController->getContext()->setWorkflowFinished(true);
			QTest::ignoreMessage(QtDebugMsg, "Authentication requested");
			QTest::ignoreMessage(QtDebugMsg, "Auto-approving the current state");
			mController->onAuthenticationRequest(context);
			QVERIFY(mController->mActiveController->getContext()->isStateApproved());

			mController->onSelfAuthenticationRequested();
			QTest::ignoreMessage(QtWarningMsg, "Cannot enqueue action AUTH , queue is already full.");
			mController->onAuthenticationRequest(context);
		}


		void test_OnAuthenticationRequestCannotSendOperationAlreadyActive()
		{
			const QString sendError("send error");
			const QSharedPointer<MockActivationContext> context(new MockActivationContext(false, false, false, false, sendError));
			mController->onChangePinRequested();
			QTest::ignoreMessage(QtCriticalMsg, R"(Cannot send "Operation already active" to caller: "send error")");
			mController->onAuthenticationRequest(context);
		}


		void test_DoShutdownActiveController()
		{
			QSignalSpy spyHideUi(mController.data(), &AppController::fireHideUi);

			mController->onChangePinRequested();
			mController->onChangePinRequested();
			mController->doShutdown();
			QVERIFY(!mController->mWaitingRequest);
			QVERIFY(mController->mActiveController->getContext()->isWorkflowKilled());
			QCOMPARE(spyHideUi.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_AppController)
#include "test_AppController.moc"
