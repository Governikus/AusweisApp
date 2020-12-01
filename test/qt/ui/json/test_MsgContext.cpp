/*!
 * \brief Unit tests for \ref MsgContext
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgContext.h"

#include "context/AuthContext.h"
#include "InternalActivationContext.h"
#include "messages/MsgHandler.h"

#include <QSignalSpy>
#include <QtTest>

using namespace governikus;

class test_MsgContext
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
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
			const MsgContext& readOnly = ctx;

			QCOMPARE(readOnly.getLastStateMsg(), MsgType::INTERNAL_ERROR);
			ctx.addStateMsg(MsgType::INSERT_CARD);
			QCOMPARE(readOnly.getLastStateMsg(), MsgType::INSERT_CARD);
			ctx.addStateMsg(MsgType::ENTER_PIN);
			QCOMPARE(readOnly.getLastStateMsg(), MsgType::ENTER_PIN);
			ctx.clear();
			QCOMPARE(readOnly.getLastStateMsg(), MsgType::INTERNAL_ERROR);
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
