/*!
 * \brief Unit tests for \ref WorkflowRequest
 *
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowRequest.h"

#include "ReaderManager.h"
#include "TestWorkflowContext.h"
#include "TestWorkflowController.h"
#include "controller/WorkflowController.h"

#include <QtTest>

using namespace governikus;


class test_WorkflowRequest
	: public QObject
{
	Q_OBJECT

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


		void initialize()
		{
			auto request = WorkflowRequest::createWorkflowRequest<TestWorkflowController, TestWorkflowContext>();

			QVERIFY(!request->isInitialized());
			QCOMPARE(request->getAction(), Action::AUTH);
			QVERIFY(!request->getContext().isNull());
			QVERIFY(request->getController().isNull());

			request->initialize();

			QVERIFY(request->isInitialized());
			QCOMPARE(request->getAction(), Action::AUTH);
			QVERIFY(!request->getContext().isNull());
			QVERIFY(!request->getController().isNull());
		}


};

QTEST_GUILESS_MAIN(test_WorkflowRequest)
#include "test_WorkflowRequest.moc"
