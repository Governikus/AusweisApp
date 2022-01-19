/*!
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#include "controller/AppController.h"

#include "context/ChangePinContext.h"
#include "controller/ChangePinController.h"

#include "MockActivationContext.h"

#include <QDir>
#include <QSharedPointer>
#include <QStandardPaths>
#include <QtTest>


using namespace governikus;


class test_AppController
	: public QObject
{
	Q_OBJECT
	QSharedPointer<AppController> mController;

	private Q_SLOTS:
		void initTestCase()
		{
			qRegisterMetaType<QSharedPointer<WorkflowContext> >("QSharedPointer<WorkflowContext>");
		}


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

			mController->onAuthenticationRequest(QUrl());
			QTest::ignoreMessage(QtDebugMsg, "governikus::AuthController done");
			QTest::ignoreMessage(QtInfoMsg, "Finished workflow AUTH");
			mController->onWorkflowFinished();
			QCOMPARE(spyWorkflowFinished.count(), 1);
		}


		void test_OnWorkflowRemoteServiceRequest()
		{
			#if !__has_include("RemoteClient.h")
			QSKIP("RemoteClient is disabled");
			#endif

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

			mController->onChangePinRequested(false);
			mController->onChangePinRequested(false);
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

			mController->onSelfAuthenticationRequested();
			mController->mActiveController->getContext()->setWorkflowFinished(true);
			mController->onAuthenticationRequest(QUrl(QStringLiteral("https://localhost")));
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
			mController->onAuthenticationContextRequest(context);
			QVERIFY(mController->mActiveController->getContext()->isStateApproved());

			mController->onSelfAuthenticationRequested();
			QTest::ignoreMessage(QtWarningMsg, "Cannot enqueue action AUTH , queue is already full.");
			mController->onAuthenticationContextRequest(context);
		}


		void test_OnAuthenticationRequestCannotSendOperationAlreadyActive()
		{
			const QString sendError("send error");
			const QSharedPointer<MockActivationContext> context(new MockActivationContext(false, false, false, false, sendError));
			mController->onChangePinRequested(false);
			QTest::ignoreMessage(QtCriticalMsg, R"(Cannot send "Operation already active" to caller: "send error")");
			mController->onAuthenticationContextRequest(context);
		}


		void test_DoShutdownActiveController()
		{
			QSignalSpy spyHideUi(mController.data(), &AppController::fireHideUi);

			mController->onChangePinRequested(false);
			mController->onChangePinRequested(false);
			mController->doShutdown();
			QVERIFY(!mController->mWaitingRequest);
			QVERIFY(mController->mActiveController->getContext()->isWorkflowKilled());
			QCOMPARE(spyHideUi.count(), 1);
		}


		void test_ClearCacheFolders()
		{
			QStandardPaths::setTestModeEnabled(true);

			const auto testFileName = QStringLiteral("testFile.test");
			const auto testDirName = QStringLiteral("testDir");
			const QStringList cachePaths = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);

			for (const QString& cachePath : cachePaths)
			{
				auto cacheDir = QDir(cachePath);
				auto testDir = QDir(cacheDir.filePath(testDirName));
				auto testFile = QFile(cacheDir.filePath(testFileName));
				auto testDirFile = QFile(testDir.filePath(testFileName));

				cacheDir.mkpath(testDir.absolutePath());
				QVERIFY(testFile.open(QIODevice::WriteOnly));
				QVERIFY(testDirFile.open(QIODevice::WriteOnly));
				cacheDir.refresh();

				QVERIFY(cacheDir.exists());
				QVERIFY(!cacheDir.isEmpty());
				QVERIFY(testDir.exists());
				QVERIFY(testFile.exists());
				QVERIFY(testDirFile.exists());
			}

			AppController::clearCacheFolders();

			for (const QString& cachePath : cachePaths)
			{
				auto cacheDir = QDir(cachePath);
				auto testDir = QDir(cacheDir.filePath(testDirName));

				QVERIFY(cacheDir.exists());
				QVERIFY(cacheDir.isEmpty());
				QVERIFY(!testDir.exists());
				QVERIFY(!QFile::exists(cacheDir.filePath(testFileName)));
				QVERIFY(!QFile::exists(testDir.filePath(testFileName)));
			}

			QStandardPaths::setTestModeEnabled(false);
		}


};

QTEST_GUILESS_MAIN(test_AppController)
#include "test_AppController.moc"
