/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref StateUpdateRetryCounter
 */

#include "states/StateUpdateRetryCounter.h"

#include "MockCardConnection.h"
#include "MockCardConnectionWorker.h"
#include "TestWorkflowContext.h"

#include <QtTest>

using namespace governikus;

class MockCardCommand
	: public BaseCardCommand
{
	Q_OBJECT

	public:
		explicit MockCardCommand(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
			: BaseCardCommand(pCardConnectionWorker)
		{
		}


		void setMockReturnCode(CardReturnCode pReturnCode)
		{
			setReturnCode(pReturnCode);
		}


		void internalExecute() override
		{
			setReturnCode(getCardConnectionWorker()->updateRetryCounter());
		}


};


class test_StateUpdateRetryCounter
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_RunWithoutCardConnection()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			StateUpdateRetryCounter counter(context);
			QSignalSpy spyContinue(&counter, &StateUpdateRetryCounter::fireContinue);
			QSignalSpy spyAbort(&counter, &StateUpdateRetryCounter::fireAbort);
			QSignalSpy spyNoCardConnection(&counter, &StateUpdateRetryCounter::fireNoCardConnection);

			QTest::ignoreMessage(QtDebugMsg, "No card connection available.");
			counter.run();
			QCOMPARE(spyContinue.count(), 0);
			QCOMPARE(spyAbort.count(), 0);
			QCOMPARE(spyNoCardConnection.count(), 1);
			QVERIFY(!context->getFailureCode().has_value());
		}


		void test_OnUpdateRetryCounterDone()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			StateUpdateRetryCounter counter(context);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QSharedPointer<MockCardCommand> command(new MockCardCommand(worker));
			QSignalSpy spyContinue(&counter, &StateUpdateRetryCounter::fireContinue);
			QSignalSpy spyAbort(&counter, &StateUpdateRetryCounter::fireAbort);
			QSignalSpy spyNoCardConnection(&counter, &StateUpdateRetryCounter::fireNoCardConnection);

			QTest::ignoreMessage(QtDebugMsg, "StateUpdateRetryCounter::onUpdateRetryCounterDone()");
			QTest::ignoreMessage(QtCriticalMsg, "An error (UNKNOWN) occurred while communicating with the card reader, cannot determine retry counter, abort state");
			counter.onUpdateRetryCounterDone(command);
			QCOMPARE(spyAbort.count(), 0);
			QCOMPARE(spyNoCardConnection.count(), 1);
			QVERIFY(!context->getFailureCode().has_value());
			command->setMockReturnCode(CardReturnCode::OK);
			context->setCardConnection(QSharedPointer<MockCardConnection>::create());
			QTest::ignoreMessage(QtDebugMsg, "StateUpdateRetryCounter::onUpdateRetryCounterDone()");
			counter.onUpdateRetryCounterDone(command);
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateUpdateRetryCounter)
#include "test_StateUpdRetryCounter.moc"
