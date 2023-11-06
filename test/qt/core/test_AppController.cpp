/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

#include "controller/AppController.h"

#include "ReaderManager.h"
#include "context/ChangePinContext.h"
#include "controller/AuthController.h"
#include "controller/ChangePinController.h"
#include "controller/SelfAuthController.h"

#include "TestWorkflowController.h"

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
			qRegisterMetaType<QSharedPointer<WorkflowContext>>("QSharedPointer<WorkflowContext>");

			const auto readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void init()
		{
			mController.reset(new AppController());
		}


		void cleanup()
		{
			mController.clear();
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void test_StartNewWorkflow()
		{
			connect(mController.data(), &AppController::fireWorkflowStarted, this, [this](const QSharedPointer<WorkflowRequest> pRequest){
					pRequest->getContext()->claim(this);
				});

			QSignalSpy spy(mController.data(), &AppController::fireWorkflowStarted);
			QTest::ignoreMessage(QtInfoMsg, "Started new workflow PIN");
			QTest::ignoreMessage(QtDebugMsg, "Start governikus::ChangePinController");
			QVERIFY(mController->startNewWorkflow(ChangePinController::createWorkflowRequest()));
			QVERIFY(mController->mActiveWorkflow->getController()->mContext->wasClaimed());
			QCOMPARE(mController->mActiveWorkflow->getController()->mContext->getAction(), Action::PIN);
			QCOMPARE(spy.count(), 1);

			QTest::ignoreMessage(QtWarningMsg, "Cannot start new workflow: PIN | Current workflow: PIN");
			QVERIFY(!mController->startNewWorkflow(ChangePinController::createWorkflowRequest()));
		}


		void test_OnWorkflowFinishedNoWaitingRequest()
		{
			QSignalSpy spyWorkflowFinished(mController.data(), &AppController::fireWorkflowFinished);

			mController->onWorkflowRequested(AuthController::createWorkflowRequest(QUrl()));
			QTest::ignoreMessage(QtDebugMsg, "governikus::AuthController done");
			QTest::ignoreMessage(QtInfoMsg, "Finish workflow AUTH");
			mController->onWorkflowFinished();
			QCOMPARE(spyWorkflowFinished.count(), 1);
		}


		void test_OnWorkflowRemoteServiceRequest()
		{
			QSignalSpy spyWorkflowFinished(mController.data(), &AppController::fireWorkflowFinished);

			mController->onWorkflowRequested(TestWorkflowController::createWorkflowRequest(false, Action::REMOTE_SERVICE));
			QTest::ignoreMessage(QtDebugMsg, "governikus::TestWorkflowController done");
			QTest::ignoreMessage(QtInfoMsg, "Finish workflow REMOTE_SERVICE");
			mController->onWorkflowFinished();
			QCOMPARE(spyWorkflowFinished.count(), 1);
		}


		void test_OnWorkflowFinishedPIN()
		{
			QSignalSpy spyWorkflowFinished(mController.data(), &AppController::fireWorkflowFinished);

			mController->onWorkflowRequested(ChangePinController::createWorkflowRequest());
			mController->onWorkflowRequested(ChangePinController::createWorkflowRequest());
			QTest::ignoreMessage(QtDebugMsg, "governikus::ChangePinController done");
			QTest::ignoreMessage(QtInfoMsg, "Finish workflow PIN");
			QTest::ignoreMessage(QtDebugMsg, "Running waiting action now.");
			QTest::ignoreMessage(QtInfoMsg, "Started new workflow PIN");
			mController->onWorkflowFinished();
			QCOMPARE(spyWorkflowFinished.count(), 1);
			QVERIFY(!mController->mWaitingRequest);
			QVERIFY(mController->mActiveWorkflow);
			QCOMPARE(mController->mActiveWorkflow->getController()->mContext->getAction(), Action::PIN);
		}


		void test_OnWorkflowFinishedAUTH()
		{
			QSignalSpy spyWorkflowFinished(mController.data(), &AppController::fireWorkflowFinished);

			mController->onWorkflowRequested(SelfAuthController::createWorkflowRequest());
			mController->mActiveWorkflow->getController()->mContext->setWorkflowFinished(true);
			mController->onWorkflowRequested(AuthController::createWorkflowRequest(QUrl(QStringLiteral("https://localhost"))));
			QTest::ignoreMessage(QtDebugMsg, "governikus::SelfAuthController done");
			QTest::ignoreMessage(QtInfoMsg, "Finish workflow SELF");
			QTest::ignoreMessage(QtDebugMsg, "Running waiting action now.");
			QTest::ignoreMessage(QtInfoMsg, "Started new workflow AUTH");
			mController->onWorkflowFinished();
			QCOMPARE(spyWorkflowFinished.count(), 1);
			QVERIFY(!mController->mWaitingRequest);
			QCOMPARE(mController->mActiveWorkflow->getController()->mContext->getAction(), Action::AUTH);
		}


		void test_OnWorkflowUnhandled()
		{
			QSignalSpy spyWorkflowStarted(mController.data(), &AppController::fireWorkflowStarted);
			QSignalSpy spyWorkflowUnhandled(mController.data(), &AppController::fireWorkflowUnhandled);

			QTest::ignoreMessage(QtInfoMsg, "Started new workflow AUTH");
			mController->onWorkflowRequested(AuthController::createWorkflowRequest(QUrl()));
			QCOMPARE(spyWorkflowUnhandled.count(), 0);
			QCOMPARE(spyWorkflowStarted.count(), 1);

			QTest::ignoreMessage(QtWarningMsg, "Skip workflow: AUTH | Current workflow: AUTH");
			mController->onWorkflowRequested(AuthController::createWorkflowRequest(QUrl()));
			QCOMPARE(spyWorkflowUnhandled.count(), 1);
			QCOMPARE(spyWorkflowStarted.count(), 1);
		}


		void test_OnAuthenticationRequestSELF()
		{
			connect(mController.data(), &AppController::fireWorkflowStarted, this, [this](const QSharedPointer<WorkflowRequest> pRequest){
					pRequest->getContext()->claim(this);
				});

			QTest::ignoreMessage(QtDebugMsg, "New workflow requested: SELF");
			QTest::ignoreMessage(QtDebugMsg, "Start governikus::SelfAuthController");
			QTest::ignoreMessage(QtInfoMsg, "Started new workflow SELF");
			mController->onWorkflowRequested(SelfAuthController::createWorkflowRequest());
			QVERIFY(mController->mActiveWorkflow->getController()->mContext->wasClaimed());
			mController->mActiveWorkflow->getController()->mContext->setWorkflowFinished(true);
			QVERIFY(!mController->mActiveWorkflow->getController()->mContext->isStateApproved());

			QTest::ignoreMessage(QtDebugMsg, "New workflow requested: AUTH");
			QTest::ignoreMessage(QtDebugMsg, "Auto-approving the current state");
			QTest::ignoreMessage(QtDebugMsg, "Enqueue workflow: AUTH");
			mController->onWorkflowRequested(AuthController::createWorkflowRequest(QUrl()));
			QVERIFY(mController->mActiveWorkflow->getController()->mContext->isStateApproved());

			QTest::ignoreMessage(QtDebugMsg, "New workflow requested: SELF");
			QTest::ignoreMessage(QtWarningMsg, "Cannot start or enqueue workflow: SELF");
			mController->onWorkflowRequested(SelfAuthController::createWorkflowRequest());

			QTest::ignoreMessage(QtDebugMsg, "New workflow requested: AUTH");
			QTest::ignoreMessage(QtWarningMsg, "Skip workflow: AUTH | Current workflow: SELF");
			QTest::ignoreMessage(QtDebugMsg, "Waiting workflow: AUTH");
			mController->onWorkflowRequested(AuthController::createWorkflowRequest(QUrl()));
		}


		void test_notClaimed()
		{
			QTest::ignoreMessage(QtDebugMsg, "New workflow requested: SELF");
			QTest::ignoreMessage(QtDebugMsg, "Start governikus::SelfAuthController");
			QTest::ignoreMessage(QtInfoMsg, "Started new workflow SELF");
			QTest::ignoreMessage(QtCriticalMsg, "Workflow was not claimed by any UI... aborting");
			QTest::ignoreMessage(QtWarningMsg, "Killing the current workflow.");

			mController->onWorkflowRequested(SelfAuthController::createWorkflowRequest());
			QVERIFY(!mController->mActiveWorkflow->getController()->mContext->wasClaimed());
		}


		void test_DoShutdownActiveController()
		{
			QSignalSpy spyHideUi(mController.data(), &AppController::fireHideUi);

			mController->onWorkflowRequested(ChangePinController::createWorkflowRequest());
			mController->onWorkflowRequested(ChangePinController::createWorkflowRequest());
			mController->doShutdown();
			QVERIFY(!mController->mWaitingRequest);
			QVERIFY(mController->mActiveWorkflow->getController()->mContext->isWorkflowKilled());
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
