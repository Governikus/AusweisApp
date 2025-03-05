/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgContext.h"

#include "ReaderManager.h"
#include "context/AuthContext.h"
#include "messages/MsgHandler.h"
#include "messages/MsgHandlerEnterPin.h"
#include "messages/MsgHandlerInsertCard.h"

#include "TestWorkflowContext.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_MsgContext
	: public QObject
{
	Q_OBJECT

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
			MsgDispatcherContext ctx;
			const MsgContext& readOnly = ctx;

			QCOMPARE(readOnly.getApiLevel(), MsgHandler::DEFAULT_MSG_LEVEL);
			ctx.setApiLevel(MsgLevel::v1);
			QCOMPARE(readOnly.getApiLevel(), MsgLevel::v1);
		}


		void stateMsg()
		{
			MsgDispatcherContext ctx;
			ctx.setWorkflowContext(QSharedPointer<TestWorkflowContext>::create());
			const MsgContext& readOnly = ctx;

			QCOMPARE(readOnly.getLastStateMsg(), MsgType::VOID);
			QVERIFY(!readOnly.getLastStateMsg());
			ctx.addStateMsg(MsgHandlerInsertCard());
			QCOMPARE(readOnly.getLastStateMsg(), MsgType::INSERT_CARD);
			QVERIFY(readOnly.getLastStateMsg());
			ctx.addStateMsg(MsgHandlerEnterPin(ctx));
			QCOMPARE(readOnly.getLastStateMsg(), MsgType::ENTER_PIN);
			QVERIFY(readOnly.getLastStateMsg());
			ctx.clear();
			QCOMPARE(readOnly.getLastStateMsg(), MsgType::VOID);
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

			ctx.setWorkflowContext(QSharedPointer<TestWorkflowContext>::create());
			QVERIFY(readOnly.isActiveWorkflow());
			QVERIFY(!readOnly.getContext<AuthContext>());
			QVERIFY(!ctx.getContext<AuthContext>());
			QVERIFY(readOnly.getContext());

			ctx.clear();
			QVERIFY(!readOnly.isActiveWorkflow());
			QVERIFY(!readOnly.getContext<AuthContext>());
			QVERIFY(!ctx.getContext<AuthContext>());
			QVERIFY(!readOnly.getContext());

			ctx.setWorkflowContext(QSharedPointer<AuthContext>(new AuthContext(true, QUrl("http://www.bla.de"_L1))));
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
