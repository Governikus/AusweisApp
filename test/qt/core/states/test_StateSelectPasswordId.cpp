/*!
 * \brief Tests the StateSelectPasswordId
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateSelectPasswordId.h"

#include "states/StateBuilder.h"

#include <QtTest>

using namespace governikus;


class test_StateSelectPasswordId
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_Run()
		{
			QSharedPointer<WorkflowContext> context(new WorkflowContext());
			const auto& state = StateBuilder::createState<StateSelectPasswordId>(context);
			QSignalSpy spyPasswordIdCAN(state, &StateSelectPasswordId::firePasswordIdCAN);
			QSignalSpy spyContinue(state, &StateSelectPasswordId::fireContinue);

			context->setCanAllowedMode(true);
			context->setStateApproved();
			QTest::ignoreMessage(QtDebugMsg, "CAN allowed: true");
			QTest::ignoreMessage(QtDebugMsg, "Running state \"StateSelectPasswordId\"");
			state->onStateApprovedChanged();
			QCOMPARE(spyPasswordIdCAN.count(), 1);

			context->setCanAllowedMode(false);
			QTest::ignoreMessage(QtDebugMsg, "CAN allowed: false");
			QTest::ignoreMessage(QtDebugMsg, "Running state \"StateSelectPasswordId\"");
			state->onStateApprovedChanged();
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateSelectPasswordId)
#include "test_StateSelectPasswordId.moc"
