/*!
 * \brief Unit tests for \ref MsgContext
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgContext.h"

#include "context/AuthContext.h"
#include "context/WorkflowContext.h"
#include "InternalActivationContext.h"
#include "messages/MsgHandlerEnterPin.h"
#include "messages/MsgHandlerInsertCard.h"
#include "ReaderManager.h"

#include <QtTest>

using namespace governikus;

class test_MsgContext
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


		void apiLevel()
		{
			MsgDispatcherContext ctx;
			const MsgContext& readOnly = ctx;

			QCOMPARE(readOnly.getApiLevel(), MsgHandler::DEFAULT_MSG_LEVEL);
			ctx.setApiLevel(MsgLevel::v1);
			QCOMPARE(readOnly.getApiLevel(), MsgLevel::v1);
		}


		void stateMsg()
		{
			MsgDispatcherContext ctx;
			ctx.setWorkflowContext(QSharedPointer<WorkflowContext>::create());
			const MsgContext& readOnly = ctx;

			QCOMPARE(readOnly.getLastStateMsg(), MsgType::INTERNAL_ERROR);
			QVERIFY(!readOnly.getLastStateMsg());
			ctx.addStateMsg(MsgHandlerInsertCard());
			QCOMPARE(readOnly.getLastStateMsg(), MsgType::INSERT_CARD);
			QVERIFY(readOnly.getLastStateMsg());
			ctx.addStateMsg(MsgHandlerEnterPin(ctx));
			QCOMPARE(readOnly.getLastStateMsg(), MsgType::ENTER_PIN);
			QVERIFY(readOnly.getLastStateMsg());
			ctx.clear();
			QCOMPARE(readOnly.getLastStateMsg(), MsgType::INTERNAL_ERROR);
			QVERIFY(!readOnly.getLastStateMsg());
		}


		void workflowContext()
		{
			MsgDispatcherContext ctx;
			const MsgContext& readOnly = ctx;

			QVERIFY(!readOnly.isActiveWorkflow());
			QVERIFY(!readOnly.getContext<AuthContext>());
			QVERIFY(!ctx.getContext<AuthContext>());
			QVERIFY(!readOnly.getContext());

			ctx.setWorkflowContext(QSharedPointer<WorkflowContext>(new WorkflowContext()));
			QVERIFY(readOnly.isActiveWorkflow());
			QVERIFY(!readOnly.getContext<AuthContext>());
			QVERIFY(!ctx.getContext<AuthContext>());
			QVERIFY(readOnly.getContext());

			ctx.clear();
			QVERIFY(!readOnly.isActiveWorkflow());
			QVERIFY(!readOnly.getContext<AuthContext>());
			QVERIFY(!ctx.getContext<AuthContext>());
			QVERIFY(!readOnly.getContext());

			ctx.setWorkflowContext(QSharedPointer<AuthContext>(new AuthContext(QSharedPointer<InternalActivationContext>::create(QUrl("http://www.bla.de")))));
			QVERIFY(readOnly.isActiveWorkflow());
			QVERIFY(readOnly.getContext<AuthContext>());
			QVERIFY(ctx.getContext<AuthContext>());
			QVERIFY(readOnly.getContext());

			QVERIFY(ctx.isActiveWorkflow());
			QVERIFY(ctx.getContext<AuthContext>());
			QVERIFY(ctx.getContext<AuthContext>());
			QVERIFY(ctx.getContext());
		}


};

QTEST_GUILESS_MAIN(test_MsgContext)
#include "test_MsgContext.moc"
