/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref WorkflowRequest
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
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void initialize()
		{
			auto request = WorkflowRequest::create<TestWorkflowController, TestWorkflowContext>();

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
