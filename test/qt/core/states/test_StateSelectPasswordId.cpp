/*!
 * \brief Tests the StateSelectPasswordId
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateSelectPasswordId.h"

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
			StateSelectPasswordId id(context);
			QSignalSpy spyPasswordIdCAN(&id, &StateSelectPasswordId::firePasswordIdCAN);
			QSignalSpy spyContinue(&id, &StateSelectPasswordId::fireContinue);

			context->setCanAllowedMode(true);
			QTest::ignoreMessage(QtDebugMsg, "CAN allowed: true");
			id.run();
			QCOMPARE(spyPasswordIdCAN.count(), 1);

			context->setCanAllowedMode(false);
			QTest::ignoreMessage(QtDebugMsg, "CAN allowed: false");
			id.run();
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateSelectPasswordId)
#include "test_StateSelectPasswordId.moc"
